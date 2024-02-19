#include "SWM221.h"

void SerialInit(void);

int main(void)
{	
	SystemInit();
	
	SerialInit();
	
	PORT_Init(PORTB, PIN8,  PORTB_PIN8_OPA0_INN,  0);
	PORT_Init(PORTB, PIN7,  PORTB_PIN7_OPA0_INP,  0);
	PORT_Init(PORTB, PIN9,  PORTB_PIN9_OPA0_OUT,  0);
	PORT_Init(PORTA, PIN13, PORTA_PIN13_OPA1_INN, 0);
	PORT_Init(PORTA, PIN9,  PORTA_PIN9_OPA1_INP,  0);
	PORT_Init(PORTA, PIN8,  PORTA_PIN8_OPA1_OUT,  0);
	PORT_Init(PORTB, PIN0,  PORTB_PIN0_OPA2_INN,  0);
	PORT_Init(PORTB, PIN1,  PORTB_PIN1_OPA2_INP,  0);
	PORT_Init(PORTB, PIN2,  PORTB_PIN2_OPA2_OUT,  0);
	
	SYS->PGA0CR = (1 << SYS_PGA0CR_EN_Pos) |
				  (0 << SYS_PGA0CR_MODE_Pos);
	
	SYS->PGA1CR = (1 << SYS_PGA1CR_EN_Pos) |
				  (0 << SYS_PGA1CR_MODE_Pos);
	
	SYS->PGA2CR = (1 << SYS_PGA2CR_EN_Pos) |
				  (0 << SYS_PGA2CR_MODE_Pos);
	
	while(1==1)
	{
	}
}


void SerialInit(void)
{
	UART_InitStructure UART_initStruct;
	
	PORT_Init(PORTA, PIN0, PORTA_PIN0_UART0_RX, 0);	//GPIOA.0配置为UART0 RXD
	PORT_Init(PORTA, PIN1, PORTA_PIN1_UART0_TX, 1);	//GPIOA.1配置为UART0 TXD
 	
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
