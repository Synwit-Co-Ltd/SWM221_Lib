#ifndef __SWM221_ADC_H__
#define	__SWM221_ADC_H__

typedef struct {
	uint8_t  clk_src;		//ADC转换时钟源：ADC_CLKSRC_HRC_DIV32、ADC_CLKSRC_XTAL_DIV32、...
	uint8_t  clk_div;		//ADC转换时钟分频，取值1--31
	uint8_t  ref_src;		//ADC转换参考源：ADC_REFSRC_VREFP、ADC_REFSRC_VDD
	uint16_t channels;		//ADC转换通道选中，ADC_CH0、ADC_CH1、... ... 、ADC_CH11及其组合（即“按位或”运算）
	uint8_t  samplAvg;		//采样取平均，触发启动ADC转换后，ADC在一个通道上连续采样、转换多次，并将它们的平均值作为该通道转换结果
	uint16_t trig_src;		//ADC触发方式：ADC_TRIGGER_SW、ADC_TRIGGER_TIMR2、ADC_TRIGGER_TIMR3、ADC_TRIGGER_PWM0A、...
	uint8_t  Continue;		//在软件触发模式下：1 连续转换模式，启动后一直采样、转换，直到软件清除START位
							//                  0 单次转换模式，转换完成后START位自动清除停止转换
	uint16_t EOC_IEn;		//EOC中断使能，可针对每个通道设置，其有效值为ADC_CH0、ADC_CH1、... ... 、ADC_CH11及其组合（即“按位或”运算）
	uint16_t OVF_IEn;		//OVF中断使能，可针对每个通道设置，其有效值为ADC_CH0、ADC_CH1、... ... 、ADC_CH11及其组合（即“按位或”运算）
} ADC_InitStructure;

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


/*  clk_src（SYS->CLKSEL.ADC）:  0 HRC   1 XTAL   2 PLL
	sys_div（SYS->CLKSEL.ADC）:  0 1分频   2 4分频   3 8分频
	adc_div0（ADC->CTRL4.CLKDIV0）: 0 4分频   1 2分频   2 1分频
	adc_div1（ADC->CTRL3.CLKDIV1）: 0 4分频   1 2分频   2 1分频
*/
#define ADC_CLKSRC(clk_src, sys_div, adc_div0, adc_div1) \
			(clk_src | (sys_div << 2) | (adc_div0 << 4) | (adc_div1 << 6))

#define ADC_CLKSRC_HRC			ADC_CLKSRC(0, 0, 2, 2)
#define ADC_CLKSRC_XTAL			ADC_CLKSRC(1, 0, 2, 2)
#define ADC_CLKSRC_PLL			ADC_CLKSRC(2, 0, 2, 2)
#define ADC_CLKSRC_HRC_DIV2		ADC_CLKSRC(0, 0, 1, 2)
#define ADC_CLKSRC_XTAL_DIV2	ADC_CLKSRC(1, 0, 1, 2)
#define ADC_CLKSRC_PLL_DIV2		ADC_CLKSRC(2, 0, 1, 2)
#define ADC_CLKSRC_HRC_DIV4		ADC_CLKSRC(0, 0, 1, 1)
#define ADC_CLKSRC_XTAL_DIV4	ADC_CLKSRC(1, 0, 1, 1)
#define ADC_CLKSRC_PLL_DIV4		ADC_CLKSRC(2, 0, 1, 1)
#define ADC_CLKSRC_HRC_DIV8		ADC_CLKSRC(0, 0, 0, 1)
#define ADC_CLKSRC_XTAL_DIV8	ADC_CLKSRC(1, 0, 0, 1)
#define ADC_CLKSRC_PLL_DIV8		ADC_CLKSRC(2, 0, 0, 1)
#define ADC_CLKSRC_HRC_DIV16	ADC_CLKSRC(0, 0, 0, 0)
#define ADC_CLKSRC_XTAL_DIV16	ADC_CLKSRC(1, 0, 0, 0)
#define ADC_CLKSRC_PLL_DIV16	ADC_CLKSRC(2, 0, 0, 0)


#define ADC_REFSRC_VREFP3V3		((7 << 4) | 0)	//芯片 3.3V 供电时：Vrefp 引脚，只能用于 F 版芯片
#define ADC_REFSRC_VDD3V3		((7 << 4) | 1)	//芯片 3.3V 供电时：芯片VDD，只能用于 F 版芯片
#define ADC_REFSRC_VREFP5V		((3 << 4) | 0)	//芯片 5V   供电时：Vrefp 引脚
#define ADC_REFSRC_VDD5V		((3 << 4) | 1)	//芯片 5V   供电时：芯片VDD
#define ADC_REFSRC_3V6			((0 << 4) | 7)	//芯片 5V   供电时：内部3.6V

#define ADC_AVG_SAMPLE1			0	
#define ADC_AVG_SAMPLE2			1	//一次启动连续采样、转换2次，并计算两次结果的平均值作为转换结果
#define ADC_AVG_SAMPLE4			3
#define ADC_AVG_SAMPLE8			7
#define ADC_AVG_SAMPLE16		15


#define ADC_TRIGGER_SW			0	//软件触发，即ADC->START.GO写1启动转换
#define ADC_TRIGGER_TIMR0		2
#define ADC_TRIGGER_TIMR1		3
#define ADC_TRIGGER_PWM0		0x1001
#define ADC_TRIGGER_PWM1		0x1004



void ADC_Init(ADC_TypeDef * ADCx, ADC_InitStructure * initStruct);		//ADC模数转换器初始化
uint32_t ADC_Calibrate(ADC_TypeDef * ADCx);
void ADC_Open(ADC_TypeDef * ADCx);							//ADC开启，可以软件启动、或硬件触发ADC转换
void ADC_Close(ADC_TypeDef * ADCx);							//ADC关闭，无法软件启动、或硬件触发ADC转换
void ADC_Start(ADC_TypeDef * ADCx);							//启动指定ADC，开始模数转换
void ADC_Stop(ADC_TypeDef * ADCx);							//关闭指定ADC，停止模数转换

uint32_t ADC_Read(ADC_TypeDef * ADCx, uint32_t chn);		//从指定通道读取转换结果
uint32_t ADC_IsEOC(ADC_TypeDef * ADCx, uint32_t chn);		//指定通道是否End Of Conversion

void ADC_ChnSelect(ADC_TypeDef * ADCx, uint32_t chns);

void ADC_IntEOCEn(ADC_TypeDef * ADCx, uint32_t chn);		//转换完成中断使能
void ADC_IntEOCDis(ADC_TypeDef * ADCx, uint32_t chn);		//转换完成中断禁止
void ADC_IntEOCClr(ADC_TypeDef * ADCx, uint32_t chn);		//转换完成中断标志清除
uint32_t ADC_IntEOCStat(ADC_TypeDef * ADCx, uint32_t chn);	//转换完成中断状态

void ADC_IntOVFEn(ADC_TypeDef * ADCx, uint32_t chn);		//数据溢出中断使能
void ADC_IntOVFDis(ADC_TypeDef * ADCx, uint32_t chn);		//数据溢出中断禁止
void ADC_IntOVFClr(ADC_TypeDef * ADCx, uint32_t chn);		//数据溢出中断标志清除
uint32_t ADC_IntOVFStat(ADC_TypeDef * ADCx, uint32_t chn);	//数据溢出中断状态


#endif //__SWM221_ADC_H__
