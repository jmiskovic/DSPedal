#include "board.h"
#include "sound.h"
#include "sysinit.h"

#define BLINK_DELAY 5000

uint32_t msec = 0;

int main() {
    SystemCoreClockUpdate();

    NVIC_SetPriority(M0APP_IRQn, 7);
    NVIC_EnableIRQ(M0APP_IRQn);

    //SysTick_Config(SystemCoreClock / 10000);
    Board_Init();
    DEBUGSTR("boot process completed\n");

    sound_init();
    sound_blocking_process();
    return 0;
}

int m0_flag;
extern char * sharedmem;

void main_periodic_task(void) {
    if (!m0_flag)
        return;
    DEBUGOUT("M0: %s\n", sharedmem);
    m0_flag = 0;
}

void M0APP_IRQHandler(void) {
    Chip_CREG_ClearM0AppEvent();
    m0_flag = 1;
}

void SysTick_Handler(void) {
    msec++;
    if (msec % BLINK_DELAY == 0) {
        if ((msec / BLINK_DELAY) % 2 == 0) {
            Board_LED_Set(0);
        }
        else {
            Board_LED_Set(1);
        }
    }
}
