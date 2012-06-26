#include "lpc13xx.h"
#include "SPI_Protocol.h"
#include "Delay_Timer.h"

#define RESET_DELAY_US		(400)
#define SPI_WRITE_DELAY   (40)
#define ADS_START_DELAY   (80)

void InitSPI(void)
{
	
	//Enable power to SSP0
	
	LPC_SYSCON->SYSAHBCLKCTRL |= (1<<11);	//Enable SSP0 clock
	
	
	//Set Peripheral Clock Frequency to 72MHz
	LPC_SYSCON->SSP0CLKDIV |= (1<<0);     //Divide by 2
	
	// Configure SPI PINs
	LPC_IOCON->SCK_LOC |= (1<<0);					  //SCK0 pin location at PIO2_11
	LPC_IOCON->PIO2_11 |= (1<<0);					  //Enable PIO2_11 as SCK0 pin
	LPC_IOCON->PIO0_8 |= (1<<0);						//Enable PIO0_8 as MISO0 
	LPC_IOCON->PIO0_9 |= (1<<0);						//Enable PIO0_9 as MOSI0
	LPC_IOCON->PIO0_2 |= (13<<4);						//Enable PIO0_2 as GPIO CS
	LPC_GPIO0->DIR |= (1<<2);								//PIO0_2 CS pin configured as Output
	LPC_SYSCON->PRESETCTRL |= (7<<0);				//Pull SSP0 block out of reset mode
	
	//Configure SPI Control Register 0. CR1 is correct at default so no need to configure
	
	LPC_SSP0->CR0 |= (7<<0);   							//8-bit data tranfer per frame		
	LPC_SSP0->CR0 |= (1<<7);								//CPHA is set to 0
	LPC_SSP0->CPSR |= 0x14;									//CPSDVSR = 2, clock prescaler
	
	LPC_SSP0->CR1 |= (1<<1);								//Enable SSP Controller
	
	return;
	
	
}

//write to SPI0 
void SPI0_Write(unsigned char Data)
{
	LPC_SSP0->DR = Data;
	while((LPC_SSP0->SR & 0x00000010));
	return;
}

//read from SPI0
unsigned char SPI0_Read()
{
	unsigned char Data;
	//for dummy data write 0x00 to SPI to activate clock while reading the incoming data from ADS
	LPC_SSP0->DR = 0x00;
	while(!(LPC_SSP0->SR & 0x00000008));
	Data = LPC_SSP0->DR;
	return Data;
}

void Init_SPI_wADS(void)
{
	unsigned char write_Array[28] = {0x40, 0x19, 0x7F, 0x86, 0x10, 0xDC, 0x03, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0xFF, 0x02, 0, 0xFE, 0x06, 0, 0, 0, 0x02, 0x0A, 0xE3};
	unsigned char datain[26];			//register information
	int a;
	
	//Configure ADS Start pin
	LPC_IOCON->PIO0_3 |= (13<<4);						//Enable PIO0_3 as SPI_Start, Set high to begin conversions
	LPC_GPIO0->DIR |= (1<<3);								//PIO0_3 SPI_Start configured as output
	
	//Configure ADS Reset pin
	LPC_IOCON->PIO0_7 |= (13<<4);						//Enable PIO0_7 as SPI_Reset
	LPC_GPIO0->DIR |= (1<<7);								//PIO0_7 configured as output
	
	/*
	//Configure ADS DRDY pin
	LPC_IOCON->PIO0_5 |= (13<<4);						//Enable PIO0_5 as SPI_DRDY
	LPC_GPIO0->DIR &= ~(1<<5);							//PIO0_5 configured as input
	*/
		
	LPC_GPIO0->DATA &= ~(1<<3); 						//Set SPI_Start pin low, Do not begin conversions
	
	LPC_GPIO0->DATA |= (1<<2); 							//Set CS pin to high		

	//Reset ADS 1298
	LPC_GPIO0->DATA |= (1<<7)	;							//Set SPI_Reset pin high
	delay(RESET_DELAY_US);
	LPC_GPIO0->DATA &= ~(1<<7);							//Set SPI_Reset pin low
	delay(RESET_DELAY_US);
	LPC_GPIO0->DATA |= (1<<7);								//Set SPI_Reset pin high
	
	
	//Stop read data continuously mode
	LPC_GPIO0->DATA &= ~(1<<2); 							//Set CS pin low
	delay(SPI_WRITE_DELAY);
	SPI0_Write(0x11);
	delay(SPI_WRITE_DELAY);
	LPC_GPIO0->DATA |= (1<<2); 							//Set CS pin high	
	delay(SPI_WRITE_DELAY);
	
	/*
	//Write Registry Commands
	//Values for opcode 1 and opcode 2 as well as Registers 0-25 of ADS stored in this array
	0. ID = 0x92
	1. Config 1 = 0x06 (for low-power mode  and 250kSPS)
	2. Config 2 = 0x10 (default configuration for test signal)
	3. Config 3 = 0xDC (VREFP = 2.4V) or 0xDC
	4. LOFF = 0x03
	5. 
	...
	20. GPIO = 0x0f
	...
	25. WCT2 = 0x00
	*/	
	
	LPC_GPIO0->DATA &= ~(1<<2); 							//Set CS pin low
	
	for (a=0; a<28; a++){
		SPI0_Write(write_Array[a]);
		delay(SPI_WRITE_DELAY);
	}

	delay(SPI_WRITE_DELAY);
	LPC_GPIO0->DATA |= (1<<2); 							//Set CS pin high
	delay(SPI_WRITE_DELAY);
	
	/*
	//Read Registry Commands
	LPC_GPIO0->DATA &= ~(1<<2); 							//Set CS pin low
	SPI0_Write(0x20);
	delay(SPI_WRITE_DELAY);
	SPI0_Write(26);
	delay(SPI_WRITE_DELAY);
	
	for (a=0; a<26; a++){
		LPC_SSP0->DR = 0x00;
		while(!(LPC_SSP0->SR & 0x00000008));
		datain[a] = (LPC_SSP0->DR & 0x000000ff);
		delay(SPI_WRITE_DELAY);
	}
	
	LPC_GPIO0->DATA |= (1<<2); 							//Set CS pin high
	*/
	
	
	
	//Enable Read Data Continiously Mode
	LPC_GPIO0->DATA &= ~(1<<2); 							//Set CS pin low
	delay(SPI_WRITE_DELAY);
	SPI0_Write(0x10);
	delay(SPI_WRITE_DELAY);
	LPC_GPIO0->DATA |= (1<<2); 							//Set CS pin high
	delay(SPI_WRITE_DELAY);
	
	
	LPC_GPIO0->DATA |= (1<<3);							//SPI_Start Pin set high to begin conversion
	delay(ADS_START_DELAY);
	
	return;
}
	

	
