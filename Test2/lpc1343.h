#ifndef _LPC1343
#define LPC1343

#define SCB_SYSAHBCLKCTRL				(*((volatile unsigned long *) 0x40048080))
#define SCB_SYSAHBCLKCTRL_GPIO	(	1<<6)
#define SCB_SYSAHBCLKCTRL_IOCON		(1<<16)
#define SCB_SYSAHBCLKCTRL_UART		(1<<12)
#define SCB_SYSAHBCLKCTRL_TMR32_0	(1<<9)
#define SCB_SYSAHBCLKCTRL_TMR16_0	(1<<7)
#define SCB_SYSAHBCLKCTRL_TMR16_1	(1<<8)



#define GPIO_GPIO0IS				(*((volatile unsigned long *) 0x50008004))
#define GPIO_GPIO0IBE				(*((volatile unsigned long *) 0x50008008))
#define GPIO_GPIO0IEV				(*((volatile unsigned long *) 0x5000800C))
#define GPIO_GPIO0MIS				(*((volatile unsigned long *) 0x50008018))
#define GPIO_GPIO0IC				(*((volatile unsigned long *) 0x5000801C))
#define GPIO_GPIO0IE				(*((volatile unsigned long *) 0x50008010))


#define ISER1								(*((volatile unsigned long *) 0xE000E104))

#define IPR0								(*((volatile unsigned long *) 0xE000E400))
#define IPR14								(*((volatile unsigned long *) 0xE000E438))

#define SCB_PDRUNCFG_SYSOSC_MASK						0x20
#define SCB_SYSOSCCTRL_BYPASS_DISABLED			0x0
#define SCB_SYSOSCCTRL_FREQRANGE_1TO20MHZ		0x0
#define SCB_CLKSEL_SOURCE_MAINOSC						0x1
#define SCB_PLLCLKUEN_UPDATE								0x1
#define SCB_PLLCLKUEN_DISABLE								0x0
#define SCB_PLLCTRL_MULT_4									0x3
#define SCB_PLLCTRL_MULT_6									0x5
#define SCB_PLLCTRL_DIV_BIT									5
#define SCB_PDRUNCFG_SYSPLL_MASK						0x80
#define SCB_PLLSTAT_LOCK										0x1
#define SCB_MAINCLKSEL_SOURCE_SYSPLLCLKOUT	0x3
#define SCB_MAINCLKUEN_UPDATE								0x1
#define SCB_MAINCLKUEN_DISABLE							0x0
#define SCB_PDSLEEPCFG_USBPAD_PD						0x400
#define SCB_PDSLEEPCFG_USBPLL_PD						0x100
#define SCB_SYSAHBCLKDIV_DIV1								0x1


#define UART_LCR_Word_Length_Select_8Chars      ((unsigned int) 0x00000003)
#define UART_LCR_Stop_Bit_Select_1Bits          ((unsigned int) 0x00000000)
#define UART_LCR_Parity_Disabled                ((unsigned int) 0x00000000)
#define UART_LCR_Parity_Select_OddParity        ((unsigned int) 0x00000000)
#define UART_LCR_Break_Control_Disabled         ((unsigned int) 0x00000000)
#define UART_LCR_Divisor_Latch_Access_Enabled   ((unsigned int) 0x00000080)
#define UART_LCR_Divisor_Latch_Access_Disabled  ((unsigned int) 0x00000000)
#define UART_FCR_FIFO_Enabled                   ((unsigned int) 0x00000001)
#define UART_FCR_Rx_FIFO_Reset                  ((unsigned int) 0x00000002) // Clear Rx FIFO
#define UART_FCR_Tx_FIFO_Reset                  ((unsigned int) 0x00000004) // Clear Tx FIFO
#define UART_IER_RBR_Interrupt_Enabled          ((unsigned int) 0x00000001)
#define UART_LSR_THRE              	            ((unsigned int) 0x00000020)
#define UART_LSR_TEMT                           ((unsigned int) 0x00000040)
#define UART_LSR_RDR_DATA                       ((unsigned int) 0x00000001) // U0RBR contains valid data
#define UART_IER_RBR_Interrupt_Enabled          ((unsigned int) 0x00000001)
#define UART_IER_RLS_Interrupt_Enabled          ((unsigned int) 0x00000004)
#define UART_IER_THRE_Interrupt_Enabled         ((unsigned int) 0x00000002)

#define UART_IntId_RDA													0x4
#define UART_IntId_THRE													0x2
#define UART_IntId_RLS													0x6
#define UART_IntId_CTI													0xB
#define UART_IntId_MASK                     		0xE // Interrupt identification


#endif
