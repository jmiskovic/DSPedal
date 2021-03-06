#include "board.h"
#include "interface.h"

#define BLINK_DELAY 5000

extern char * sharedmem;

int main_M0() {
    volatile unsigned int i;
    Board_Delay(BLINK_DELAY * 5000);
    DEBUGINIT();
    Board_Init();
    interface_init();
    for(;;) {
        i+= 1;
        interface_process();
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
