/*
 * Copyright(C) NXP Semiconductors, 2012
 * All rights reserved.
 *
 * Software that is described herein is for illustrative purposes only
 * which provides customers with programming information regarding the
 * LPC products.  This software is supplied "AS IS" without any warranties of
 * any kind, and NXP Semiconductors and its licensor disclaim any and
 * all warranties, express or implied, including all implied warranties of
 * merchantability, fitness for a particular purpose and non-infringement of
 * intellectual property rights.  NXP Semiconductors assumes no responsibility
 * or liability for the use of the software, conveys no license or rights under any
 * patent, copyright, mask work right, or any other intellectual property rights in
 * or to any products. NXP Semiconductors reserves the right to make changes
 * in the software without notification. NXP Semiconductors also makes no
 * representation or warranty that such application will be suitable for the
 * specified use without further testing or modification.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation is hereby granted, under NXP Semiconductors' and its
 * licensor's relevant copyrights in the software, without fee, provided that it
 * is used in conjunction with NXP Semiconductors microcontrollers.  This
 * copyright, permission, and disclaimer notice must appear in all copies of
 * this code.
 */

#include <string.h>
#include "board.h"
#include "string.h"
#include "retarget.h"

#include "usbd_cfg.h"
#include "cdc_vcom.h"
#include "mem_tests.h"

/* System configuration variables used by chip driver */
const uint32_t ExtRateIn = 0;
const uint32_t OscRateIn = 12000000;

void Board_UART_Init(LPC_USART_T *pUART)
{
	if (pUART == LPC_USART0) {
		Chip_SCU_PinMuxSet(0x6, 4, (SCU_MODE_PULLDOWN | SCU_MODE_FUNC2));					/* P6.5 : UART0_TXD */
		Chip_SCU_PinMuxSet(0x6, 5, (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_FUNC2));/* P6.4 : UART0_RXD */
	}
	else if (pUART == LPC_UART1) {
		Chip_SCU_PinMuxSet(0x1, 13, (SCU_MODE_PULLDOWN | SCU_MODE_FUNC2));				/* P1.13 : UART1_TXD */
		Chip_SCU_PinMuxSet(0x1, 14, (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_FUNC2));	/* P1.14 : UART1_RX */
	}
}

/* Initialize debug output via UART for board */
void Board_Debug_Init(void)
{
#if defined(DEBUG_UART)
	Board_UART_Init(DEBUG_UART);

	Chip_UART_Init(DEBUG_UART);
	Chip_UART_SetBaud(DEBUG_UART, 115200);
	Chip_UART_ConfigData(DEBUG_UART, UART_LCR_WLEN8 | UART_LCR_SBS_1BIT | UART_LCR_PARITY_DIS);

	/* Enable UART Transmit */
	Chip_UART_TXEnable(DEBUG_UART);
#elif defined(DEBUG_VCOM_USB1)
	Board_USB1_Init();
#endif
}

/* Sends a character on the UART */
void Board_UARTPutChar(char ch)
{
#if defined(DEBUG_UART)
	/* Wait for space in FIFO */
	while ((Chip_UART_ReadLineStatus(DEBUG_UART) & UART_LSR_THRE) == 0) {}
	Chip_UART_SendByte(DEBUG_UART, (uint8_t) ch);
#elif defined(DEBUG_VCOM_USB1)
	vcom_write((uint8_t*) (&ch), 1);
#endif
}

/* Gets a character from the UART, returns EOF if no character is ready */
int Board_UARTGetChar(void)
{
#if defined(DEBUG_UART)
	if (Chip_UART_ReadLineStatus(DEBUG_UART) & UART_LSR_RDR) {
		return (int) Chip_UART_ReadByte(DEBUG_UART);
	}
#endif
	return EOF;
}

/* Outputs a string on the debug UART */
void Board_UARTPutSTR(char *str)
{
#if defined(DEBUG_UART)
	while (*str != '\0') {
		Board_UARTPutChar(*str++);
	}
#elif defined(DEBUG_VCOM_USB1)
	vcom_write((uint8_t *) str, strlen(str));
#endif
}

void Board_LED_Set(bool On)
{
	Chip_GPIO_SetPinState(LPC_GPIO_PORT, 7, 7, (bool) !On);
}

bool Board_LED_Test(void)
{
	return (bool) !Chip_GPIO_GetPinState(LPC_GPIO_PORT, 7, 7);
}

void Board_LED_Toggle()
{
	Board_LED_Set(!Board_LED_Test());
}

void Board_Buttons_Init(void)
{
	Chip_SCU_PinMuxSet(0xE, 0, (SCU_MODE_PULLUP | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_FUNC4));		/* KEY 0 Sel */
	Chip_SCU_PinMuxSet(0xE, 1, (SCU_MODE_PULLUP | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_FUNC4));		/* KEY 1 Up */
	Chip_SCU_PinMuxSet(0xE, 2, (SCU_MODE_PULLUP | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_FUNC4));		/* KEY 2 Right */
	Chip_SCU_PinMuxSet(0xE, 3, (SCU_MODE_PULLUP | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_FUNC4));		/* KEY 3 Down */
	Chip_SCU_PinMuxSet(0xE, 4, (SCU_MODE_PULLUP | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_FUNC4));		/* KEY 4 Left */

	Chip_GPIO_SetPinDIRInput(LPC_GPIO_PORT, BUTTONS_GPIO_PORT_NUM, (1 << BUTTONS_PRESS_GPIO_PORT_NUM));	// input
	Chip_GPIO_SetPinDIRInput(LPC_GPIO_PORT, BUTTONS_GPIO_PORT_NUM, (1 << BUTTONS_UP_GPIO_PORT_NUM));	// input
	Chip_GPIO_SetPinDIRInput(LPC_GPIO_PORT, BUTTONS_GPIO_PORT_NUM, (1 << BUTTONS_RIGHT_GPIO_PORT_NUM));	// input
	Chip_GPIO_SetPinDIRInput(LPC_GPIO_PORT, BUTTONS_GPIO_PORT_NUM, (1 << BUTTONS_DOWN_GPIO_PORT_NUM));	// input
	Chip_GPIO_SetPinDIRInput(LPC_GPIO_PORT, BUTTONS_GPIO_PORT_NUM, (1 << BUTTONS_LEFT_GPIO_PORT_NUM));	// input
}

uint32_t Buttons_GetStatus(void)
{
	uint8_t ret = NO_BUTTON_PRESSED;
	if (Chip_GPIO_GetPinState(LPC_GPIO_PORT, BUTTONS_GPIO_PORT_NUM, BUTTONS_PRESS_GPIO_PORT_NUM) == 0)
		ret |= BUTTONS_PRESS;
	if (Chip_GPIO_GetPinState(LPC_GPIO_PORT, BUTTONS_GPIO_PORT_NUM, BUTTONS_UP_GPIO_PORT_NUM) == 0)
		ret |= BUTTONS_UP;
	if (Chip_GPIO_GetPinState(LPC_GPIO_PORT, BUTTONS_GPIO_PORT_NUM, BUTTONS_RIGHT_GPIO_PORT_NUM) == 0)
		ret |= BUTTONS_RIGHT;
	if (Chip_GPIO_GetPinState(LPC_GPIO_PORT, BUTTONS_GPIO_PORT_NUM, BUTTONS_DOWN_GPIO_PORT_NUM) == 0)
		ret |= BUTTONS_DOWN;
	if (Chip_GPIO_GetPinState(LPC_GPIO_PORT, BUTTONS_GPIO_PORT_NUM, BUTTONS_LEFT_GPIO_PORT_NUM) == 0)
		ret |= BUTTONS_LEFT;
	return ret;
}

void Board_Joystick_Init(void)
{
	Chip_SCU_PinMuxSet(0xD, 2, (SCU_MODE_PULLUP | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_FUNC4));		/* KEY 0 Sel */
	Chip_SCU_PinMuxSet(0xD, 3, (SCU_MODE_PULLUP | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_FUNC4));		/* KEY 1 Up */
	Chip_SCU_PinMuxSet(0xD, 4, (SCU_MODE_PULLUP | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_FUNC4));		/* KEY 2 Right */
	Chip_SCU_PinMuxSet(0xD, 5, (SCU_MODE_PULLUP | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_FUNC4));		/* KEY 3 Down */
	Chip_SCU_PinMuxSet(0xD, 6, (SCU_MODE_PULLUP | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_FUNC4));		/* KEY 4 Left */

	Chip_GPIO_SetPinDIRInput(LPC_GPIO_PORT, JOYSTICK_GPIO_PORT_NUM, JOYSTICK_UP_GPIO_BIT_NUM);		/* input */
	Chip_GPIO_SetPinDIRInput(LPC_GPIO_PORT, JOYSTICK_GPIO_PORT_NUM, JOYSTICK_DOWN_GPIO_BIT_NUM);	/* input */
	Chip_GPIO_SetPinDIRInput(LPC_GPIO_PORT, JOYSTICK_GPIO_PORT_NUM, JOYSTICK_LEFT_GPIO_BIT_NUM);	/* input */
	Chip_GPIO_SetPinDIRInput(LPC_GPIO_PORT, JOYSTICK_GPIO_PORT_NUM, JOYSTICK_RIGHT_GPIO_BIT_NUM);	/* input */
	Chip_GPIO_SetPinDIRInput(LPC_GPIO_PORT, JOYSTICK_GPIO_PORT_NUM, JOYSTICK_PRESS_GPIO_BIT_NUM);	/* input */
}

uint8_t Joystick_GetStatus(void)
{
	uint8_t ret = NO_JOYSTICK_PRESSED;
	if (Chip_GPIO_GetPinState(LPC_GPIO_PORT, JOYSTICK_GPIO_PORT_NUM, JOYSTICK_PRESS_GPIO_BIT_NUM) == 0)
		ret |= JOYSTICK_PRESS;
	if (Chip_GPIO_GetPinState(LPC_GPIO_PORT, JOYSTICK_GPIO_PORT_NUM, JOYSTICK_UP_GPIO_BIT_NUM) == 0)
		ret |= JOYSTICK_UP;
	if (Chip_GPIO_GetPinState(LPC_GPIO_PORT, JOYSTICK_GPIO_PORT_NUM, JOYSTICK_RIGHT_GPIO_BIT_NUM) == 0)
		ret |= JOYSTICK_RIGHT;
	if (Chip_GPIO_GetPinState(LPC_GPIO_PORT, JOYSTICK_GPIO_PORT_NUM, JOYSTICK_DOWN_GPIO_BIT_NUM) == 0)
		ret |= JOYSTICK_DOWN;
	if (Chip_GPIO_GetPinState(LPC_GPIO_PORT, JOYSTICK_GPIO_PORT_NUM, JOYSTICK_LEFT_GPIO_BIT_NUM) == 0)
		ret |= JOYSTICK_LEFT;
	return ret;
}

static USBD_HANDLE_T g_hUsb;
/* Endpoint 0 patch that prevents nested NAK event processing */
static uint32_t g_ep0RxBusy = 0;/* flag indicating whether EP0 OUT/RX buffer is busy. */
static USB_EP_HANDLER_T g_Ep0BaseHdlr;	/* variable to store the pointer to base EP0 handler */
const USBD_API_T *g_pUsbApi;

/* EP0_patch part of WORKAROUND for artf45032. */
ErrorCode_t EP0_patch(USBD_HANDLE_T hUsb, void *data, uint32_t event)
{
	switch (event) {
	case USB_EVT_OUT_NAK:
		if (g_ep0RxBusy) {
			/* we already queued the buffer so ignore this NAK event. */
			return LPC_OK;
		}
		else {
			/* Mark EP0_RX buffer as busy and allow base handler to queue the buffer. */
			g_ep0RxBusy = 1;
		}
		break;

	case USB_EVT_SETUP:	/* reset the flag when new setup sequence starts */
	case USB_EVT_OUT:
		/* we received the packet so clear the flag. */
		g_ep0RxBusy = 0;
		break;
	}
	return g_Ep0BaseHdlr(hUsb, data, event);
}

/* Find the address of interface descriptor for given class type. */
USB_INTERFACE_DESCRIPTOR *find_IntfDesc(const uint8_t *pDesc, uint32_t intfClass)
{
	USB_COMMON_DESCRIPTOR *pD;
	USB_INTERFACE_DESCRIPTOR *pIntfDesc = 0;
	uint32_t next_desc_adr;

	pD = (USB_COMMON_DESCRIPTOR *) pDesc;
	next_desc_adr = (uint32_t) pDesc;

	while (pD->bLength) {
		/* is it interface descriptor */
		if (pD->bDescriptorType == USB_INTERFACE_DESCRIPTOR_TYPE) {

			pIntfDesc = (USB_INTERFACE_DESCRIPTOR *) pD;
			/* did we find the right interface descriptor */
			if (pIntfDesc->bInterfaceClass == intfClass) {
				break;
			}
		}
		pIntfDesc = 0;
		next_desc_adr = (uint32_t) pD + pD->bLength;
		pD = (USB_COMMON_DESCRIPTOR *) next_desc_adr;
	}

	return pIntfDesc;
}

void Board_USB1_Init(void) {
	USBD_API_INIT_PARAM_T usb_param;
	USB_CORE_DESCS_T desc;
	ErrorCode_t ret = LPC_OK;
	USB_CORE_CTRL_T *pCtrl;

	/* enable clocks and pinmux */
	USB_init_pin_clk();
	/* Init USB API structure */
	g_pUsbApi = (const USBD_API_T *) LPC_ROM_API->usbdApiBase;
	/* initialize call back structures */
	memset((void *) &usb_param, 0, sizeof(USBD_API_INIT_PARAM_T));
	usb_param.usb_reg_base = LPC_USB_BASE;
	usb_param.max_num_ep = 4;
	usb_param.mem_base = USB_STACK_MEM_BASE;
	usb_param.mem_size = USB_STACK_MEM_SIZE;

	/* Set the USB descriptors */
	desc.device_desc = (uint8_t *) USB_DeviceDescriptor;
	desc.string_desc = (uint8_t *) USB_StringDescriptor;
#ifdef USE_USB0
	desc.high_speed_desc = USB_HsConfigDescriptor;
	desc.full_speed_desc = USB_FsConfigDescriptor;
	desc.device_qualifier = (uint8_t *) USB_DeviceQualifier;
#else
	/* Note, to pass USBCV test full-speed only devices should have both
	 * descriptor arrays point to same location and device_qualifier set
	 * to 0.
	 */
	desc.high_speed_desc = USB_FsConfigDescriptor;
	desc.full_speed_desc = USB_FsConfigDescriptor;
	desc.device_qualifier = 0;
#endif
	/* USB Initialization */
	ret = USBD_API->hw->Init(&g_hUsb, &desc, &usb_param);
	if (ret == LPC_OK) {

		/*	WORKAROUND for artf45032 ROM driver BUG:
		    Due to a race condition there is the chance that a second NAK event will
		    occur before the default endpoint0 handler has completed its preparation
		    of the DMA engine for the first NAK event. This can cause certain fields
		    in the DMA descriptors to be in an invalid state when the USB controller
		    reads them, thereby causing a hang.
		 */
		pCtrl = (USB_CORE_CTRL_T *) g_hUsb;	/* convert the handle to control structure */
		g_Ep0BaseHdlr = pCtrl->ep_event_hdlr[0];/* retrieve the default EP0_OUT handler */
		pCtrl->ep_event_hdlr[0] = EP0_patch;/* set our patch routine as EP0_OUT handler */

		/* Init VCOM interface */
		ret = vcom_init(g_hUsb, &desc, &usb_param);
		if (ret == LPC_OK) {
			/*  enable USB interrupts */
			NVIC_EnableIRQ(LPC_USB_IRQ);
			/* now connect */
			USBD_API->hw->Connect(g_hUsb, 1);
		}

	}
}

void Board_Delay(uint32_t n) {
	while(n--) {}
}

void Board_Blink_Forever(uint8_t count) {
	uint8_t i;
	const uint32_t BLINK_DELAY = 1000u * 5000u;
	Board_LED_Set(1); // if count==0, keep led always on
	for (;;) {
		for (i = count; i > 0; i--) {
			Board_LED_Set(1);
			Board_Delay(BLINK_DELAY);
			Board_LED_Set(0);
			Board_Delay(BLINK_DELAY);
		}
		Board_Delay(BLINK_DELAY * 5);
	}
}

/* Returns the MAC address assigned to this board */
void Board_ENET_GetMacADDR(uint8_t *mcaddr)
{
	uint8_t boardmac[] = {0x00, 0x60, 0x37, 0x12, 0x34, 0x56};

	memcpy(mcaddr, boardmac, 6);
}

int check_mem(void) {
	MEM_TEST_SETUP_T mem_setup = { (uint32_t *) 0x28000000, 12800 };
	bool valid = 1;
	DEBUGSTR("\nmem_test_walking0...     ");
	valid = mem_test_walking0(&mem_setup);
	if (valid) DEBUGSTR("OK"); else DEBUGOUT("FAULT @%p %04lx!=%04lx", mem_setup.fail_addr, mem_setup.is_val, mem_setup.ex_val);
	DEBUGSTR("\nmem_test_walking1...     ");
	valid = mem_test_walking1(&mem_setup);
	if (valid) DEBUGSTR("OK"); else DEBUGOUT("FAULT @%p %04lx!=%04lx", mem_setup.fail_addr, mem_setup.is_val, mem_setup.ex_val);
	DEBUGSTR("\nmem_test_address...      ");
	valid = mem_test_address(&mem_setup);
	if (valid) DEBUGSTR("OK"); else DEBUGOUT("FAULT @%p %04lx!=%04lx", mem_setup.fail_addr, mem_setup.is_val, mem_setup.ex_val);
	DEBUGSTR("\nmem_test_invaddress...   ");
	valid = mem_test_invaddress(&mem_setup);
	if (valid) DEBUGSTR("OK"); else DEBUGOUT("FAULT @%p %04lx!=%04lx", mem_setup.fail_addr, mem_setup.is_val, mem_setup.ex_val);
	DEBUGSTR("\nmem_test_pattern...      ");
	valid = mem_test_pattern(&mem_setup);
	if (valid) DEBUGSTR("OK"); else DEBUGOUT("FAULT @%p %04lx!=%04lx", mem_setup.fail_addr, mem_setup.is_val, mem_setup.ex_val);
	DEBUGSTR("\nmem_test_pattern_seed... ");
	valid = mem_test_pattern_seed(&mem_setup, 0x5ad1babe, 7);
	if (valid) DEBUGSTR("OK"); else DEBUGOUT("FAULT @%p %04lx!=%04lx", mem_setup.fail_addr, mem_setup.is_val, mem_setup.ex_val);
	DEBUGSTR("\n");
	return valid;
}

/* Set up and initialize all required blocks and functions related to the
   board hardware */
void Board_Init(void)
{
	/* Sets up DEBUG UART */
	DEBUGINIT();

	Chip_GPIO_Init(LPC_GPIO_PORT);

	Board_Buttons_Init();
	Board_Joystick_Init();

	/* Initialize LEDs */
	Chip_GPIO_SetPinDIROutput(LPC_GPIO_PORT, 7, 7);
	Chip_GPIO_SetPinState(LPC_GPIO_PORT, 7, 7, 1);

	//check_mem();
}

void Board_I2C_Init(I2C_ID_T id)
{
	if (id == I2C1) {
		/* Configure pin function for I2C1*/
		Chip_SCU_PinMuxSet(0x2, 3, (SCU_MODE_ZIF_DIS | SCU_MODE_INBUFF_EN | SCU_MODE_FUNC1));		/* P2.3 : I2C1_SDA */
		Chip_SCU_PinMuxSet(0x2, 4, (SCU_MODE_ZIF_DIS | SCU_MODE_INBUFF_EN | SCU_MODE_FUNC1));		/* P2.4 : I2C1_SCL */
	}
	else {
		Chip_SCU_I2C0PinConfig(I2C0_STANDARD_FAST_MODE);
	}
}

void Board_SDMMC_Init(void)
{
	Chip_SCU_PinMuxSet(0x1, 9, (SCU_PINIO_FAST | SCU_MODE_FUNC7));	/* P1.9 connected to SDIO_D0 */
	Chip_SCU_PinMuxSet(0x1, 10, (SCU_PINIO_FAST | SCU_MODE_FUNC7));	/* P1.10 connected to SDIO_D1 */
	Chip_SCU_PinMuxSet(0x1, 11, (SCU_PINIO_FAST | SCU_MODE_FUNC7));	/* P1.11 connected to SDIO_D2 */
	Chip_SCU_PinMuxSet(0x1, 12, (SCU_PINIO_FAST | SCU_MODE_FUNC7));	/* P1.12 connected to SDIO_D3 */

	Chip_SCU_ClockPinMuxSet(2, (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_FUNC4));	/* CLK2 connected to SDIO_CLK */
	Chip_SCU_PinMuxSet(0x1, 6, (SCU_PINIO_FAST | SCU_MODE_FUNC7));	/* P1.6 connected to SDIO_CMD */
}

void Board_SSP_Init(LPC_SSP_T *pSSP)
{
	if (pSSP == LPC_SSP1) {
		/* Set up clock and power for SSP1 module */
		/* Configure SSP1 pins*/
		/* SCLK comes out pin CLK0 */
		Chip_SCU_ClockPinMuxSet(0, (SCU_PINIO_FAST | SCU_MODE_FUNC6));		/* CLK0 connected to CLK	SCU_MODE_FUNC6=SSP1 CLK1  */
		Chip_SCU_PinMuxSet(0x1, 5, (SCU_PINIO_FAST | SCU_MODE_FUNC5));			/* P1.5 connected to nCS	SCU_MODE_FUNC5=SSP1 SSEL1 */
		Chip_SCU_PinMuxSet(0x1, 3, (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_FUNC5));/* P1.3 connected to SO		SCU_MODE_FUNC5=SSP1 MISO1 */
		Chip_SCU_PinMuxSet(0x1, 4, (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_FUNC5));/* P1.4 connected to nSI	SCU_MODE_FUNC5=SSP1 MOSI1 */
	}
	else {
		return;
	}
}

/* Initialize Audio Codec */
static Status Board_Audio_CodecInit(int micIn)
{
	if (!UDA1380_Init(UDA1380_MIC_IN_LR & - (micIn != 0))) {
		return ERROR;
	}

	return SUCCESS;
}

/* Board Audio initialization */
void Board_Audio_Init(LPC_I2S_T *pI2S, int micIn)
{
	I2S_AUDIO_FORMAT_T I2S_Config;

	I2S_Config.SampleRate = 48000;
	I2S_Config.ChannelNumber = 2;		/* 1 is mono, 2 is stereo */
	I2S_Config.WordWidth =  16;			/* 8, 16 or 32 bits */
	Chip_I2S_Init(pI2S);
	Chip_I2S_TxConfig(pI2S, &I2S_Config);

	/* Init UDA1380 CODEC */
	while (Board_Audio_CodecInit(micIn) != SUCCESS) {}
}

