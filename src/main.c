#include "board.h"
#include "sound.h"
#include "sysinit.h"


uint32_t msec = 0;
#define NUM_CHECK 21
int sram_check() {
	volatile unsigned int i = 1;
	unsigned int comp_sum = 0, read_sum = 0;
    volatile unsigned int * p = (unsigned int *) 0x28000000;

	for (i = 0; i < NUM_CHECK; i++) {
		*p = i;
		DEBUGOUT("%04x ", *p);
		p++;
		comp_sum += i;
	}
	DEBUGOUT("\nreadout\n");
	p = (unsigned int *) 0x28000000;
	for (i = 0; i < NUM_CHECK; i++) {
		read_sum += *p;
		DEBUGOUT("%04x ", *p);
		p++;
	}
	return comp_sum == read_sum;
}

void delay(unsigned n) {
	while(n--) {}
}

int main()
{
    SystemCoreClockUpdate();
    SystemInit();
    SysTick_Config(SystemCoreClock / 10000);
    Board_Init();
    DEBUGSTR("boot process completed\n");
    //sound_init();

    int errcode = 0;//= sram_check();
    DEBUGOUT("sram check: %d\n", errcode);
    if (errcode == 1)
    	for(;;) {
    		Board_LED_Set(1);
    	}
    for(;;)
    {
/*        Board_LED_Set(0);
        delay(10000000);
        Board_LED_Set(1);
    	delay(10000000);*/
        //DEBUGSTR("hi\n");
    }
    return 0;
}

void USB1_IRQHandler(void) {
    for(;;)
    {
        Board_LED_Set(0);
        delay(1000000);
        Board_LED_Set(1);
    	delay(1000000);
        //DEBUGSTR("hi\n");
    }
}

#define BLINK_DELAY 5000

void SysTick_Handler(void)
{
    msec++;
    if (msec % BLINK_DELAY == 0) {
        if ((msec / BLINK_DELAY) % 2 == 0) {
            Board_LED_Set(0);
            DEBUGSTR("led off\n");
        }
        else {
            Board_LED_Set(1);
            DEBUGSTR("led on\n");
        }
    }
}