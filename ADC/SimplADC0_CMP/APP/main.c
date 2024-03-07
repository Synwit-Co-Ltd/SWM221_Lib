#include <string.h>
#include "SWM221.h"

#define ADC_LIMIT_MIN	400
#define ADC_LIMIT_MAX	3600

#define ADC_SIZE  16
uint16_t ADC_Result[ADC_SIZE] = {0};


void SerialInit(void);

int main(void)
{
	DMA_InitStructure DMA_initStruct;
	ADC_InitStructure ADC_initStruct;
	ADC_SEQ_InitStructure ADC_SEQ_initStruct;
	ADC_CMP_InitStructure ADC_CMP_initStruct;
	
	SystemInit();
	
	SerialInit();
	
	PORT_Init(PORTA, PIN14, PORTA_PIN14_ADC0_CH0, 0);		//PA.14 => ADC0.CH0
	PORT_Init(PORTA, PIN11, PORTA_PIN11_ADC0_CH1, 0);		//PA.11 => ADC0.CH1
	PORT_Init(PORTA, PIN8,  PORTA_PIN8_ADC0_CH2,  0);		//PA.8  => ADC0.CH2
	PORT_Init(PORTB, PIN6,  PORTB_PIN6_ADC0_CH5,  0);		//PB.6  => ADC0.CH5
	PORT_Init(PORTB, PIN5,  PORTB_PIN5_ADC0_CH6,  0);		//PB.5  => ADC0.CH6
	PORT_Init(PORTB, PIN4,  PORTB_PIN4_ADC0_CH7,  0);		//PB.4  => ADC0.CH7
	PORT_Init(PORTB, PIN2,  PORTB_PIN2_ADC0_CH8,  0);		//PB.2  => ADC0.CH8

	ADC_initStruct.clkdiv = 4;
	ADC_initStruct.samplAvg = ADC_AVG_SAMPLE1;
	ADC_Init(ADC0, &ADC_initStruct);
	
	ADC_SEQ_initStruct.trig_src = ADC_TRIGGER_TIMER2;
	ADC_SEQ_initStruct.samp_tim = 6;
	ADC_SEQ_initStruct.conv_cnt = ADC_SIZE;
	ADC_SEQ_initStruct.EOCIntEn = 0;
	ADC_SEQ_initStruct.channels = (uint8_t []){ ADC_CH0, 0 };
	ADC_SEQ_Init(ADC0, ADC_SEQ1, &ADC_SEQ_initStruct);
	
	ADC_CMP_initStruct.LowerLimit = ADC_LIMIT_MIN;
	ADC_CMP_initStruct.LowerLimitIEn = 1;
	ADC_CMP_initStruct.UpperLimit = ADC_LIMIT_MAX;
	ADC_CMP_initStruct.UpperLimitIEn = 1;
	ADC_CMP_Init(ADC0, ADC_SEQ1, &ADC_CMP_initStruct);
	
	ADC0->CR |= (1 << ADC_CR_SEQ1DMAEN_Pos);
	
	ADC_Open(ADC0);
	
	DMA_initStruct.Mode = DMA_MODE_CIRCLE;
	DMA_initStruct.Unit = DMA_UNIT_HALFWORD;
	DMA_initStruct.Count = ADC_SIZE;
	DMA_initStruct.PeripheralAddr = (uint32_t)&ADC0->SEQ1DMA;
	DMA_initStruct.PeripheralAddrInc = 0;
	DMA_initStruct.MemoryAddr = (uint32_t)ADC_Result;
	DMA_initStruct.MemoryAddrInc = 1;
	DMA_initStruct.Handshake = DMA_CH0_ADC0SEQ1;
	DMA_initStruct.Priority = DMA_PRI_LOW;
	DMA_initStruct.INTEn = 0;
	DMA_CH_Init(DMA_CH0, &DMA_initStruct);
	DMA_CH_Open(DMA_CH0);
	
	TIMR_Init(TIMR2, TIMR_MODE_TIMER, CyclesPerUs, 1000, 0);
	TIMR_Start(TIMR2);
	
	while(1==1)
	{
	}
}


void ADC_Handler(void)
{
	uint16_t adc_result[ADC_SIZE];
	
	if(ADC_INTStat(ADC0, ADC_SEQ1, ADC_IT_CMP_MAX))
	{
		ADC_INTClr(ADC0, ADC_SEQ1, ADC_IT_CMP_MAX);
		
		memcpy(adc_result, ADC_Result, sizeof(ADC_Result));
		
		printf("ADC Value > %d found:\n", ADC_LIMIT_MAX);
		for(int i = 0; i < ADC_SIZE; i++)
			printf("%d, ", adc_result[i]);
		printf("\n\n");
	}
	
	if(ADC_INTStat(ADC0, ADC_SEQ1, ADC_IT_CMP_MIN))
	{
		ADC_INTClr(ADC0, ADC_SEQ1, ADC_IT_CMP_MIN);
		
		memcpy(adc_result, ADC_Result, sizeof(ADC_Result));
		
		printf("ADC Value < %d found:\n", ADC_LIMIT_MIN);
		for(int i = 0; i < ADC_SIZE; i++)
			printf("%d, ", adc_result[i]);
		printf("\n\n");
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
