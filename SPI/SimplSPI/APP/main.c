#include "SWM221.h"

void SerialInit(void);

int main(void)
{	
	uint32_t rxdata, txdata = 0x23;
	SPI_InitStructure SPI_initStruct;
	
	SystemInit();
	
	SerialInit();
	
	PORT_Init(PORTB, PIN15, PORTB_PIN15_SPI0_SSEL, 0);
	PORT_Init(PORTB, PIN10, PORTB_PIN10_SPI0_SCLK, 0);
	PORT_Init(PORTB, PIN13, PORTB_PIN13_SPI0_MOSI, 0);	//将MOSI与MISO连接，自发、自收、然后打印
	PORT_Init(PORTB, PIN14, PORTB_PIN14_SPI0_MISO, 1);

	SPI_initStruct.clkDiv = SPI_CLKDIV_4;
	SPI_initStruct.FrameFormat = SPI_FORMAT_SPI;
	SPI_initStruct.SampleEdge = SPI_SECOND_EDGE;
	SPI_initStruct.IdleLevel = SPI_LOW_LEVEL;
	SPI_initStruct.WordSize = 8;
	SPI_initStruct.Master = 1;
	SPI_initStruct.RXThreshold = 0;
	SPI_initStruct.RXThresholdIEn = 0;
	SPI_initStruct.TXThreshold = 0;
	SPI_initStruct.TXThresholdIEn = 0;
	SPI_initStruct.TXCompleteIEn  = 0;
	SPI_Init(SPI0, &SPI_initStruct);
	SPI_Open(SPI0);
	
	while(1==1)
	{
		rxdata = SPI_ReadWrite(SPI0, txdata);
		txdata = rxdata + 1;		//将SPI0_MOSI与SPI0_MISO短接，则可看到打印数值加一递增
		
		printf("rxdata: 0x%X\r\n", rxdata);
		
		for(int i = 0; i < SystemCoreClock/10; i++);
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
	UART_initStruct.RXThresholdIEn = 0;
	UART_initStruct.TXThresholdIEn = 0;
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
