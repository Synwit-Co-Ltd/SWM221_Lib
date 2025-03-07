#include "SWM221.h"


/* QSPI use as normal SPI (half-duplex) */


void SerialInit(void);

int main(void)
{
	QSPI_InitStructure QSPI_initStruct;
	uint8_t txbuf[8] = { 0x01, 0x12, 0x23, 0x34, 0x45, 0x56, 0x67, 0x78 };
	uint8_t rxbuf[8] = { 0x00 };
	
	SystemInit();
	
	SerialInit();
	
	PORT_Init(PORTC, PIN3,  PORTC_PIN3_QSPI0_SCLK,  0);
	PORT_Init(PORTC, PIN2,  PORTC_PIN2_QSPI0_SSEL,  0);
	PORT_Init(PORTA, PIN15, PORTA_PIN15_QSPI0_MOSI, 1);
	PORT_Init(PORTB, PIN0,  PORTB_PIN0_QSPI0_MISO,  1);
//	PORT_Init(PORTB, PIN1,  PORTB_PIN1_QSPI0_D2,    1);
//	PORT_Init(PORTB, PIN2,  PORTB_PIN2_QSPI0_D3,    1);
	
	QSPI_initStruct.Size = QSPI_Size_16MB;
	QSPI_initStruct.ClkDiv = 8;
	QSPI_initStruct.ClkMode = QSPI_ClkMode_3;
	QSPI_initStruct.SampleShift = (QSPI_initStruct.ClkDiv == 2) ? QSPI_SampleShift_1_SYSCLK : QSPI_SampleShift_NONE;
	QSPI_initStruct.IntEn = 0;
	QSPI_Init(QSPI0, &QSPI_initStruct);
	QSPI_Open(QSPI0);
	
	while(1==1)
	{
		QSPI_SPI_Write(QSPI0, txbuf, sizeof(txbuf));
		
		for(int i = 0; i < SystemCoreClock / 16; i++) __NOP();
		
		QSPI_SPI_Read(QSPI0, rxbuf, sizeof(rxbuf));
		
		printf("Received data: %02X,  %02X,  %02X,  %02X,  %02X,  %02X,  %02X,  %02X\n",
					rxbuf[0], rxbuf[1], rxbuf[2], rxbuf[3], rxbuf[4], rxbuf[5], rxbuf[6], rxbuf[7]);
		
		for(int i = 0; i < SystemCoreClock / 16; i++) __NOP();
	}
}


void SerialInit(void)
{
	UART_InitStructure UART_initStruct;
	
	PORT_Init(PORTA, PIN0, PORTA_PIN0_UART0_RX, 1);	//GPIOA.0配置为UART0 RXD
	PORT_Init(PORTA, PIN1, PORTA_PIN1_UART0_TX, 0);	//GPIOA.1配置为UART0 TXD
 	
 	UART_initStruct.Baudrate = 57600;
	UART_initStruct.DataBits = UART_DATA_8BIT;
	UART_initStruct.Parity = UART_PARITY_NONE;
	UART_initStruct.StopBits = UART_STOP_1BIT;
	UART_initStruct.RXThreshold = 3;
	UART_initStruct.RXThresholdIEn = 0;
	UART_initStruct.TXThreshold = 3;
	UART_initStruct.TXThresholdIEn = 0;
	UART_initStruct.TimeoutTime = 10;
	UART_initStruct.TimeoutIEn = 0;
 	UART_Init(UART0, &UART_initStruct);
	UART_Open(UART0);
}

/****************************************************************************************************************************************** 
* 函数名称: fputc()
* 功能说明: printf()使用此函数完成实际的串口打印动作
* 输    入: int ch		要打印的字符
*			FILE *f		文件句柄
* 输    出: 无
* 注意事项: 无
******************************************************************************************************************************************/
int fputc(int ch, FILE *f)
{
	UART_WriteByte(UART0, ch);
	
	while(UART_IsTXBusy(UART0));
 	
	return ch;
}
