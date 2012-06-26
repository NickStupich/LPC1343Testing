#ifndef _SPI_Protocol
#define SPI_Protocol

void InitSPI(void);
void SPI0_Write(unsigned char Data);
unsigned char SPI0_Read(void);
void Init_SPI_wADS(void);

#endif
