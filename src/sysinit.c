#if defined(NO_BOARD_LIB)
#include "chip.h"
const uint32_t ExtRateIn = 0;
const uint32_t OscRateIn = 12000000;
#else
#include "board.h"
#endif

static int M0Image_Boot(uint32_t m0_image_addr)
{
    if (m0_image_addr & 0xFFF) { /* Make sure the alignment is OK */
        return -1;
    }

    /* Make sure the M0 core is being held in reset via the RGU */
    Chip_RGU_TriggerReset(RGU_M0APP_RST);
    Chip_Clock_Enable(CLK_M4_M0APP);
    Chip_CREG_SetM0AppMemMap(m0_image_addr);
    Chip_RGU_ClearReset(RGU_M0APP_RST);
    return 0;
}


/* Set up and initialize hardware prior to call to main */
void SystemInit(void)
{
#if defined(CORE_M3) || defined(CORE_M4)
    unsigned int *pSCB_VTOR = (unsigned int *) 0xE000ED08;

    extern void (* const g_pfnVectors[])(void);
/*    LPC_CREG->MXMEMMAP = (unsigned int) &g_pfnVectors; */
/*    *pSCB_VTOR = (unsigned int) &g_pfnVectors; */
    *pSCB_VTOR = (unsigned int) &g_pfnVectors;


#if defined(__FPU_PRESENT) && __FPU_PRESENT == 1
    fpuInit();
#endif

#if defined(NO_BOARD_LIB)
    /* Chip specific SystemInit */
    Chip_SystemInit();
#else
    /* Board specific SystemInit */
    Board_SystemInit();
#endif

#if defined(BOOT_CORE_M0)
    extern void * __core_m0app_START__;
    M0Image_Boot((uint32_t) &__core_m0app_START__);
#endif

#endif /* defined(CORE_M3) || defined(CORE_M4) */
}
