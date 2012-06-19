#ifndef _LPC1343
#define LPC1343

#define SCB_SYSAHBCLKCTRL				(*((volatile unsigned long *) 0x40048080))
#define SCB_SYSAHBCLKCTRL_GPIO	(1<<6)
#define SCB_SYSAHBCLKCTRL_IOCON	(1<<16)



#define GPIO_GPIO0IS				(*((volatile unsigned long *) 0x50008004))
#define GPIO_GPIO0IBE				(*((volatile unsigned long *) 0x50008008))
#define GPIO_GPIO0IEV				(*((volatile unsigned long *) 0x5000800C))
#define GPIO_GPIO0MIS				(*((volatile unsigned long *) 0x50008018))
#define GPIO_GPIO0IC				(*((volatile unsigned long *) 0x5000801C))
#define GPIO_GPIO0IE				(*((volatile unsigned long *) 0x50008010))


#define ISER1								(*((volatile unsigned long *) 0xE000E104))

#define IPR0								(*((volatile unsigned long *) 0xE000E400))
#define IPR14								(*((volatile unsigned long *) 0xE000E438))


#endif
