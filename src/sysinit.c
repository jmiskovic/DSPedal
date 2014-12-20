#if defined(NO_BOARD_LIB)
#include "chip.h"
const uint32_t ExtRateIn = 0;
const uint32_t OscRateIn = 12000000;
#else
#include "board.h"
#endif

/* Set up and initialize hardware prior to call to main */
void SystemInit(void)
{


#if defined(CORE_M3) || defined(CORE_M4)
    unsigned int *pSCB_VTOR = (unsigned int *) 0xE000ED08;

    extern void (* const g_pfnVectors[])(void);
    LPC_CREG->MXMEMMAP = (unsigned int) &g_pfnVectors;
/*    *pSCB_VTOR = (unsigned int) &g_pfnVectors; */
    *pSCB_VTOR = 0x00000000;


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

#endif /* defined(CORE_M3) || defined(CORE_M4) */
}
