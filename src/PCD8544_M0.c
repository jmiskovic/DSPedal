/**
 * \file PCD8544.c
 * \brief Functions relating to PCD8544 LCD Controller (Nokia 5110 LCD).
 * \author Andy Gock
 * \see glcd.h
 */

/*
	Copyright (c) 2012, Andy Gock

	All rights reserved.

	Redistribution and use in source and binary forms, with or without
	modification, are permitted provided that the following conditions are met:
		* Redistributions of source code must retain the above copyright
		  notice, this list of conditions and the following disclaimer.
		* Redistributions in binary form must reproduce the above copyright
		  notice, this list of conditions and the following disclaimer in the
		  documentation and/or other materials provided with the distribution.
		* Neither the name of Andy Gock nor the
		  names of its contributors may be used to endorse or promote products
		  derived from this software without specific prior written permission.

	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
	ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
	WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
	DISCLAIMED. IN NO EVENT SHALL ANDY GOCK BE LIABLE FOR ANY
	DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
	(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
	LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
	ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
	(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
	SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "board.h"
#include "stdint.h"
#include "graphics.h"
#include "PCD8544.h"

#define GLCD_DC_LOW() Chip_GPIO_SetPinState(LPC_GPIO_PORT, 4, 9, 0)
#define GLCD_DC_HIGH() Chip_GPIO_SetPinState(LPC_GPIO_PORT, 4, 9, 1)
#define RST(val) Chip_GPIO_SetPinState(LPC_GPIO_PORT, 7, 25, val)
#define CS(val)  Chip_GPIO_SetPinState(LPC_GPIO_PORT, 4, 15, val)

static void glcd_reset() {
    SPI_CONFIG_FORMAT_T spi_config = {SPI_BITS_8, SPI_CLOCK_MODE0, SPI_DATA_MSB_FIRST};
    RST(0);
    Chip_SPI_Init(LPC_SPI);
    Chip_SPI_SetMode(LPC_SPI, SPI_MODE_MASTER);
    Chip_SPI_Int_Disable(LPC_SPI);
    Chip_SPI_SetBitRate(LPC_SPI, 100000);
    Chip_SPI_SetFormat(LPC_SPI, &spi_config);
    Chip_SCU_PinMuxSet(0xA,  2, (SCU_MODE_PULLUP | SCU_MODE_ZIF_DIS | SCU_MODE_FUNC0)); /* DC  GPIO4[9] */
    Chip_SCU_PinMuxSet(0xF, 11, (SCU_MODE_PULLUP | SCU_MODE_ZIF_DIS | SCU_MODE_FUNC4)); /* RST GPIO7[25] */
    Chip_SCU_PinMuxSet(0x9,  3, (SCU_MODE_PULLUP | SCU_MODE_ZIF_DIS | SCU_MODE_FUNC0)); /* CS  GPIO4[15] */
    Chip_SCU_PinMuxSet(0x3,  7, (SCU_PINIO_FAST | SCU_MODE_FUNC1)); /* SPI MOSI */
    Chip_SCU_PinMuxSet(0x3,  3, (SCU_PINIO_FAST | SCU_MODE_FUNC1)); /* SPI SCK */
    Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT, 4, 9);
    Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT, 7, 25);
    Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT, 4, 15);
    RST(1);
    CS(0);
}

/* for some reason LPCopen declares but does not implement this function */
uint32_t Chip_SPI_WriteFrames_Blocking(LPC_SPI_T *pSPI, uint8_t *buffer, uint32_t buffer_len) {
    uint32_t i;
    for(i = 0 ; i < buffer_len; i++) {
        Chip_SPI_SendFrame(pSPI, *buffer);
        /* wait for transfer to finish and perform a dummy read */
        while(!(Chip_SPI_GetStatus(pSPI) & SPI_SR_SPIF))
        {}
        Chip_SPI_ReceiveFrame(pSPI);
        buffer++;
    }
    return i;
}

static void glcd_spi_write(uint8_t data) {
	Chip_SPI_WriteFrames_Blocking(LPC_SPI, &data, 1);
}


static void glcd_command(uint8_t c)
{
	GLCD_DC_LOW();
	glcd_spi_write(c);
}

static void glcd_data(uint8_t c)
{
	GLCD_DC_HIGH();
	glcd_spi_write(c);
}

void glcd_set_contrast(uint8_t val) {
	glcd_command(PCD8544_FUNCTION_SET | PCD8544_EXTENDED_INSTRUCTION);
	glcd_command(PCD8544_SET_VOP | (val&0x7f));
	glcd_command(PCD8544_FUNCTION_SET);
	glcd_command(PCD8544_DISPLAY_CONTROL | PCD8544_DISPLAY_NORMAL);
}

void glcd_power_down(void)
{
	/* First, fill RAM with zeroes to ensure minimum specified current consumption */
	graphics_clear();
	graphics_refresh();

	/* Power down */
	glcd_command(PCD8544_FUNCTION_SET|PCD8544_POWER_DOWN);
}

void glcd_power_up(void)
{
	glcd_command(PCD8544_FUNCTION_SET);
}

void glcd_set_y_address(uint8_t y)
{
	glcd_command(PCD8544_SET_Y_ADDRESS|(y > 5 ? 5 : y));
}

void glcd_set_x_address(uint8_t x)
{
	glcd_command(PCD8544_SET_X_ADDRESS|(x & 0x7f));
}

void glcd_write(const uint8_t graphics_canvas[])
{
    uint8_t bank;
	for (bank = 0; bank < PCD8544_MAX_BANKS; bank++) {
		/* Each bank is a single row 8 bits tall */
		uint8_t column;

		glcd_command(PCD8544_SET_Y_ADDRESS | bank);
		glcd_command(PCD8544_SET_X_ADDRESS | 0);

		for (column = 0; column <= PCD8544_MAX_COLS; column++)
		{
			glcd_data(graphics_canvas[LCD_PIXEL_HEIGHT * LCD_PIXEL_WIDTH / 8 - 1 - (PCD8544_MAX_COLS * bank + column)] );
		}
	}
}

void glcd_PCD8544_init(void) {

	glcd_reset();

	/* Get into the EXTENDED mode! */
	glcd_command(PCD8544_FUNCTION_SET | PCD8544_EXTENDED_INSTRUCTION);

	/* LCD bias select (4 is optimal?) */
	glcd_command(PCD8544_SET_BIAS | 0x2);

	/* Set VOP (affects contrast) */
	glcd_command(PCD8544_SET_VOP | 80); /* Experimentally determined, play with this figure until contrast looks nice */

	/* Back to standard instructions */
	glcd_command(PCD8544_FUNCTION_SET);

	/* Normal mode */
	glcd_command(PCD8544_DISPLAY_CONTROL | PCD8544_DISPLAY_NORMAL);
}
