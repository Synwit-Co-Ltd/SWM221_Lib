#ifndef __SWM221_ADC_H__
#define	__SWM221_ADC_H__


typedef struct {
	uint8_t  clkdiv;		//1-32
	uint8_t  samplAvg;		//ADC_AVG_SAMPLE1、ADC_AVG_SAMPLE2、ADC_AVG_SAMPLE4、ADC_AVG_SAMPLE8
} ADC_InitStructure;


typedef struct {
	uint8_t  trig_src;		//ADC序列触发方式：ADC_TRIGGER_SW、ADC_TRIGGER_TIMER0、ADC_TRIGGER_TIMER1、... ...
	uint8_t  samp_tim;		//ADC序列采样时间，可取值4--259
	uint8_t  conv_cnt;		//ADC序列转换次数，可取值1--256
	uint8_t  EOCIntEn;		//转换完成中断使能，可取值0、1
	uint8_t *channels;		//序列转换通道选择，元素为 ADC_CH0、ADC_CH1、...、ADC_CH11 的数组，最多 8 个通道，以 0xF 结束，通道可重复
} ADC_SEQ_InitStructure;


typedef struct {
	uint16_t UpperLimit;	//比较上限值
	uint16_t UpperLimitIEn;	//ADC转换结果大于UpperLimit中断使能
	uint16_t LowerLimit;	//比较下限值
	uint16_t LowerLimitIEn;	//ADC转换结果小于LowerLimit中断使能
} ADC_CMP_InitStructure;


#define ADC_SEQ0	0x1
#define ADC_SEQ1	0x2

#define ADC_CH0		0
#define ADC_CH1		1
#define ADC_CH2		2
#define ADC_CH3		3
#define ADC_CH4		4
#define ADC_CH5		5
#define ADC_CH6		6
#define ADC_CH7		7
#define ADC_CH8		8
#define ADC_CH9		9


#define ADC_AVG_SAMPLE1			0
#define ADC_AVG_SAMPLE2			1	//一次启动连续采样、转换2次，并计算两次结果的平均值作为转换结果
#define ADC_AVG_SAMPLE4			2
#define ADC_AVG_SAMPLE8			3

#define ADC_TRIGGER_NO			0
#define ADC_TRIGGER_SW			1	//软件启动
#define ADC_TRIGGER_TIMER0		2
#define ADC_TRIGGER_TIMER1		3
#define ADC_TRIGGER_TIMER2		4
#define ADC_TRIGGER_PWM0		16
#define ADC_TRIGGER_PWM1		17


/* Interrupt Type */
#define ADC_IT_EOC			(1 << 0)	//End Of Conversion
#define ADC_IT_CMP_MAX		(1 << 1)	//转换结果大于COMP.MAX
#define ADC_IT_CMP_MIN		(1 << 2)	//转换结果小于COMP.MIN


void ADC_Init(ADC_TypeDef * ADCx, ADC_InitStructure * initStruct);		//ADC模数转换器初始化
void ADC_SEQ_Init(ADC_TypeDef * ADCx, uint32_t seq, ADC_SEQ_InitStructure * initStruct);	//ADC序列初始化
void ADC_CMP_Init(ADC_TypeDef * ADCx, uint32_t seq, ADC_CMP_InitStructure * initStruct);	//ADC比较功能初始化
void ADC_Open(ADC_TypeDef * ADCx);							//ADC开启，可以软件启动、或硬件触发ADC转换
void ADC_Close(ADC_TypeDef * ADCx);							//ADC关闭，无法软件启动、或硬件触发ADC转换
void ADC_Start(uint32_t ADC0_seq, uint32_t ADC1_seq);		//启动指定ADC，开始模数转换
void ADC_Stop(uint32_t ADC0_seq, uint32_t ADC1_seq);		//关闭指定ADC，停止模数转换
bool ADC_Busy(ADC_TypeDef * ADCx);

uint32_t ADC_Read(ADC_TypeDef * ADCx, uint32_t chn);		//从指定通道读取转换结果
uint32_t ADC_DataAvailable(ADC_TypeDef * ADCx, uint32_t chn);			//指定通道是否有数据可读取


void ADC_INTEn(ADC_TypeDef * ADCx, uint32_t seq, uint32_t it);
void ADC_INTEn(ADC_TypeDef * ADCx, uint32_t seq, uint32_t it);
void ADC_INTClr(ADC_TypeDef * ADCx, uint32_t seq, uint32_t it);
uint32_t ADC_INTStat(ADC_TypeDef * ADCx, uint32_t seq, uint32_t it);


#endif //__SWM221_ADC_H__
