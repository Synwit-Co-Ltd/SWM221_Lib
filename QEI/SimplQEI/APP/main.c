#include "SWM221.h"

volatile int32_t Speed;

void SerialInit(void);
void testSignal(void);

int main(void)
{
	uint32_t i;
	QEI_InitStructure  QEI_initStruct;
	
 	SystemInit();
	
	SerialInit();
	
	testSignal();
	
	PORT_Init(PORTB, PIN4, PORTB_PIN4_QEI_A,   1);
	PORT_Init(PORTB, PIN5, PORTB_PIN5_QEI_B,   1);
	PORT_Init(PORTB, PIN6, PORTB_PIN6_QEI_Z,   1);
	PORT_Init(PORTB, PIN7, PORTB_PIN7_QEI_DIR, 0);
	
	QEI_initStruct.mode   = QEI_MODE_X4;
	QEI_initStruct.maxcnt = 10000;
	QEI_initStruct.swapAB = 0;
	QEI_initStruct.intINDEXEn = 0;
	QEI_initStruct.intMATCHEn = 0;
	QEI_initStruct.intCNTOVEn = 0;
	QEI_initStruct.intERROREn = 0;
	QEI_Init(QEI, &QEI_initStruct);
	
	QEI_Start(QEI);
	
	TIMR_Init(BTIMR1, TIMR_MODE_TIMER, CyclesPerUs, 50000, 1);	//被测信号周期为：1/10KHz * 1000 = 0.1s，测速间隔应小于0.05s
	TIMR_Start(BTIMR1);
	
	while(1==1)
	{
		printf("speed = %+d\r\n", Speed);
		
		for(i = 0; i < CyclesPerUs*100; i++) __NOP();
	}
}


void BTIMR1_Handler(void)
{
	static int16_t Cnt[2] = {0};

	TIMR_INTClr(BTIMR1);
	
	Cnt[1] = Cnt[0];
	Cnt[0] = (int16_t)QEI->POSCNT;
	
	Speed = Cnt[1] - Cnt[0];
}


void testSignal(void)
{
	GPIO_Init(GPIOA, PIN2, 1, 0, 0, 0);		// 接QEI_A
	GPIO_Init(GPIOA, PIN3, 1, 0, 0, 0);		// 接QEI_B
	GPIO_Init(GPIOA, PIN4, 1, 0, 0, 0);		// 接QEI_Z
	GPIO_ClrBit(GPIOA, PIN2);
	GPIO_ClrBit(GPIOA, PIN3);
	GPIO_ClrBit(GPIOA, PIN4);
	
	TIMR_Init(BTIMR0, TIMR_MODE_TIMER, CyclesPerUs, 100, 1);	//10KHz
	TIMR_Start(BTIMR0);
}

void BTIMR0_Handler(void)
{	
	static uint32_t i = 0;
	
	TIMR_INTClr(BTIMR0);
	
	if(i%20000 < 10000)	//模拟正转
	{
		switch(i%4)
		{
		case 0:
			GPIOA->DATAPIN2 = 0;
			GPIOA->DATAPIN3 = 1;
			break;
		
		case 1:
			GPIOA->DATAPIN2 = 0;
			GPIOA->DATAPIN3 = 0;
			break;
		
		case 2:
			GPIOA->DATAPIN2 = 1;
			GPIOA->DATAPIN3 = 0;
			break;
		
		case 3:
			GPIOA->DATAPIN2 = 1;
			GPIOA->DATAPIN3 = 1;
			break;
		}
	}
	else				//模拟反转
	{
		switch(i%4)
		{
		case 0:
			GPIOA->DATAPIN2 = 1;
			GPIOA->DATAPIN3 = 1;
			break;
		
		case 1:
			GPIOA->DATAPIN2 = 1;
			GPIOA->DATAPIN3 = 0;
			break;
		
		case 2:
			GPIOA->DATAPIN2 = 0;
			GPIOA->DATAPIN3 = 0;
			break;
		
		case 3:
			GPIOA->DATAPIN2 = 0;
			GPIOA->DATAPIN3 = 1;
			break;
		}
	}
	
	switch(i%1000)
	{
	case 0: GPIOA->DATAPIN4 = 1; break;
	case 1: GPIOA->DATAPIN4 = 0; break;
	default: break;
	}
	
	i++;
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
