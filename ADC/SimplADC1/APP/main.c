#include "SWM221.h"


#define ADC_USE_INT  0


void SerialInit(void);

int main(void)
{
	ADC_InitStructure ADC_initStruct;
	ADC_SEQ_InitStructure ADC_SEQ_initStruct;
	
	SystemInit();
	
	SerialInit();
	
	PORT_Init(PORTA, PIN13, PORTA_PIN13_ADC1_CH0, 0);		//PA.13 => ADC1.CH0
	PORT_Init(PORTA, PIN12, PORTA_PIN12_ADC1_CH1, 0);		//PA.12 => ADC1.CH1
	PORT_Init(PORTB, PIN9,  PORTB_PIN9_ADC1_CH2,  0);		//PB.9  => ADC1.CH2
	PORT_Init(PORTB, PIN7,  PORTB_PIN7_ADC1_CH3,  0);		//PB.7  => ADC1.CH3
	PORT_Init(PORTB, PIN0,  PORTB_PIN0_ADC1_CH6,  0);		//PB.0  => ADC1.CH6
//	PORT_Init(PORTC, PIN0,  PORTC_PIN0_ADC1_CH7,  0);		//PC.0  => ADC1.CH7, SWCLK
	PORT_Init(PORTB, PIN15, PORTB_PIN15_ADC1_CH8, 0);		//PB.15 => ADC1.CH8
	PORT_Init(PORTB, PIN14, PORTB_PIN14_ADC1_CH9, 0);		//PB.14 => ADC1.CH9
	
	ADC_initStruct.clkdiv = 4;
	ADC_initStruct.samplAvg = ADC_AVG_SAMPLE1;
	ADC_Init(ADC1, &ADC_initStruct);
	
	ADC_SEQ_initStruct.trig_src = ADC_TRIGGER_SW;
	ADC_SEQ_initStruct.samp_tim = 6;
	ADC_SEQ_initStruct.conv_cnt = 1;
	ADC_SEQ_initStruct.EOCIntEn = ADC_USE_INT;
	ADC_SEQ_initStruct.channels = (uint8_t []){ ADC_CH1, 0xF };
	ADC_SEQ_Init(ADC1, ADC_SEQ0, &ADC_SEQ_initStruct);
	
	ADC_Open(ADC1);
	
#if ADC_USE_INT
	ADC_Start(0, ADC_SEQ0);
	
	while(1==1)
	{
	}
#else
	while(1==1)
	{
		ADC_Start(0, ADC_SEQ0);
		while(ADC_Busy(ADC1)) __NOP();
		printf("%4d,", ADC_Read(ADC1, ADC_CH1));
	}
#endif
}


void ADC_Handler(void)
{
	if(ADC_INTStat(ADC1, ADC_SEQ0, ADC_IT_EOC))
	{
		ADC_INTClr(ADC1, ADC_SEQ0, ADC_IT_EOC);
		
		printf("%4d,", ADC_Read(ADC1, ADC_CH1));
		
		ADC_Start(0, ADC_SEQ0);
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
