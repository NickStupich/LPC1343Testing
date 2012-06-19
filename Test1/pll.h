#include "lpc13xx.h"

#define SYSPLLCLKSEL	(*((volatile unsigned long *) 0x40048040))
#define SYSPLLCLKUEN	(*((volatile unsigned long *) 0x40048044))
#define MAINCLKSEL		(*((volatile unsigned long *) 0x40048070))
#define MAINCLKUEN		(*((volatile unsigned long *) 0x40048074))
#define SYSPLLCTRL		(*((volatile unsigned long *) 0x40048008))
#define SYSPLLSTAT		(*((volatile unsigned long *) 0x4004800C))
#define PDRUNCFG			(*((volatile unsigned long *) 0x40048238))



/**********************************************************************************/
/*  Function name: Init_PLL                                                       */
/*  	Parameters                                                                */
/*          Input   :  PLL_Fclkin, PLL_Clock_Source, PLL_Fclkout                  */
/*          Output  :  No	                                                  */
/*	Action: Adjust PLL clock out       					  */
/**********************************************************************************/
void Init_PLL(unsigned long PLL_Fclkin, char PLL_Clock_Source, unsigned long PLL_Fclkout){
  unsigned long MSEL = 0x00000000;
  unsigned long PSEL = 0x00000000;
  
  // 1. Power Down PLL before initialization
  PDRUNCFG |= 0x00000080;       // Set SYSPLL_PD
  // 2. Select the System PLL clock source:
  SYSPLLCLKUEN &= ~0x00000001;  // No change 
  if(PLL_Clock_Source == 'I')     { SYSPLLCLKSEL = 0x00000000;  }     // IRC oscillator
  else if(PLL_Clock_Source == 'E'){ SYSPLLCLKSEL = 0x00000001;  }     // System oscillator
  else if(PLL_Clock_Source == 'W'){ SYSPLLCLKSEL = 0x00000002;  }     // WDT oscillator
  else                            { return;                     }     // Error in function call
  SYSPLLCLKUEN |= 0x00000001;   // Update clock source 
  // Definitions are changed! SYSPLLCLKUEN is really SYSPLLUEN in datasheet!
  // 3. Specify the PLL input clock frequency Fclkin.
  // PLL_Fclkin = 12000000MHz -> Internal RC oscillator (IRC), 12MHz/1% or Exernal Qrystall oscillator
  // 4. Boost system PLL to desired Fcco clock: 192MHz and then divide by M to obtain desired clock
  // FCCO - Frequency of the Current Controlled Oscillator (CCO); 156 to 320 MHz.
  SYSPLLCTRL |= 0x00000020; // Set PSEL to 01, i.e. 1 then Fcco = PLL_Fclkout * (2*PSEL) = 192MHz
  // PSEL = 192MHz / 2*PLL_Fclkout = 96MHz/PLL_Fclkout
  PSEL = 96000000/PLL_Fclkout;
  if(PSEL < 2)      { SYSPLLCTRL &= ~0x00000060; }  // PSEL = 00, P = 1
  else if(PSEL < 4) { SYSPLLCTRL |=  0x00000020; }  // PSEL = 01, P = 2
  else if(PSEL < 8) { SYSPLLCTRL |=  0x00000040; }  // PSEL = 10, P = 4
  else              { SYSPLLCTRL |=  0x00000060;  } // PSEL = 10, P = 8
  
  // 5. Calculate MSEL to obtain the desired output frequency Fclkout with (MSEL + 1) = Fclkout / Fclkin.
  MSEL = PLL_Fclkout/PLL_Fclkin;
  MSEL = ((MSEL & 0x000001F) - 1);
  // 6. Write MSEL
  SYSPLLCTRL |= MSEL;
  // 7. Enable the main system PLL by clearing bit 7 in PDRUNCFG
  PDRUNCFG &= ~0x00000080;       // Set SYSPLL_PD
  // 8. Then wait until the PLL clock is locked. SYSPLLSTAT bit LOCK
  while(!(SYSPLLSTAT & 0x000000001)){}  //If 1 -> PLL locked
}



void SystemInit()
{
	//Init_PLL(12000000, 'E', 72000000);
	/****clock to 72Mhz with PLL****/
	/*
	SYSPLLCLKSEL = 0x1;	//system oscillator as source for pll
	SYSPLLCTRL = 0x2 | (0x1 << 5);	//feedback divider = (2+1), post divier = 1
	
	//wait for PLL to synchronize
	while(!SYSPLLSTAT);
	
	//pll update enable register
	SYSPLLCLKUEN = 0x0;
	SYSPLLCLKUEN = 0x1;
	
	MAINCLKSEL = 0x3; //select system pll output
	
	//clock update enable register - makes new settings take effect
	MAINCLKUEN = 0x0;
	MAINCLKUEN = 0x1;
	*/
}
