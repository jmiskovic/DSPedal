#include <stdint.h>
#include "board.h"
#include "faust.h"

#define I2S_DMA_TX_CHAN GPDMA_CONN_I2S_Tx_Channel_0
#define I2S_DMA_RX_CHAN GPDMA_CONN_I2S_Rx_Channel_1

enum buffer_state {
    BUFFER_FULL = 0,
    BUFFER_EMPTY = 1,
    BUFFER_AVAILABLE = 2,
};

typedef struct ring_buff {
    uint32_t buffer[256];
    uint8_t read_index;
    uint8_t write_index;
} Ring_Buffer_t;

static Ring_Buffer_t ring_buffer;

static uint8_t send_flag;
static uint8_t channelTC;
static uint8_t dmaChannelNum_I2S_Tx, dmaChannelNum_I2S_Rx;
static uint8_t dma_send_receive;

static uint8_t ring_buff_get_status(Ring_Buffer_t *ring_buff)
{
    if (ring_buff->read_index == ring_buff->write_index) {
        return BUFFER_EMPTY;
    }
    else if (ring_buff->read_index == (ring_buff->write_index) + 1) {
        return BUFFER_FULL;
    }
    else {return BUFFER_AVAILABLE; }
}

void I2S0_IRQHandler(void)
{
    while ((ring_buff_get_status(&ring_buffer) != BUFFER_FULL) && (Chip_I2S_GetRxLevel(LPC_I2S0) > 0)) {
        ring_buffer.buffer[ring_buffer.write_index++] = Chip_I2S_Receive(LPC_I2S0);
    }
    while ((ring_buff_get_status(&ring_buffer) != BUFFER_EMPTY) && (Chip_I2S_GetTxLevel(LPC_I2S0) < 8)) {
        Chip_I2S_Send(LPC_I2S0, ring_buffer.buffer[ring_buffer.read_index++]);
    }
    Board_LED_Toggle();
}

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

#define FAUST_BUFFER 4

mydsp dsp;
static void passthrough_pooling(void)
{
    uint32_t polling_data = 0;
    int i;
    Board_LED_Set(1);
    float inputs[2][FAUST_BUFFER];
    float outputs[2][FAUST_BUFFER];

    instanceInitmydsp(&dsp, 48000);
    for (;;) {
        for (i = 0; i < FAUST_BUFFER; i++) {
            while (!(Chip_I2S_GetRxLevel(LPC_I2S0) > 0)) {}
            polling_data = Chip_I2S_Receive(LPC_I2S0);
            inputs[0][i] = (polling_data >> 16) / 32768.0f;
            inputs[1][i] = (polling_data & 0xFFFF) / 32768.0f;
        }
        Board_LED_Toggle();
        computemydsp(&dsp, FAUST_BUFFER, inputs, outputs);
        for (i = 0; i < FAUST_BUFFER; i++) {
            while (!(Chip_I2S_GetTxLevel(LPC_I2S0) < 4)) {}
            //polling_data = (((int16_t) (outputs[0][i] * 32768.0f)) << 16) | (((int16_t) (outputs[1][i] * 32768.0f)) & 0xFFFF);
            polling_data = (((int16_t) (inputs[0][i] * 32768.0f)) << 16) | (((int16_t) (inputs[1][i] * 32768.0f)) & 0xFFFF);
            Chip_I2S_Send(LPC_I2S0, polling_data);
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
    passthrough_pooling();
}