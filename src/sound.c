#include <stdint.h>
#include "board.h"
#include "faust.h"

#define __ALLOCATE_IN_SDRAM__ __attribute__((section("SDRAM")))

/*#define BUFFER_SIZE 1000000
typedef struct ring_buff {
    uint8_t read_index;
    uint8_t write_index;
    float buffer [BUFFER_SIZE];
} Ring_Buffer_t;
Ring_Buffer_t __ALLOCATE_IN_SDRAM__ ring;
*/

static void passthrough_interrupt(void)
{
    Chip_I2S_Int_RxCmd(LPC_I2S0, ENABLE, 4);
    Chip_I2S_Int_TxCmd(LPC_I2S0, ENABLE, 4);
    NVIC_EnableIRQ(I2S0_IRQn);
    for (;;) {
    }
    Chip_I2S_Int_RxCmd(LPC_I2S0, DISABLE, 4);
    NVIC_DisableIRQ(I2S0_IRQn);
}

#define FAUST_BUFFER 2

mydsp __ALLOCATE_IN_SDRAM__ dsp;
void sound_blocking_process(void)
{
/*
    ring.read_index = 1;
    ring.write_index = 0;
    uint8_t send_flag = 0;
    float fsig;
    float factor = 0.75f;
    int32_t sig;
    for (;;) {
        if (Chip_I2S_GetRxLevel(LPC_I2S0) > 0) {
            ring.write_index = (ring.write_index + 1) % BUFFER_SIZE;
            //ring.read_index = (ring.read_index + 1) % BUFFER_SIZE;
            sig = (int16_t)(Chip_I2S_Receive(LPC_I2S0) & 0xFFFF);
            ring.buffer[ring.write_index] = sig / 32768.0f + ring.buffer[ring.write_index] * 0.8f;
            Chip_I2S_Send(LPC_I2S0, 0xFFFF & ((uint32_t) ((ring.buffer[ring.write_index]) * 32768.0f)));
        }
    }*/

    uint32_t polling_data = 0;
    int i;
    int16_t left, right;
    float inputs[2][FAUST_BUFFER] = {0};
    float outputs[2][FAUST_BUFFER] = {0};
    float * input_pointers[] = {&inputs[0][0], &inputs[1][0]};
    float * output_pointers[] = {&outputs[0][0], &outputs[1][0]};
    instanceInitmydsp(&dsp, 48000);
    for (;;) {
        for (i = 0; i < FAUST_BUFFER; i++) {
            while (!(Chip_I2S_GetRxLevel(LPC_I2S0) > 0)) {}
            polling_data = Chip_I2S_Receive(LPC_I2S0);
            left = (int16_t)(polling_data >> 16);
            right = (int16_t)(polling_data & 0xFFFF);
            inputs[0][i] = left / 32768.0f;
            inputs[1][i] = right / 32768.0f;
        }
        computemydsp(&dsp, FAUST_BUFFER, input_pointers, output_pointers);
        for (i = 0; i < FAUST_BUFFER; i++) {
            while (!(Chip_I2S_GetTxLevel(LPC_I2S0) < 4)) {}
            left = outputs[0][i] * 32768.0f;
            right = outputs[1][i] * 32768.0f;

            polling_data = (((uint16_t) left) << 16) | (((uint16_t) right) & 0xffff);
            Chip_I2S_Send(LPC_I2S0, polling_data);
        }
    }
}

void sound_blocking_passthrough(void)
{
    uint8_t send_flag;
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
    audio_config.SampleRate = 48000;
    audio_config.ChannelNumber = 2;
    audio_config.WordWidth = 16;
    Board_Audio_Init(LPC_I2S0, UDA1380_LINE_IN);
    Chip_I2S_Init(LPC_I2S0);
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