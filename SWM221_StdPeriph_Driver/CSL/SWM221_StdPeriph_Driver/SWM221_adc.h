#ifndef __SWM221_ADC_H__
#define	__SWM221_ADC_H__


typedef struct {
	uint8_t  clkdiv;		//1-32
	uint8_t  samplAvg;		//ADC_AVG_SAMPLE1��ADC_AVG_SAMPLE2��ADC_AVG_SAMPLE4��ADC_AVG_SAMPLE8
} ADC_InitStructure;


typedef struct {
	uint8_t  trig_src;		//ADC���д�����ʽ��ADC_TRIGGER_SW��ADC_TRIGGER_TIMER0��ADC_TRIGGER_TIMER1��... ...
	uint8_t  samp_tim;		//ADC���в���ʱ�䣬��ȡֵ4--259
	uint8_t  conv_cnt;		//ADC����ת����������ȡֵ1--256
	uint8_t  EOCIntEn;		//ת������ж�ʹ�ܣ���ȡֵ0��1
	uint8_t *channels;		//����ת��ͨ��ѡ��Ԫ��Ϊ ADC_CH0��ADC_CH1��...��ADC_CH11 �����飬��� 8 ��ͨ������ 0 ������ͨ�����ظ�
} ADC_SEQ_InitStructure;


typedef struct {
	uint16_t UpperLimit;	//�Ƚ�����ֵ
	uint16_t UpperLimitIEn;	//ADCת���������UpperLimit�ж�ʹ��
	uint16_t LowerLimit;	//�Ƚ�����ֵ
	uint16_t LowerLimitIEn;	//ADCת�����С��LowerLimit�ж�ʹ��
} ADC_CMP_InitStructure;


#define ADC_SEQ0	0x1
#define ADC_SEQ1	0x2

#define ADC_CH0		0x001
#define ADC_CH1		0x002
#define ADC_CH2		0x004
#define ADC_CH3		0x008
#define ADC_CH4		0x010
#define ADC_CH5		0x020
#define ADC_CH6		0x040
#define ADC_CH7		0x080
#define ADC_CH8		0x100
#define ADC_CH9		0x200
#define ADC_CH10	0x400
#define ADC_CH11	0x800


#define ADC_AVG_SAMPLE1			0
#define ADC_AVG_SAMPLE2			1	//һ����������������ת��2�Σ����������ν����ƽ��ֵ��Ϊת�����
#define ADC_AVG_SAMPLE4			2
#define ADC_AVG_SAMPLE8			3

#define ADC_TRIGGER_NO			0
#define ADC_TRIGGER_SW			1	//��������
#define ADC_TRIGGER_TIMER0		2
#define ADC_TRIGGER_TIMER1		3
#define ADC_TRIGGER_TIMER2		4
#define ADC_TRIGGER_EXTRIG0		6
#define ADC_TRIGGER_EXTRIG1		7
#define ADC_TRIGGER_PWM0		10
#define ADC_TRIGGER_PWM1		11


/* Interrupt Type */
#define ADC_IT_EOC			(1 << 0)	//End Of Conversion
#define ADC_IT_CMP_MAX		(1 << 1)	//ת���������COMP.MAX
#define ADC_IT_CMP_MIN		(1 << 2)	//ת�����С��COMP.MIN


void ADC_Init(ADC_TypeDef * ADCx, ADC_InitStructure * initStruct);		//ADCģ��ת������ʼ��
void ADC_SEQ_Init(ADC_TypeDef * ADCx, uint32_t seq, ADC_SEQ_InitStructure * initStruct);	//ADC���г�ʼ��
void ADC_CMP_Init(ADC_TypeDef * ADCx, uint32_t seq, ADC_CMP_InitStructure * initStruct);	//ADC�ȽϹ��ܳ�ʼ��
void ADC_Open(ADC_TypeDef * ADCx);							//ADC����������������������Ӳ������ADCת��
void ADC_Close(ADC_TypeDef * ADCx);							//ADC�رգ��޷�������������Ӳ������ADCת��
void ADC_Start(uint32_t ADC0_seq, uint32_t ADC1_seq);		//����ָ��ADC����ʼģ��ת��
void ADC_Stop(uint32_t ADC0_seq, uint32_t ADC1_seq);		//�ر�ָ��ADC��ֹͣģ��ת��

uint32_t ADC_Read(ADC_TypeDef * ADCx, uint32_t chn);		//��ָ��ͨ����ȡת�����
uint32_t ADC_DataAvailable(ADC_TypeDef * ADCx, uint32_t chn);			//ָ��ͨ���Ƿ������ݿɶ�ȡ


void ADC_INTEn(ADC_TypeDef * ADCx, uint32_t seq, uint32_t it);
void ADC_INTEn(ADC_TypeDef * ADCx, uint32_t seq, uint32_t it);
void ADC_INTClr(ADC_TypeDef * ADCx, uint32_t seq, uint32_t it);
uint32_t ADC_INTStat(ADC_TypeDef * ADCx, uint32_t seq, uint32_t it);


#endif //__SWM221_ADC_H__