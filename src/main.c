#include "board.h"
#include "sound.h"
#include "sysinit.h"

#define NUM_CHECK 21
#define BLINK_DELAY 5000

uint32_t msec = 0;

int main() {
    SystemCoreClockUpdate();
    SystemInit();
    //SysTick_Config(SystemCoreClock / 10000);
    Board_Init();
    DEBUGSTR("boot process completed\n");
    //sound_init();
    int errcode = 0;//= sram_check();
    DEBUGOUT("sram check: %d\n", errcode);
    Board_Blink_Forever(4);
    for(;;) {
        Board_LED_Set(0); Board_Delay(BLINK_DELAY * 1000); Board_LED_Set(1); DEBUGSTR("led on\n"); Board_Delay(BLINK_DELAY * 1000);
    }
    return 0;
}

void SysTick_Handler(void) {
    msec++;
    if (msec % BLINK_DELAY == 0) {
        if ((msec / BLINK_DELAY) % 2 == 0) {
            Board_LED_Set(0);
        }
        else {
            Board_LED_Set(1);
            DEBUGSTR("led on\n");
        }
    }
}