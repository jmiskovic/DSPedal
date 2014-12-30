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

#include "board.h"

/* The System initialization code is called prior to the application and
   initializes the board for run-time operation. */

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/

/* Structure for initial base clock states */
struct CLK_BASE_STATES {
    CHIP_CGU_BASE_CLK_T clk;    /* Base clock */
    CHIP_CGU_CLKIN_T clkin;    /* Base clock source, see UM for allowable souorces per base clock */
    bool autoblock_enab;/* Set to true to enable autoblocking on frequency change */
    bool powerdn;        /* Set to true if the base clock is initially powered down */
};

/* Initial base clock states are mostly on */
STATIC const struct CLK_BASE_STATES InitClkStates[] = {
/*
    {CLK_BASE_PHY_TX, CLKIN_ENET_TX, true, false},
#if defined(USE_RMII)
    {CLK_BASE_PHY_RX, CLKIN_ENET_TX, true, false},
#else
    {CLK_BASE_PHY_RX, CLKIN_ENET_RX, true, false},
#endif

    // Clocks derived from dividers
    {CLK_BASE_LCD, CLKIN_IDIVC, true, false},
    {CLK_BASE_USB1, CLKIN_IDIVD, true, true}
*/
};

/* SPIFI high speed pin mode setup */
STATIC const PINMUX_GRP_T spifipinmuxing[] = {
    {0x3, 3,  (SCU_PINIO_FAST | SCU_MODE_FUNC3)},    /* SPIFI CLK */
    {0x3, 4,  (SCU_PINIO_FAST | SCU_MODE_FUNC3)},    /* SPIFI D3 */
    {0x3, 5,  (SCU_PINIO_FAST | SCU_MODE_FUNC3)},    /* SPIFI D2 */
    {0x3, 6,  (SCU_PINIO_FAST | SCU_MODE_FUNC3)},    /* SPIFI D1 */
    {0x3, 7,  (SCU_PINIO_FAST | SCU_MODE_FUNC3)},    /* SPIFI D0 */
    {0x3, 8,  (SCU_PINIO_FAST | SCU_MODE_FUNC3)}    /* SPIFI CS/SSEL */
};

STATIC const PINMUX_GRP_T pinmuxing[] = {
    /* Board LEDs */
    {0xE, 7, (SCU_MODE_INBUFF_EN | SCU_MODE_PULLDOWN | SCU_MODE_FUNC4)},
    /*  I2S  */
    {0x4, 7,  (SCU_PINIO_FAST | SCU_MODE_FUNC7)}, /* I2S0_TX_SCK */
    {0x3, 0,  (SCU_PINIO_FAST | SCU_MODE_FUNC0)}, /* I2S0_RX_SCK */
    {0xC, 12, (SCU_PINIO_FAST | SCU_MODE_FUNC6)}, /* I2S0_TX_SDA */
    {0x3, 2,  (SCU_PINIO_FAST | SCU_MODE_FUNC1)}, /* I2S0_RX_SDA */
    {0xc, 13, (SCU_PINIO_FAST | SCU_MODE_FUNC6)}, /* I2S0_TX_WS */
    {0x3, 1,  (SCU_PINIO_FAST | SCU_MODE_FUNC1)}, /* I2S0_RX_WS */
    /* EMC data */
    /* External data lines D0 .. D15 */
    {0x1, 7, (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_FUNC3)},    /* EMC_D0 */
    {0x1, 8, (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_FUNC3)},    /* EMC_D1 */
    {0x1, 9, (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_FUNC3)},    /* EMC_D2 */
    {0x1, 10, (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_FUNC3)},    /* EMC_D3 */
    {0x1, 11, (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_FUNC3)},    /* EMC_D4 */
    {0x1, 12, (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_FUNC3)},    /* EMC_D5 */
    {0x1, 13, (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_FUNC3)},    /* EMC_D6 */
    {0x1, 14, (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_FUNC3)},    /* EMC_D7 */
    {0x5, 4, (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_FUNC2)},    /* EMC_D8 */
    {0x5, 5, (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_FUNC2)},    /* EMC_D9 */
    {0x5, 6, (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_FUNC2)},    /* EMC_D10 */
    {0x5, 7, (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_FUNC2)},    /* EMC_D11 */
    {0x5, 0, (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_FUNC2)},    /* EMC_D12 */
    {0x5, 1, (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_FUNC2)},    /* EMC_D13 */
    {0x5, 2, (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_FUNC2)},    /* EMC_D14 */
    {0x5, 3, (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_FUNC2)},    /* EMC_D15 */
    /* EMC address */
    {0x2, 9, (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_FUNC3)},    /* EMC_A0 */
    {0x2, 10, (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_FUNC3)},    /* EMC_A1 */
    {0x2, 11, (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_FUNC3)},    /* EMC_A2 */
    {0x2, 12, (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_FUNC3)},    /* EMC_A3 */
    {0x2, 13, (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_FUNC3)},    /* EMC_A4 */
    {0x1, 0, (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_FUNC2)},    /* EMC_A5 */
    {0x1, 1, (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_FUNC2)},    /* EMC_A6 */
    {0x1, 2, (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_FUNC2)},    /* EMC_A7 */
    {0x2, 8, (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_FUNC3)},    /* EMC_A8 */
    {0x2, 7, (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_FUNC3)},    /* EMC_A9 */
    {0x2, 6, (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_FUNC2)},    /* EMC_A10 */
    {0x2, 2, (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_FUNC2)},    /* EMC_A11 */
    {0x2, 1, (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_FUNC2)},    /* EMC_A12 */
    {0x2, 0, (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_FUNC2)},    /* EMC_A13 */
    {0x6, 8, (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_FUNC1)},    /* EMC_A14 */
    /* EMC control pins */
    {0x6, 9, (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_FUNC3)},    /* EMC_DYCS0 */
    {0x1, 6, (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_FUNC3)},    /* EMC_WE */
    {0x6, 4, (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_FUNC3)},    /* EMC_CAS */
    {0x6, 5, (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_FUNC3)},    /* EMC_RAS */
    {0x6, 11, (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_FUNC3)},    /* EMC_CKEOUT0 */
    {0x6, 12, (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_FUNC3)},    /* EMC_DQMOUT0 */
    {0x6, 10, (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_FUNC3)},    /* EMC_DQMOUT1 */

    /* errata */
    {0xD, 0, (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_FUNC2)},    /* EMC_DQMOUT2 */
    {0xE, 13, (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_FUNC3)},    /* EMC_DQMOUT3 */
    /* repeated */
	{0x1, 3,  (SCU_PINIO_FAST | SCU_MODE_FUNC3)},	/* OE   */
	{0x1, 6,  (SCU_PINIO_FAST | SCU_MODE_FUNC3)},	/* WE   */
	{0x1, 5,  (SCU_PINIO_FAST | SCU_MODE_FUNC3)},	/* CS0  */
};

/* Pin clock mux values, re-used structure, value in first index is meaningless */
STATIC const PINMUX_GRP_T pinclockmuxing[] = {
	{0, 0,  (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_FUNC0)},
	{0, 1,  (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_FUNC0)},
	{0, 2,  (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_FUNC0)},
	{0, 3,  (SCU_MODE_INACT | SCU_MODE_INBUFF_EN | SCU_MODE_ZIF_DIS | SCU_MODE_HIGHSPEEDSLEW_EN | SCU_MODE_FUNC0)},
};


/* EMC clock delay */
#define CLK0_DELAY 4

/* SDRAM timing and chip Config */
STATIC const IP_EMC_DYN_CONFIG_T MT48LC16M16_config = {
    EMC_NANOSECOND(64000000 / 4096),/* Refresh period */
    0x01,                   /* ReadConfig	Clock, command delayed */
	EMC_NANOSECOND(18),		/* tRP			Precharge Command Period */
	EMC_NANOSECOND(42),		/* tRAS			Active to Precharge Command Period */
	EMC_NANOSECOND(70),		/* tSREX		Self Refresh Exit Time */
	EMC_NANOSECOND(18),		/* tAPR			Last Data Out to Active Time */
	EMC_CLOCK(0x04),		/* tDAL			Data In to Active Command Time */
	EMC_NANOSECOND(8),		/* tWR			Write Recovery Time */
	EMC_NANOSECOND(60),		/* tRC			Active to Active Command Period */
	EMC_NANOSECOND(60),		/* tRFC			Auto-refresh Period */
	EMC_NANOSECOND(70),		/* tXSR			Exit Selt Refresh */
	EMC_NANOSECOND(12),		/* tRRD			Active Bank A to Active Bank B Time */
	EMC_CLOCK(0x02   ),		/* tMRD			Load Mode register command to Active Command */
    {                               /* Device Configuration array */
        {
            EMC_ADDRESS_DYCS0,    /* Board uses DYCS0 for SDRAM */
            3,    /* RAS */

            EMC_DYN_MODE_WBMODE_PROGRAMMED |
            EMC_DYN_MODE_OPMODE_STANDARD |
            EMC_DYN_MODE_CAS_3 |
            EMC_DYN_MODE_BURST_TYPE_SEQUENTIAL |
            EMC_DYN_MODE_BURST_LEN_8,

            EMC_DYN_CONFIG_MD_SDRAM |
            EMC_DYN_CONFIG_DATA_BUS_16 |
            EMC_DYN_CONFIG_16Mx16_4BANKS_13ROWS_9COLS
        },
        {0, 0, 0, 0},
        {0, 0, 0, 0},
        {0, 0, 0, 0}
    }
};

/* Setup external memories */
void Board_SetupExtMemory(void)
{
    /* Setup EMC Delays */
    /* Move all clock delays together */
    LPC_SCU->EMCDELAYCLK = ((CLK0_DELAY) | (CLK0_DELAY << 4) | (CLK0_DELAY << 8) | (CLK0_DELAY << 12));

    /* Setup EMC Clock Divider for divide by 2 - this is done in both the CCU (clocking)
       and CREG. For frequencies over 120MHz, a divider of 2 must be used. For frequencies
       less than 120MHz, a divider of 1 or 2 is ok. */
    Chip_Clock_EnableOpts(CLK_MX_EMC_DIV, true, true, 2);
    LPC_CREG->CREG6 |= (1 << 16);

    /* Enable EMC clock */
    Chip_Clock_Enable(CLK_MX_EMC);

    /* Init EMC Controller -Enable-LE mode */
    Chip_EMC_Init(1, 0, 0);
    /* Init EMC Dynamic Controller */
    Chip_EMC_Dynamic_Init((IP_EMC_DYN_CONFIG_T *) &MT48LC16M16_config);
    /* Enable Buffer for External Flash */
    //LPC_EMC->STATICCONFIG0 |= 1 << 19;
}

/* Sets up system pin muxing */
void Board_SetupMuxing(void)
{
	uint8_t i;
    /* Setup system level pin muxing */
    Chip_SCU_SetPinMuxing(pinmuxing, sizeof(pinmuxing) / sizeof(PINMUX_GRP_T));

	/* Clock pins only, group field not used */
	for (i = 0; i < (sizeof(pinclockmuxing) / sizeof(pinclockmuxing[0])); i++) {
		Chip_SCU_ClockPinMuxSet(pinclockmuxing[i].pinnum, pinclockmuxing[i].modefunc);
	}

    /* SPIFI pin setup is done prior to setting up system clocking */
    Chip_SCU_SetPinMuxing(spifipinmuxing, sizeof(spifipinmuxing) / sizeof(PINMUX_GRP_T));
}

/* Set up and initialize clocking prior to call to main */
void Board_SetupClocking(void)
{
    int i;

    Chip_SetupCoreClock(CLKIN_CRYSTAL, MAX_CLOCK_FREQ, true);

    /* Reset and enable 32Khz oscillator */
    LPC_CREG->CREG0 &= ~((1 << 3) | (1 << 2));
    LPC_CREG->CREG0 |= (1 << 1) | (1 << 0);

    /* Setup a divider E for main PLL clock switch SPIFI clock to that divider.
       Divide rate is based on CPU speed and speed of SPI FLASH part. */
#if (MAX_CLOCK_FREQ > 180000000)
    Chip_Clock_SetDivider(CLK_IDIV_E, CLKIN_MAINPLL, 5);
#else
    Chip_Clock_SetDivider(CLK_IDIV_E, CLKIN_MAINPLL, 4);
#endif
    Chip_Clock_SetBaseClock(CLK_BASE_SPIFI, CLKIN_IDIVE, true, false);

    /* Setup system base clocks and initial states. This won't enable and
       disable individual clocks, but sets up the base clock sources for
       each individual peripheral clock. */
    for (i = 0; i < (sizeof(InitClkStates) / sizeof(InitClkStates[0])); i++) {
        Chip_Clock_SetBaseClock(InitClkStates[i].clk, InitClkStates[i].clkin,
                                InitClkStates[i].autoblock_enab, InitClkStates[i].powerdn);
    }
}

/* Set up and initialize hardware prior to call to main */
void Board_SystemInit(void)
{
    /* Setup system clocking and memory. This is done early to allow the
       application and tools to clear memory and use scatter loading to
       external memory. */
    Board_SetupMuxing();
    Board_SetupClocking();
	Board_SetupExtMemory();
}
