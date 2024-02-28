#include "SWM221.h"

#define LIN_ID_Sensor	0x11	// 此从机是一个传感器，只能从它读数据
#define LIN_ID_Switch	0x21	// 此从机是一个开关，  只能向它写数据

#define LIN_NB_Sensor	6		// Number of Byte, 传感器的数据是 6 字节
#define LIN_NB_Switch	4

uint8_t Buffer_Sensor[LIN_NB_Sensor] = { 0x12, 0x34, 0x56, 0x78, 0x55, 0xAA };
uint8_t Buffer_Switch[LIN_NB_Switch] = { 0 };
volatile uint32_t Index_Sensor = 0;
volatile uint32_t Index_Switch = 0;

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
	
	USART_LINConfig(USART0, USART_LIN_SLAVE, USART_CHECKSUM_LIN13, USART_IT_LIN_ID | USART_IT_LIN_DONE);
	
	while(1==1)
	{
		if(Index_Switch == LIN_NB_Switch)
		{
			Index_Switch = 0;
			
			printf("Data from Master: ");
			for(int i = 0; i < LIN_NB_Switch; i++)
			{
				printf("%02X, ", Buffer_Switch[i]);
			}
			printf("\n\n");
		}
	}
}


void GPIOA1_GPIOC1_USART0_Handler(void)
{
	if(USART_INTStat(USART0, USART_IT_LIN_ID))	// ID 已接收
	{
		if(USART0->LINID == LIN_ID_Sensor)
		{
			USART_LINResponse(USART0, USART_LIN_PUBLISH, LIN_NB_Sensor, USART_CHECKSUM_LIN13);		// 传感器发送采集数据
			
			USART_INTEn(USART0, USART_IT_TX_RDY);
			
			Index_Sensor = 0;
		}
		else if(USART0->LINID == LIN_ID_Switch)
		{
			USART_LINResponse(USART0, USART_LIN_SUBSCRIBE, LIN_NB_Switch, USART_CHECKSUM_LIN13);	// 开关接收指令数据
			
			Index_Switch = 0;
		}
		else
		{
			USART_LINResponse(USART0, USART_LIN_IGNORE, 0, 0);
		}
	}
	
	if(USART_INTStat(USART0, USART_IT_RX_RDY))
	{
		uint8_t chr = USART_Read(USART0);
		
		if(Index_Switch < LIN_NB_Switch)
			Buffer_Switch[Index_Switch++] = chr;
			
	}
	else if(USART_INTStat(USART0, USART_IT_RX_TO))
	{
	}
	
	if(USART_INTStat(USART0, USART_IT_TX_RDY))
	{
		if(Index_Sensor < LIN_NB_Sensor)
			USART_Write(USART0, Buffer_Sensor[Index_Sensor++]);
		else
			USART_INTDis(USART0, USART_IT_TX_RDY);
	}
	
	if(USART_INTStat(USART0, USART_IT_LIN_DONE))
	{
	}
	
	USART_INTClr(USART0, USART_IT_RX_TO | USART_IT_LIN_ID | USART_IT_LIN_DONE);
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
