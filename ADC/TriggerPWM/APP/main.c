#include "SWM221.h"

void SerialInit(void);
void PWM0AInit(void);

int main(void)
{
	ADC_InitStructure ADC_initStruct;
	ADC_SEQ_InitStructure ADC_SEQ_initStruct;
	
 	SystemInit();
	
	SerialInit();
	
	GPIO_Init(GPIOA, PIN5, 1, 0, 0, 0);
	
	PORT_Init(PORTA, PIN14, PORTA_PIN14_ADC0_CH0, 0);		//PA.14 => ADC0.CH0
	PORT_Init(PORTA, PIN11, PORTA_PIN11_ADC0_CH1, 0);		//PA.11 => ADC0.CH1，注意：此通道只在 ADC 参考源为 ADC_REF_VDD 时可用
	PORT_Init(PORTA, PIN8,  PORTA_PIN8_ADC0_CH2,  0);		//PA.8  => ADC0.CH2
	PORT_Init(PORTB, PIN6,  PORTB_PIN6_ADC0_CH5,  0);		//PB.6  => ADC0.CH5
	PORT_Init(PORTB, PIN5,  PORTB_PIN5_ADC0_CH6,  0);		//PB.5  => ADC0.CH6
	PORT_Init(PORTB, PIN4,  PORTB_PIN4_ADC0_CH7,  0);		//PB.4  => ADC0.CH7
	PORT_Init(PORTB, PIN2,  PORTB_PIN2_ADC0_CH8,  0);		//PB.2  => ADC0.CH8
	
	ADC_initStruct.clkdiv = 4;
	ADC_initStruct.refsrc = ADC_REF_VDD;
	ADC_initStruct.samplAvg = ADC_AVG_SAMPLE1;
	ADC_Init(ADC0, &ADC_initStruct);
	
	ADC_SEQ_initStruct.trig_src = ADC_TRIGGER_PWM0;		// PWM0 触发 ADC 通道 0 转换，产生中断
	ADC_SEQ_initStruct.samp_tim = 6;
	ADC_SEQ_initStruct.conv_cnt = 1;
	ADC_SEQ_initStruct.EOCIntEn = 1;
	ADC_SEQ_initStruct.channels = (uint8_t []){ ADC_CH0, 0xF };
	ADC_SEQ_Init(ADC0, ADC_SEQ0, &ADC_SEQ_initStruct);
	
	ADC_SEQ_initStruct.trig_src = ADC_TRIGGER_SW;		// 软件 启动 ADC 通道 2 转换，不产生中断
	ADC_SEQ_initStruct.samp_tim = 6;
	ADC_SEQ_initStruct.conv_cnt = 1;
	ADC_SEQ_initStruct.EOCIntEn = 0;
	ADC_SEQ_initStruct.channels = (uint8_t []){ ADC_CH2, 0xF };
	ADC_SEQ_Init(ADC0, ADC_SEQ1, &ADC_SEQ_initStruct);
	
	ADC_Open(ADC0);
	
	PWM0AInit();
	
	while(1==1)
	{
		/* 1、若 ADC 正在执行 SEQ0 转换，则此软件启动被忽略，不会产生 SEQ1 转换结果
		 * 2、若 ADC 正在执行 SEQ1 转换，新到来的 PWM 触发会打断、并终止软件启动的 SEQ1 转换，不会产生 SEQ1 转换结果
		*/
		ADC_Start(ADC_SEQ1, 0);
		while(ADC_Busy(ADC0)) __NOP();
		if(ADC_DataAvailable(ADC0, ADC_CH2))
		{
			printf("%d,", ADC_Read(ADC0, ADC_CH2));
		}
		
		for(int i = 0; i < SystemCoreClock/1000; i++) {}
	}
}


void ADC_Handler(void)
{
	GPIO_InvBit(GPIOA, PIN5);
	
	if(ADC_INTStat(ADC0, ADC_SEQ0, ADC_IT_EOC))
	{
		ADC_INTClr(ADC0, ADC_SEQ0, ADC_IT_EOC);
		
//		printf("%d,", ADC_Read(ADC0, ADC_CH0));
	}
}


void PWM0AInit(void)
{
	PWM_InitStructure PWM_initStruct;
	
	PORT_Init(PORTA, PIN3, PORTA_PIN3_PWM0A,  0);
	PORT_Init(PORTA, PIN2, PORTA_PIN2_PWM0AN, 0);
	PORT_Init(PORTA, PIN6, PORTA_PIN6_PWM0B,  0);
	PORT_Init(PORTA, PIN7, PORTA_PIN7_PWM0BN, 0);
	
	PWM_initStruct.Mode = PWM_EDGE_ALIGNED;
	PWM_initStruct.Clkdiv = 6;					//F_PWM = 60M/6 = 10M
	PWM_initStruct.Period = 10000;				//10M/10000 = 1000Hz，中心对称模式下频率降低到500Hz
	PWM_initStruct.HdutyA =  2500;				//2500/10000 = 25%
	PWM_initStruct.DeadzoneA = 50;
	PWM_initStruct.IdleLevelA = 0;
	PWM_initStruct.IdleLevelAN= 0;
	PWM_initStruct.OutputInvA = 0;
	PWM_initStruct.OutputInvAN= 0;
	PWM_initStruct.HdutyB =  5000;				//5000/10000 = 50%
	PWM_initStruct.DeadzoneB = 50;
	PWM_initStruct.IdleLevelB = 0;
	PWM_initStruct.IdleLevelBN= 0;
	PWM_initStruct.OutputInvB = 0;
	PWM_initStruct.OutputInvBN= 0;
	PWM_initStruct.UpOvfIE    = 0;
	PWM_initStruct.DownOvfIE  = 0;
	PWM_initStruct.UpCmpAIE   = 0;
	PWM_initStruct.DownCmpAIE = 0;
	PWM_initStruct.UpCmpBIE   = 0;
	PWM_initStruct.DownCmpBIE = 0;
	PWM_Init(PWM0, &PWM_initStruct);
	PWM_Init(PWM1, &PWM_initStruct);
	
	PWM_Start(PWM0_MSK | PWM1_MSK);
	
	
	/* Mask */
	PWM_CmpTrigger(PWM0, 1500, PWM_DIR_UP, 64, PWM_TRG_1, 0);	//PWM0向上计数计数值等于1500时发出一个触发信号，触发信号发送到 trigger1
	
 	PWM_OutMask(PWM0, PWM_CH_A, PWM_EVT_1, 0, PWM_EVT_1, 1);	//PWM0A和PWM0AN在event1为高时分别输出0和1
 	PWM_OutMask(PWM0, PWM_CH_B, PWM_EVT_1, 0, PWM_EVT_1, 1);	//PWM0B和PWM0BN在event1为高时分别输出0和1
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
