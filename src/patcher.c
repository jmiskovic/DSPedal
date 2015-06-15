#include "patcher.h"
#include "board_api.h"
#include "spifilib_api.h"
#include "faust.h"
#include "sound.h"
#include "board.h"

#define MAX_EFFECT_COUNT (32) // how many effects can be managed by patcher
static const uint32_t SPIFLASH_EFFECT_SIZE = 1 * 1024 * 1024;
static const uint32_t DELIMITER_SIZE = 3;

static const uint32_t SPIFLASH_SIZE = 0x2000000;
static const uint32_t SPIFLASH_BASE_ADDRESS = 0x14000000;

static const char magic_pattern = '@';
static const char delimiter[3] = "_|_";

static uint32_t lmem[21]; // Local memory, 32-bit aligned that will be used for driver context (handle)

// defined by linker
extern unsigned int __end_of_text_M0; // first available address after code is loaded
extern unsigned int __end_of_text_M4;
extern unsigned int __size_of_text_M0; // total size of this memory section
extern unsigned int __size_of_text_M4;

static void * last_addressM0 = (void *) &__end_of_text_M0;
static void * last_addressM4 = (void *) &__end_of_text_M4;
static void * last_addressData = (void*) 0x28000000; // SDRAM logical address

struct EffectInfo {
    uint8_t magicPattern;
    char name[15];
    uint16_t size_M0guicode;
    uint16_t size_M0initcode;
    uint16_t size_M4dspcode;
    uint32_t size_data;
};

struct Effect {
    struct Effect * next;
    struct EffectInfo info;
    uint8_t memindex;
    mydsp * data;
    void (* buildUserInterfacemydsp) (mydsp* dsp, UIGlue* interface);
    void (* initmydsp) (mydsp* dsp, int samplingFreq);
    void (* computemydsp) (mydsp* dsp, int count, FAUSTFLOAT** inputs, FAUSTFLOAT** outputs);
} __attribute__ ((aligned (32)));

static uint8_t effect_pool_size = 0;
static struct Effect effect_pool[MAX_EFFECT_COUNT];

static struct Effect * available_effects = NULL;
static struct Effect * active_effects = NULL;

static SPIFI_HANDLE_T *pSpifi;

static void list_push(struct Effect ** phead, struct Effect * add);
static struct Effect * list_pop(struct Effect ** phead);
static void list_preappend(struct Effect ** phead, struct Effect * add);
static void list_remove(struct Effect ** phead, struct Effect * del);
static bool readHeader(struct EffectInfo * info, uint8_t memindex);
static void construct_available_list();
static void load_code(struct Effect * e);
static void unload_code(struct Effect * e);
static void fatalError(char *str, SPIFI_ERR_T errNum);
static SPIFI_HANDLE_T *spifi_init();
static SPIFI_ERR_T flash_read(uint32_t * destination, uint32_t * source, uint32_t length);
static SPIFI_ERR_T flash_program(uint32_t * destination, uint32_t * source, uint32_t length);

void patcher_init()
{
    SPIFI_ERR_T errCode;

    pSpifi = spifi_init();
    spifiDevGetFamilyName(pSpifi);

    errCode = spifiDevUnlockDevice(pSpifi);
    if (errCode != SPIFI_ERR_NONE) {
        fatalError("unlockDevice", errCode);
    }

    spifiDevSetMemMode(pSpifi, false);
    spifiDevSetOpts(pSpifi, SPIFI_OPT_USE_QUAD, true);

    for(volatile int b = 0; b == 0;) {} // debugger barrier
    construct_available_list();

    // don't execute but include code for debugging purposes
    volatile int b = 0;
    if(b != 0) {
        flash_read(NULL, NULL, 0);
        flash_program(NULL, NULL, 0);
        spifiDevEraseAll(pSpifi);
        spifiDevPageProgram(pSpifi, SPIFLASH_BASE_ADDRESS, NULL, 256);
        spifiDevRead(pSpifi, SPIFLASH_BASE_ADDRESS, NULL, 256);
        spifiDevGetInfo(pSpifi, SPIFI_INFO_DEVSIZE); // SPIFI_INFO_ERASE_BLOCKS, SPIFI_INFO_ERASE_BLOCKSIZE, SPIFI_INFO_PAGESIZE, SPIFI_INFO_OPTIONS
        activate_effect(&(effect_pool[0]));
        deactivate_effect();
    }
}

void activate_effect(struct Effect * e)
{
    list_remove(&available_effects, e); // order matters! effect can only belong to single stack (because of *next)
    list_push(&active_effects, e);
    load_code(e);
}

void deactivate_effect()
{
    struct Effect * e = list_pop(&active_effects);
    list_preappend(&available_effects, e);
    unload_code(e);
}

/***************************** linked list helper functions *********************************/

static void list_push(struct Effect ** phead, struct Effect * add) {
    struct Effect * curr = *phead;
    if (curr == NULL) {
        *phead = add;
        return;
    }
    while (curr->next != NULL)
        curr = curr->next;
    curr->next = add;
    add->next = NULL;
}

static struct Effect * list_pop(struct Effect ** phead) {
    struct Effect * curr = *phead;
    struct Effect * ret;
    if (*phead == NULL)
        return NULL;
    if (curr->next == NULL) {
        ret = curr;
        *phead = NULL;
        return ret;
    }

    while (curr->next->next != NULL)
        curr = curr->next;

    ret = curr->next;
    curr->next = NULL;
    return ret;
}

static void list_preappend(struct Effect ** phead, struct Effect * add) {
    add->next = *phead;
    *phead = add;
}

static void list_remove(struct Effect ** phead, struct Effect * del) {
    // warning: function silently failes if there's nothing to delete
    struct Effect * curr = *phead;
    if (*phead == NULL)
        return;
    if (curr == del) {
        *phead = curr->next;
        return;
    }
    while (curr->next != NULL) {
        if (curr->next == del) {
            curr->next = curr->next->next;
            return;
        }
        curr = curr->next;
    }
}

/****************************** loading/unloading effects ***********************************/

static bool readHeader(struct EffectInfo * info, uint8_t memindex) {
    SPIFI_ERR_T errCode;
    struct EffectInfo * eb = (struct EffectInfo *) (SPIFLASH_EFFECT_SIZE * memindex);
    errCode = flash_read((uint32_t *) info, (uint32_t *) eb, sizeof(*info));
    return errCode == SPIFI_ERR_NONE;
}

static void construct_available_list() { // scan FLASH for available effects, add them to list
    struct Effect * e;

    for (int i = 0; i < MAX_EFFECT_COUNT; i++)
    {
        // read header into next free effect from the pool
        if (readHeader(&(effect_pool[effect_pool_size].info), i) && effect_pool[effect_pool_size].info.magicPattern == magic_pattern)
        {
            // take list element from pool and add to available effects
            e = &effect_pool[effect_pool_size];
            list_push(&available_effects, e);
            e->memindex = i;
            effect_pool_size++;
        } else {
            break; // TODO: remove
        }
    }
}

static volatile bool execute_in_place = false;
static volatile __attribute__((used)) struct Effect effect; // dbg
static volatile __attribute__((used)) struct EffectInfo header; // dbg

static void load_code(struct Effect * e) {
    SPIFI_ERR_T errCode;
    uint32_t effect_start_address = SPIFLASH_BASE_ADDRESS + e->memindex * SPIFLASH_EFFECT_SIZE;
    uint32_t M0guicode_address = effect_start_address + sizeof(struct EffectInfo) + DELIMITER_SIZE;
    uint32_t M0initcode_address = M0guicode_address + e->info.size_M0guicode + DELIMITER_SIZE;
    uint32_t M4dspcode_address = M0initcode_address + e->info.size_M0initcode + DELIMITER_SIZE;

    e->initmydsp               = last_addressM0; // executed in this function, not kept in RAM
    e->buildUserInterfacemydsp = last_addressM0;
    e->computemydsp            = last_addressM4;
    e->data                    = last_addressData;

    // TODO: check for space in all memories
    errCode = flash_read((uint32_t *) M0initcode_address, (uint32_t *) e->initmydsp, e->info.size_M0initcode);
    if (errCode != SPIFI_ERR_NONE) fatalError("failed loading M0 init code", errCode);
    
    if (execute_in_place)
        e->initmydsp(e->data, SAMPLE_RATE); // initialize dsp data before overwriting this code block
    
    errCode = flash_read((uint32_t *) M0guicode_address, (uint32_t *) e->buildUserInterfacemydsp, e->info.size_M0guicode);
    if (errCode != SPIFI_ERR_NONE) fatalError("failed loading M0 gui code", errCode);
    errCode = flash_read((uint32_t *) M4dspcode_address, (uint32_t *) e->computemydsp, e->info.size_M4dspcode);
    if (errCode != SPIFI_ERR_NONE) fatalError("failed loading M4 dsp code", errCode);

    if (errCode == SPIFI_ERR_NONE) { // mark memory space as occupied
        last_addressM0   += e->info.size_M0guicode;
        last_addressM4   += e->info.size_M0initcode;
        last_addressData += e->info.size_data;
    }
}

static void unload_code(struct Effect * e) {
    // mark memory space as unoccupied
    last_addressM0   -= e->info.size_M0guicode;
    last_addressM4   -= e->info.size_M4dspcode;
    last_addressData -= e->info.size_data;
}

static void fatalError(char *str, SPIFI_ERR_T errNum) {
    while (1) {
        __WFI();
    }
}

static SPIFI_HANDLE_T *spifi_init() {
    uint32_t memSize;
    SPIFI_HANDLE_T *pReturnVal;

    /* Initialize LPCSPIFILIB library, reset the interface */
    spifiInit(LPC_SPIFI_BASE, true);
    spifiRegisterFamily(spifi_REG_FAMILY_CommonCommandSet); /* new interface for all supported memories */

    /* Get required memory for detected device, this may vary per device family */
    memSize = spifiGetHandleMemSize(LPC_SPIFI_BASE);
    if (memSize == 0) {
        /* No device detected, error */
        fatalError("spi init error - did you pause debugger?", SPIFI_ERR_GEN);
    }

    /* Initialize and detect a device and get device context */
    pReturnVal = spifiInitDevice(&lmem, sizeof(lmem), LPC_SPIFI_BASE, SPIFLASH_BASE_ADDRESS);
    if (pReturnVal == NULL) {
        fatalError("spifiInitDevice", SPIFI_ERR_GEN);
    }
    return pReturnVal;
}

/************************************ flash memory helper functions *************************/

static volatile bool use_flash = false;
static const uint32_t RAM_BASE_ADDRESS = 0x10088000;
#include <string.h>

static SPIFI_ERR_T flash_read(uint32_t * destination, uint32_t * source, uint32_t length) {
    SPIFI_ERR_T errCode = SPIFI_ERR_NONE;
    if (use_flash)
        errCode = spifiRead(pSpifi, SPIFLASH_BASE_ADDRESS + (uint32_t) source, destination, length);
    else
        memcpy(destination, (uint32_t * ) (RAM_BASE_ADDRESS + (uint32_t) source), length);
    return errCode;
 }

static SPIFI_ERR_T flash_program(uint32_t * destination, uint32_t * source, uint32_t length) {
    SPIFI_ERR_T errCode = SPIFI_ERR_NONE;
    if (use_flash) {
        errCode = spifiEraseByAddr(pSpifi, SPIFLASH_BASE_ADDRESS + (uint32_t) destination, SPIFLASH_BASE_ADDRESS + (uint32_t) destination + length);
        if (errCode == SPIFI_ERR_NONE)
            errCode =  spifiProgram(pSpifi, SPIFLASH_BASE_ADDRESS + (uint32_t) destination, source, length);
    } else
        memcpy((uint32_t * ) (RAM_BASE_ADDRESS + (uint32_t) destination), source, length);
    return errCode;
 }
