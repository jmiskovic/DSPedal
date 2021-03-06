/*
 * @brief LPC4357-EVB board file
 *
 * @note
 * Copyright(C) NXP Semiconductors, 2013
 * All rights reserved.
 *
 * @par
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
 * @par
 * Permission to use, copy, modify, and distribute this software and its
 * documentation is hereby granted, under NXP Semiconductors' and its
 * licensor's relevant copyrights in the software, without fee, provided that it
 * is used in conjunction with NXP Semiconductors microcontrollers.  This
 * copyright, permission, and disclaimer notice must appear in all copies of
 * this code.
 */

#ifndef __BOARD_H_
#define __BOARD_H_

#include "chip.h"
/* board_api.h is included at the bottom of this file after DEBUG setup */

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup BOARD_LPC4357_EVB LPC4357-EVB board support software API functions
 * @ingroup LPCOPEN_43XX_BOARD_NGX4357
 * The board support software API functions provide some simple abstracted
 * functions used across multiple LPCOpen board examples. See @ref BOARD_COMMON_API
 * for the functions defined by this board support layer.<br>
 * @{
 */

/** @defgroup BOARD_LPC4357_EVB_OPTIONS BOARD: LPC4357-EVB board options
 * This board has options that configure its operation at build-time.<br>
 * @{
 */

/** Define DEBUG_ENABLE to enable IO via the DEBUGSTR, DEBUGOUT, and
    DEBUGIN macros. If not defined, DEBUG* functions will be optimized
	out of the code at build time.

    Commented out, please use compiler flag -DDEBUG_ENABLE
#define DEBUG_ENABLE
 */

/** Define DEBUG_SEMIHOSTING along with DEBUG_ENABLE to enable IO support
    via semihosting. You may need to use a C library that supports
	semihosting with this option.

    Commented out, please use compiler flag -DDEBUG_SEMIHOSTING
#define DEBUG_SEMIHOSTING
 */

/** Define DEBUG_SHAREDMEM along with DEBUG_ENABLE to enable IO support
    via shared memory interface.

    Commented out, please use compiler flag -DDEBUG_SHAREDMEM
#define DEBUG_SHAREDMEM
 */

/** Board UART used for debug output and input using the DEBUG* macros. This
    is also the port used for Board_UARTPutChar, Board_UARTGetChar, and
	Board_UARTPutSTR functions. */
/* #define DEBUG_UART LPC_USART0 */

//#define DEBUG_VCOM_USB1

/**
 * @}
 */

/* Board name */
#define BOARD_4357_EVB

/* Build for RMII interface */
#define USE_RMII

#define I2CDEV_UDA1380_ADDR     (0x34 >> 1)
#define UDA1380_I2C_BUS          I2C0
#define ADXL345_I2C_BUS          I2C0

enum BUTTON_VALUES {
    BUTTONS_DOWN      = 0x01,
    BUTTONS_UP        = 0x02,
    BUTTONS_RIGHT     = 0x04,
    BUTTONS_LEFT      = 0x08,
    BUTTONS_PRESS     = 0x10,
    NO_BUTTON_PRESSED = 0x00
};

enum JOYSTICK_VALUES {
    NO_JOYSTICK_PRESSED = 0x00,
    JOYSTICK_UP         = 0x01,
    JOYSTICK_DOWN       = 0x02,
    JOYSTICK_LEFT       = 0x04,
    JOYSTICK_RIGHT      = 0x08,
    JOYSTICK_PRESS      = 0x10
};

#define BUTTONS_GPIO_PORT_NUM   7

enum BUTTON_GPIO_BIT_NUMS {
    BUTTONS_PRESS_GPIO_PORT_NUM     = 0,
    BUTTONS_UP_GPIO_PORT_NUM        = 1,
    BUTTONS_RIGHT_GPIO_PORT_NUM     = 2,
    BUTTONS_DOWN_GPIO_PORT_NUM      = 3,
    BUTTONS_LEFT_GPIO_PORT_NUM      = 4
};

#define JOYSTICK_GPIO_PORT_NUM 6

enum JOYSTICK_GPIO_BIT_NUMS {
    JOYSTICK_UP_GPIO_BIT_NUM    = 16,
    JOYSTICK_RIGHT_GPIO_BIT_NUM = 17,
    JOYSTICK_PRESS_GPIO_BIT_NUM = 18,
    JOYSTICK_DOWN_GPIO_BIT_NUM  = 19,
    JOYSTICK_LEFT_GPIO_BIT_NUM  = 20
};

/**
 * @brief	Sets up board specific I2C interface
 * @param	id	: I2C Peripheral ID (I2C0, I2C1)
 * @return	Nothing
 */
void Board_I2C_Init(I2C_ID_T id);

/**
 * @brief	Sets up I2C Fast Plus mode
 * @param	id	: Must always be I2C0
 * @return	Nothing
 * @note	This function must be called before calling
 *          Chip_I2C_SetClockRate() to set clock rates above
 *          normal range 100KHz to 400KHz. Only I2C0 supports
 *          this mode.
 */
STATIC INLINE void Board_I2C_EnableFastPlus(I2C_ID_T id)
{
	Chip_SCU_I2C0PinConfig(I2C0_FAST_MODE_PLUS);
}

/**
 * @brief	Disable I2C Fast Plus mode and enables default mode
 * @param	id	: Must always be I2C0
 * @return	Nothing
 * @sa		Board_I2C_EnableFastPlus()
 */
STATIC INLINE void Board_I2C_DisableFastPlus(I2C_ID_T id)
{
	Chip_SCU_I2C0PinConfig(I2C0_STANDARD_FAST_MODE);
}

/**
 * @brief	Initializes board specific GPIO Interrupt
 * @return	Nothing
 */
void Board_GPIO_Int_Init(void);

/**
 * @brief	Initialize pin muxing for SSP interface
 * @param	pSSP	: Pointer to SSP interface to initialize
 * @return	Nothing
 */
void Board_SSP_Init(LPC_SSP_T *pSSP);

/**
 * @brief	Returns the MAC address assigned to this board
 * @param	mcaddr : Pointer to 6-byte character array to populate with MAC address
 * @return	Nothing
 */
void Board_ENET_GetMacADDR(uint8_t *mcaddr);

/**
 * @brief	Initialize pin muxing for a UART
 * @param	pUART	: Pointer to UART register block for UART pins to init
 * @return	Nothing
 */
void Board_UART_Init(LPC_USART_T *pUART);

/**
 * @brief	Initialize pin muxing for SDMMC interface
 * @return	Nothing
 */
void Board_SDMMC_Init(void);

/**
 * @brief	Initialize button(s) interface on board
 * @return	Nothing
 */
void Board_Buttons_Init(void);

/**
 * @brief	Initialize joystick interface on board
 * @return	Nothing
 */
void Board_Joystick_Init(void);

/**
 * @brief	Returns joystick states on board
 * @return	Returns a JOY_* value, ir JOY_PRESS or JOY_UP
 */
uint8_t Joystick_GetStatus(void);

/**
 * @brief	Returns button(s) state on board
 * @return	Returns BUTTONS_BUTTON1 if button1 is pressed
 */
uint32_t Buttons_GetStatus (void);

/**
 * @brief   Initialize USB1 interface on board
 * @return  Nothing
 */
void Board_USB1_Init(void);

/**
 * @brief	Initialize I2S interface for the board and UDA1380
 * @param	pI2S	: Pointer to I2S register interface used on this board
 * @param	micIn	: If 1 MIC will be used as input, if 0 LINE_IN will be used
 * @return	Nothing
 */
void Board_Audio_Init(LPC_I2S_T *pI2S, int micIn);


void Board_Delay(uint32_t n);
void Board_Blink_Forever(uint8_t count);

/**
 * @}
 */

#include "board_api.h"
#include "lpc_phy.h"
#include "uda1380.h"

#ifdef __cplusplus
}
#endif

#endif /* __BOARD_H_ */
