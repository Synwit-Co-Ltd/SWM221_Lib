#include <string.h>
#include "SWM221.h"

const char TX_String[8][32] = {
	"TestString\r\n",
	"TestString1\r\n",
	"TestString12\r\n",
	"TestString123\r\n",
	"TestString1234\r\n",
	"TestString12345\r\n",
	"TestString123456\r\n",
	"TestString1234567\r\n",
};

#define RX_LEN	256
char RX_Buffer[RX_LEN] = { 0 };
char TX_Buffer[RX_LEN] = { 0 };

void SerialInit(void);

int main(void)
{
	SystemInit();
	
	SerialInit();
	
	USART_InitStructure USART_initStruct;
	
 	PORT_Init(PORTA, PIN2, PORTA_PIN2_USART0_TX, 0);
	PORT_Init(PORTA, PIN3, PORTA_PIN3_USART0_RX, 1);	// 连接 PA1，接收 UART0 发出的数据，然后原样发出
 	
 	USART_initStruct.Baudrate = 57600;
	USART_initStruct.DataBits = USART_DATA_8BIT;
	USART_initStruct.Parity = USART_PARITY_NONE;
	USART_initStruct.StopBits = USART_STOP_1BIT;
	USART_initStruct.RXReadyIEn = 0;
	USART_initStruct.TXReadyIEn = 0;
	USART_initStruct.TimeoutIEn = 1;
	USART_initStruct.TimeoutTime = 50;	// 50 个 bit 时间内未接收到新的数据，触发接收超时中断
 	USART_Init(USART0, &USART_initStruct);
	USART_Open(USART0);
	
	DMA_InitStructure DMA_initStruct;
	
	DMA_initStruct.Mode = DMA_MODE_SINGLE;
	DMA_initStruct.Unit = DMA_UNIT_BYTE;
	DMA_initStruct.Count = RX_LEN;
	DMA_initStruct.MemoryAddr = (uint32_t)RX_Buffer;
	DMA_initStruct.MemoryAddrInc = 1;
	DMA_initStruct.PeripheralAddr = (uint32_t)&USART0->RHR;
	DMA_initStruct.PeripheralAddrInc = 0;
	DMA_initStruct.Handshake = DMA_CH0_USART0RX;
	DMA_initStruct.Priority = DMA_PRI_LOW;
	DMA_initStruct.INTEn = 0;
	DMA_CH_Init(DMA_CH0, &DMA_initStruct);
	DMA_CH_Open(DMA_CH0);
	
	DMA_initStruct.Mode = DMA_MODE_SINGLE;
	DMA_initStruct.Unit = DMA_UNIT_BYTE;
	DMA_initStruct.Count = strlen(TX_Buffer);
	DMA_initStruct.MemoryAddr = (uint32_t)TX_Buffer;
	DMA_initStruct.MemoryAddrInc = 1;
	DMA_initStruct.PeripheralAddr = (uint32_t)&USART0->THR;
	DMA_initStruct.PeripheralAddrInc = 0;
	DMA_initStruct.Handshake = DMA_CH1_USART0TX;
	DMA_initStruct.Priority = DMA_PRI_LOW;
	DMA_initStruct.INTEn = 0;
	DMA_CH_Init(DMA_CH1, &DMA_initStruct);
	
	while(1==1)
	{
		for(int i = 0; i < 8; i++)
		{
			for(int j = 0; j < strlen(TX_String[i]); j++)
			{
				UART_WriteByte(UART0, TX_String[i][j]);
	
				while(UART_IsTXBusy(UART0));
			}
			
			for(int j = 0; j < SystemCoreClock/8; j++) __NOP();
		}
	}
}


void GPIOA1_GPIOC1_USART0_Handler(void)
{
	if(USART_INTStat(USART0, USART_IT_RX_TO))
	{
		USART_INTClr(USART0, USART_IT_RX_TO);
		
		int str_len = RX_LEN - DMA_CH_GetRemaining(DMA_CH0);
		memcpy(TX_Buffer, RX_Buffer, str_len);
		
		DMA_CH_SetAddrAndCount(DMA_CH1, (uint32_t)TX_Buffer, str_len);
		DMA_CH_Open(DMA_CH1);
		
		DMA_CH_Close(DMA_CH0);
		DMA_CH_Open(DMA_CH0);	// 接收下一帧数据
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
