#ifndef _LPC1343
#define LPC1343

/* Used to reset the microcontroller into isp mode from software */
typedef void (*IAP)(unsigned int[], unsigned int[]);
#define IAP_LOCATION 0x1fff1ff1

/* Clocking enabled control */
#define SCB_SYSAHBCLKCTRL_GPIO									(1<<6)
#define SCB_SYSAHBCLKCTRL_IOCON									(1<<16)
#define SCB_SYSAHBCLKCTRL_UART									(1<<12)
#define SCB_SYSAHBCLKCTRL_TMR32_0								(1<<9)
#define	SCB_SYSAHBCLKCTRL_TMR32_1								(1<<10)
#define SCB_SYSAHBCLKCTRL_TMR16_0								(1<<7)
#define SCB_SYSAHBCLKCTRL_TMR16_1								(1<<8)
#define SCB_SYSAHBCLKCTRL_SPI0									(1<<11)

/* clocking control */
#define SCB_PDRUNCFG_SYSOSC_MASK								0x20
#define SCB_SYSOSCCTRL_BYPASS_DISABLED					0x0
#define SCB_SYSOSCCTRL_FREQRANGE_1TO20MHZ				0x0
#define SCB_CLKSEL_SOURCE_MAINOSC								0x1
#define SCB_PLLCLKUEN_UPDATE										0x1
#define SCB_PLLCLKUEN_DISABLE										0x0
#define SCB_PLLCTRL_MULT_4											0x3
#define SCB_PLLCTRL_MULT_6											0x5
#define SCB_PLLCTRL_DIV_BIT											0x5
#define SCB_PDRUNCFG_SYSPLL_MASK								0x80
#define SCB_PLLSTAT_LOCK												0x1
#define SCB_MAINCLKSEL_SOURCE_SYSPLLCLKOUT			0x3
#define SCB_MAINCLKUEN_UPDATE										0x1
#define SCB_MAINCLKUEN_DISABLE									0x0
#define SCB_PDSLEEPCFG_USBPAD_PD								0x400
#define SCB_PDSLEEPCFG_USBPLL_PD								0x100
#define SCB_SYSAHBCLKDIV_DIV1										0x1

/* UART Definitions */
#define UART_LCR_Word_Length_Select_8Chars      0x3
#define UART_LCR_Stop_Bit_Select_1Bits          0x0
#define UART_LCR_Parity_Disabled                0x0
#define UART_LCR_Parity_Select_OddParity        0x0
#define UART_LCR_Break_Control_Disabled         0x0
#define UART_LCR_Divisor_Latch_Access_Enabled   0x80
#define UART_LCR_Divisor_Latch_Access_Disabled  0x0

#define UART_FCR_FIFO_Enabled                   0x1
#define UART_FCR_Rx_FIFO_Reset                  0x2
#define UART_FCR_Tx_FIFO_Reset                  0x4

#define UART_IER_RBR_Interrupt_Enabled          0x1

#define UART_LSR_THRE              	            0x20
#define UART_LSR_TEMT                           0x40
#define UART_LSR_RDR_DATA                       0x1

#define UART_IER_RBR_Interrupt_Enabled          0x1
#define UART_IER_RLS_Interrupt_Enabled          0x4
#define UART_IER_THRE_Interrupt_Enabled         0x2

#define UART_IntId_RDA													0x4
#define UART_IntId_THRE													0x2
#define UART_IntId_RLS													0x6
#define UART_IntId_CTI													0xB
#define UART_IntId_MASK                     		0xE


/* SPI definitions */
#define			SSP_SSP0SR_BSY_BUSY									(1<<4)
#define			SSP_SSP0SR_TNF_NOTFULL							(1<<1)
#define 		SSP_SSP0SR_RNE_NOTEMPTY							(1<<2)

#define 		SSP_CR0_DSS_8BIT										0x7
#define			SSP_CR0_FRF_SPI											0x0
#define			SSP_CR0_CPOL_LOW										0x0
#define			SSP_CR0_CPHA_BACK										(1<<7)

#define			SSP_CR1_ENABLED											0x2

/* IOCON set up */
#define			IOCON_FUNC_GPIO							0
#define			IOCON_FUNC_1								1
#define			IOCON_FUNC_2								2
#define			IOCON_FUNC_3								3

#define 		IOCON_MODE_INACTIVE					(0<<3)
#define			IOCON_MODE_PULL_DOWN				(1<<3)
#define			IOCON_MODE_PULL_UP					(2<<3)
#define			IOCON_MODE_REPEATER					(3<<3)

/* GPIO macros */
#define 		LOW(pin)														&= ~(1<<pin)																			
#define			HIGH(pin)														|= (1<<pin)
#define			LPC_GPIO(port)											((LPC_GPIO_TypeDef   *) (LPC_AHB_BASE + port * 0x10000))
#define 		GPIO_OUTPUT(port, pin, value)				LPC_GPIO(port)->DATA value(pin)
#define			GPIO_PIN_STATE(port, pin)						((LPC_GPIO(port)->DATA & (1<<pin))>>pin)
#define			GPIO_IRQNUM(port)										(IRQn_Type)(EINT0_IRQn - port)
#define			SET_GPIO_AS_OUTPUT(port, pin)				LPC_GPIO(port)->DIR |= (1<<pin)

//need to do the internal one so that the macros "port" and "pin" are expanded before being concatenated
#define 		LPC_IOCON_PIO_INTERNAL(port, pin)				LPC_IOCON->PIO## port ##_## pin 
#define			LPC_IOCON_PIO(port, pin)							LPC_IOCON_PIO_INTERNAL(port, pin)

#endif
