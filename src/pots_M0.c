#include "board.h"
#include "pots.h"

uint8_t pot_channels[POTS_COUNT] = {
    ADC_CH7,        /**< ADC channel 7 */
    ADC_CH5,        /**< ADC channel 5 */
    ADC_CH6,        /**< ADC channel 6 */
};

void pots_init() {
    ADC_CLOCK_SETUP_T ADCSetup;
    Chip_ADC_Init(LPC_ADC0, &ADCSetup);
    Chip_ADC_SetSampleRate(LPC_ADC0, &ADCSetup, 1000); /* 1 kHz sampling */
    Chip_ADC_EnableChannel(LPC_ADC0, ADC_CH5, ENABLE);
    Chip_ADC_EnableChannel(LPC_ADC0, ADC_CH6, ENABLE);
    Chip_ADC_EnableChannel(LPC_ADC0, ADC_CH7, ENABLE);
    Chip_SCU_ADC_Channel_Config(0, 5);
    Chip_SCU_ADC_Channel_Config(0, 6);
    Chip_SCU_ADC_Channel_Config(0, 7);
    Chip_ADC_SetBurstCmd(LPC_ADC0, ENABLE);
}

uint16_t pots_read(uint8_t ch) {
    if (ch >= POTS_COUNT)
        return 0;
    uint16_t dataADC;
    //while (Chip_ADC_ReadStatus(LPC_ADC0, ch + 5, ADC_DR_DONE_STAT) != SET) {}
    Chip_ADC_ReadValue(LPC_ADC0, pot_channels[ch], &dataADC);
    return dataADC;
}