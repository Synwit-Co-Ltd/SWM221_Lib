#include "SWM221.h"

#define LIN_ID_Sensor	0x11	// 此从机是一个传感器，只能从它读数据
#define LIN_ID_Switch	0x21	// 此从机是一个开关，  只能向它写数据

#define LIN_NB_Sensor	6		// Number of Byte, 传感器的数据是 6 字节
#define LIN_NB_Switch	4

uint8_t Buffer_Sensor[LIN_NB_Sensor] = { 0 };
uint8_t Buffer_Switch[LIN_NB_Switch] = { 0x12, 0x34, 0x56, 0x78 };
volatile uint32_t Index_Sensor = 0;
volatile uint32_t Index_Switch = 0;

volatile uint32_t NACK_Sensor  = 0;

void SerialInit(void);

int main(void)
{
	SystemInit();
	
	SerialInit();
	
	USART_InitStructure USART_initStruct;
	
 	PORT_Init(PORTA, PIN2, PORTA_PIN2_USART0_TX, 0);
	PORT_Init(PORTA, PIN3, PORTA_PIN3_USART0_RX, 1);
 	
 	USART_initStruct.Baudrate = 9600;
	USART_initStruct.DataBits = USART_DATA_8BIT;
	USART_initStruct.Parity = USART_PARITY_NONE;
	USART_initStruct.StopBits = USART_STOP_1BIT;
	USART_initStruct.RXReadyIEn = 1;
	USART_initStruct.TXReadyIEn = 0;
	USART_initStruct.TimeoutIEn = 0;
 	USART_Init(USART0, &USART_initStruct);
	USART_Open(USART0);
	
	USART_LINConfig(USART0, USART_LIN_MASTER, USART_CHECKSUM_LIN13, USART_IT_LIN_ID | USART_IT_LIN_DONE | USART_IT_LIN_NAKERR);
	
	while(1==1)
	{
		USART_LINStart(USART0, LIN_ID_Sensor, USART_LIN_SUBSCRIBE, LIN_NB_Sensor, USART_CHECKSUM_LIN13);	// 从 Sensor 读数据
		
		NACK_Sensor = 0;
		Index_Sensor = 0;
		while(Index_Sensor < LIN_NB_Sensor)
		{
			if(NACK_Sensor)
				goto retry;
		}
		
		printf("Data from Sensor: ");
		for(int i = 0; i < LIN_NB_Sensor; i++)
		{
			printf("%02X, ", Buffer_Sensor[i]);
		}
		printf("\n\n");
		
		USART_LINStart(USART0, LIN_ID_Switch, USART_LIN_PUBLISH, LIN_NB_Switch, USART_CHECKSUM_LIN13);		// 向 Switch 写数据
		
retry:
		for(int i = 0; i < SystemCoreClock/2; i++) __NOP();
	}
}


void USART0_Handler(void)
{
	if(USART_INTStat(USART0, USART_IT_LIN_ID))	// ID 已发送
	{
		if(((USART0->LINMR & USART_LINMR_NACT_Msk) >> USART_LINMR_NACT_Pos) == USART_LIN_PUBLISH)
		{
			USART_INTEn(USART0, USART_IT_TX_RDY);
			
			Index_Switch = 0;
		}
		else
		{
			Index_Sensor = 0;
		}
	}
	
	if(USART_INTStat(USART0, USART_IT_RX_RDY))
	{
		uint8_t chr = USART_Read(USART0);
		
		if(Index_Sensor < LIN_NB_Sensor)
			Buffer_Sensor[Index_Sensor++] = chr;
	}
	else if(USART_INTStat(USART0, USART_IT_RX_TO))
	{
	}
	
	if(USART_INTStat(USART0, USART_IT_TX_RDY))
	{
		USART_Write(USART0, Buffer_Switch[Index_Switch++]);
		
		if(Index_Switch == LIN_NB_Switch)
			USART_INTDis(USART0, USART_IT_TX_RDY);
	}
	
	if(USART_INTStat(USART0, USART_IT_LIN_DONE))
	{
	}
	
	if(USART_INTStat(USART0, USART_IT_LIN_NAKERR))
	{
		NACK_Sensor = 1;
	}
	
	USART_INTClr(USART0, USART_IT_RX_TO | USART_IT_LIN_ID | USART_IT_LIN_DONE | USART_IT_LIN_NAKERR);
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
