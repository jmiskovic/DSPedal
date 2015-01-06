#include "board.h"

#define BLINK_DELAY 5000

int main_M0() {
    volatile unsigned int i;
    Board_LED_Set(1);
    for(;;) {
        i++;
        Board_LED_Set(0); Board_Delay(BLINK_DELAY * 1000); Board_LED_Set(1); Board_Delay(BLINK_DELAY * 1000);
    }
    return 0;
}
