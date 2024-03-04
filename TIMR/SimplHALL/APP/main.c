#include "SWM221.h"

void SerialInit(void);
void TestSignal(void);

int main(void)
{	
	SystemInit();
	
	SerialInit();
	
	TestSignal();
	
	PORT_Init(PORTB, PIN4, PORTB_PIN4_HALL_IN0, 1);		//连接PA9
	PORT_Init(PORTB, PIN5, PORTB_PIN5_HALL_IN1, 1);		//连接PA10
	PORT_Init(PORTB, PIN6, PORTB_PIN6_HALL_IN2, 1);		//连接PA11
	PORTB->PULLU |= ((1 << PIN4) | (1 << PIN5) | (1 << PIN6));
	
	TIMR_Init(TIMR0, TIMR_MODE_TIMER, CyclesPerUs, 2000000, 1);		//2秒钟未检测到HALL输入变化，触发超时中断
	
	TIMRG->HALLEN = 1;
	TIMRG->HALLIF = 7;
	TIMRG->HALLIE = 1;
	NVIC_EnableIRQ(HALL_IRQn);
	
	TIMR_Start(TIMR0);
	
	while(1==1)
	{
	}
}


void HALL_Handler(void)
{
	TIMRG->HALLIF = 7;
	
	printf("%dus\r\n", TIMRG->HALLDR);
}


void TIMR0_Handler(void)
{
	TIMR0->IF = (1 << TIMR_IF_TO_Pos);
	
	printf("HALL Time-out\r\n");
}


void TestSignal(void)
{
	GPIO_Init(GPIOA, PIN9,  1, 0, 0, 0);
	GPIO_Init(GPIOA, PIN10, 1, 0, 0, 0);
	GPIO_Init(GPIOA, PIN11, 1, 0, 0, 0);
	
	TIMR_Init(TIMR1, TIMR_MODE_TIMER, CyclesPerUs, 100000, 1);
	TIMR_Start(TIMR1);
}


void TIMR1_Handler(void)
{
	static uint32_t setp = 0;
	
	TIMR1->IF = (1 << TIMR_IF_TO_Pos);
	
	switch(setp++)
	{
	case 0: GPIO_SetBit(GPIOA, PIN9);  break;
	case 1: GPIO_SetBit(GPIOA, PIN10); break;
	case 2: GPIO_SetBit(GPIOA, PIN11); break;
	case 3: GPIO_ClrBit(GPIOA, PIN9);  break;
	case 4: GPIO_ClrBit(GPIOA, PIN10); break;
	case 5: GPIO_ClrBit(GPIOA, PIN11); setp = 0; break;
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
