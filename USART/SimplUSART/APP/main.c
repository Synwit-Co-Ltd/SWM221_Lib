#include "SWM221.h"

void SerialInit(void);

int main(void)
{
	SystemInit();
	
	SerialInit();
	
	printf("Hello World from USART0!\r\n");
	
	while(1==1)
	{
		while(!USART_INTStat(USART0, USART_IT_RX_RDY)) __NOP();
		
		USART_Write(USART0, USART_Read(USART0));
	}
}

void SerialInit(void)
{
	USART_InitStructure USART_initStruct;
	
	PORT_Init(PORTA, PIN2, PORTA_PIN2_USART0_TX, 0);
	PORT_Init(PORTA, PIN3, PORTA_PIN3_USART0_RX, 1);
 	
	USART_initStruct.Baudrate = 57600;
	USART_initStruct.DataBits = USART_DATA_8BIT;
	USART_initStruct.Parity = USART_PARITY_NONE;
	USART_initStruct.StopBits = USART_STOP_1BIT;
	USART_initStruct.RXReadyIEn = 0;
	USART_initStruct.TXReadyIEn = 0;
	USART_initStruct.TimeoutIEn = 0;
 	USART_Init(USART0, &USART_initStruct);
	USART_Open(USART0);
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
	USART_Write(USART0, ch);
	
	while(!USART_INTStat(USART0, USART_IT_TX_EMPTY)) __NOP();
 	
	return ch;
}
