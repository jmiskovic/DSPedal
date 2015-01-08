#include <stdint.h>
#include "board.h"
#include "faust.h"

#define SAMPLE_RATE 48000
#define BUFFER_SIZE 1

extern void main_periodic_task(void);

void sound_blocking_process(void)
{
    uint32_t polling_data = 0;
    int16_t left, right;
    int i;
    mydsp * dsp = (mydsp * ) DSP_MEMORY_BLOCK_START;
    float inputs[2][BUFFER_SIZE] = {{0}};
    float outputs[2][BUFFER_SIZE] = {{0}};
    float * input_pointers[] = {&inputs[0][0], &inputs[1][0]};
    float * output_pointers[] = {&outputs[0][0], &outputs[1][0]};
    instanceInitmydsp(dsp, SAMPLE_RATE);

    for (;;) {
        for (i = 0; i < BUFFER_SIZE; i++) {
            while (!(Chip_I2S_GetRxLevel(LPC_I2S0) > 0)) {}
            polling_data = Chip_I2S_Receive(LPC_I2S0);
            left = (int16_t)(polling_data >> 16);
            right = (int16_t)(polling_data & 0xFFFF);
            inputs[0][i] = left / 32768.0f;
            inputs[1][i] = right / 32768.0f;
        }
        computemydsp(dsp, BUFFER_SIZE, input_pointers, output_pointers);

        for (i = 0; i < BUFFER_SIZE; i++) {
            while (!(Chip_I2S_GetTxLevel(LPC_I2S0) < 4)) {}
            left = outputs[0][i] * 32768.0f;
            right = outputs[1][i] * 32768.0f;

            polling_data = (((uint16_t) left) << 16) | (((uint16_t) right) & 0xffff);
            Chip_I2S_Send(LPC_I2S0, polling_data);
        }
#ifdef DEBUG_ENABLE
        main_periodic_task();
#endif
    }
}

void sound_blocking_passthrough(void)
{
    uint8_t send_flag = 0;
    uint32_t polling_data = 0;
    for (;;) {
        if (Chip_I2S_GetRxLevel(LPC_I2S0) > 0) {
            polling_data = Chip_I2S_Receive(LPC_I2S0);
            send_flag = 1;
        }
        if ((Chip_I2S_GetTxLevel(LPC_I2S0) < 4) && (send_flag == 1)) {
            Chip_I2S_Send(LPC_I2S0, polling_data);
            send_flag = 0;
        }
    }
}

void sound_init(void)
{
    Status ret;
    I2S_AUDIO_FORMAT_T audio_config;
    audio_config.SampleRate = SAMPLE_RATE;
    audio_config.ChannelNumber = 2;
    audio_config.WordWidth = 16;

    Board_Audio_Init(LPC_I2S0, UDA1380_LINE_IN);
    do {
        ret = Chip_I2S_RxConfig(LPC_I2S0, &audio_config);
    } while (ret != SUCCESS);
    do {
        ret = Chip_I2S_TxConfig(LPC_I2S0, &audio_config);
    } while (ret != SUCCESS);
    Chip_I2S_TxStop(LPC_I2S0);
    Chip_I2S_DisableMute(LPC_I2S0);
    Chip_I2S_TxStart(LPC_I2S0);
    Chip_I2S_RxStart(LPC_I2S0);
}