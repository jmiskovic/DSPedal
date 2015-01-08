#include "board.h"
#include "interface.h"

#define BLINK_DELAY 5000

extern char * sharedmem;

int main_M0() {
    volatile unsigned int i;
    DEBUGINIT();
    Board_LED_Set(1);
    Board_Init();
    Board_Accellerometer_Init();
    interface_init();
    for(;;) {
        i+= 1;
        interface_process();
        Board_LED_Set(0); Board_Delay(BLINK_DELAY * 50); Board_LED_Set(1); Board_Delay(BLINK_DELAY * 50);
    }
    interface_init();
    interface_process();
    return 0;
}
