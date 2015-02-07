#include "board.h"
#include "interface.h"

#define BLINK_DELAY 5000

extern char * sharedmem;

int main_M0() {
    volatile unsigned int i;
    DEBUGINIT();
    Board_LED_Set(1);
    Board_Init();
    //Board_Accellerometer_Init();
    interface_init();
    for(;;) {
        i+= 1;
        interface_process();
        Board_Delay(BLINK_DELAY * 100);
        if (i % 10 == 0) {
            if ((i / 10) % 2 == 1)
                Board_LED_Set(1);
            else
                Board_LED_Set(0);
        }
    }
    interface_init();
    interface_process();
    return 0;
}
