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
    __DSB(); /* carry out previous operations before booting M0 */
    /* Make sure the M0 core is being held in reset via the RGU */
    Chip_RGU_TriggerReset(RGU_M0APP_RST);
    Chip_Clock_Enable(CLK_M4_M0APP);
    Chip_CREG_SetM0AppMemMap(m0_image_addr); /* the reset vector location */
    Chip_RGU_ClearReset(RGU_M0APP_RST);
    return 0;
}


/* Set up and initialize hardware prior to call to main */
void SystemInit(void)
{
#if defined(CORE_M4)
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

#if defined(BOOT_M0)
    extern void * __VMATextM0;
    M0Image_Boot((uint32_t) &__VMATextM0);
#endif

#endif /* defined(CORE_M3) || defined(CORE_M4) */
}
