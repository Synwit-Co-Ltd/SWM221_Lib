#include "SWM221.h"
#include "CircleBuffer.h"

volatile bool msg_rcvd = false;

CircleBuffer_t CirBuf;

void SerialInit(void);

int main(void)
{
	SystemInit();
	
	SerialInit();
	
	USART_InitStructure USART_initStruct;
	
 	PORT_Init(PORTA, PIN2, PORTA_PIN2_USART0_TX, 0);
	PORT_Init(PORTA, PIN3, PORTA_PIN3_USART0_RX, 1);
 	
 	USART_initStruct.Baudrate = 57600;
	USART_initStruct.DataBits = USART_DATA_8BIT;
	USART_initStruct.Parity = USART_PARITY_NONE;
	USART_initStruct.StopBits = USART_STOP_1BIT;
	USART_initStruct.RXReadyIEn = 1;
	USART_initStruct.TXReadyIEn = 0;
	USART_initStruct.TimeoutIEn = 1;
	USART_initStruct.TimeoutTime = 50;	// 50 个 bit 时间内未接收到新的数据，触发接收超时中断
 	USART_Init(USART0, &USART_initStruct);
	USART_Open(USART0);
	
	while(1==1)
	{
		if(msg_rcvd)
		{
			msg_rcvd = false;
			
			USART_INTEn(USART0, USART_IT_TX_RDY);
		}
	}
}


void GPIOA1_GPIOC1_USART0_Handler(void)
{
	uint8_t chr;
	
	if(USART_INTStat(USART0, USART_IT_RX_RDY))
	{
		chr = USART_Read(USART0);
		
		CirBuf_Write(&CirBuf, &chr, 1);
	}
	else if(USART_INTStat(USART0, USART_IT_RX_TO))
	{
		USART_INTClr(USART0, USART_IT_RX_TO);
		
		msg_rcvd = true;
	}
	
	if(USART_INTStat(USART0, USART_IT_TX_RDY))
	{
		CirBuf_Read(&CirBuf, &chr, 1);
		
		USART_Write(USART0, chr);
		
		if(CirBuf_Empty(&CirBuf))
		{
			USART_INTDis(USART0, USART_IT_TX_RDY);
		}
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
