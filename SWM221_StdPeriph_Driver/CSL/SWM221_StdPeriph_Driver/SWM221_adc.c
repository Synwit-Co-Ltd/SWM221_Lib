/****************************************************************************************************************************************** 
* 文件名称:	SWM341_adc.c
* 功能说明:	SWM341单片机的ADC数模转换器功能驱动库
* 技术支持:	http://www.synwit.com.cn/e/tool/gbook/?bid=1
* 注意事项:
* 版本日期: V1.0.0		2016年1月30日
* 升级记录: 
*******************************************************************************************************************************************
* @attention
*
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS WITH CODING INFORMATION 
* REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME. AS A RESULT, SYNWIT SHALL NOT BE HELD LIABLE 
* FOR ANY DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE CONTENT 
* OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING INFORMATION CONTAINED HEREIN IN CONN-
* -ECTION WITH THEIR PRODUCTS.
*
* COPYRIGHT 2012 Synwit Technology  
*******************************************************************************************************************************************/
#include "SWM221.h"
#include "SWM221_adc.h"


/****************************************************************************************************************************************** 
* 函数名称: ADC_Init()
* 功能说明:	ADC模数转换器初始化
* 输    入: ADC_TypeDef * ADCx		指定要设置的ADC，有效值包括ADC0、ADC1
*			ADC_InitStructure * initStruct		包含ADC各相关定值的结构体
* 输    出: 无
* 注意事项: 无
******************************************************************************************************************************************/
void ADC_Init(ADC_TypeDef * ADCx, ADC_InitStructure * initStruct)
{
	switch((uint32_t)ADCx)
	{
	case ((uint32_t)ADC0):
	case ((uint32_t)ADC1):
		SYS->CLKEN0 |= (0x01 << SYS_CLKEN0_ADC0_Pos);
		break;
	}
	
	ADC_Close(ADCx);		//一些关键寄存器只能在ADC关闭时设置
	
	ADCx->CR &= ~(ADC_CR_CLKDIV_Msk | ADC_CR_AVG_Msk | ADC_CR_BITS_Pos);
	ADCx->CR |= ((initStruct->clkdiv - 1) << ADC_CR_CLKDIV_Pos) |
				(initStruct->samplAvg	  << ADC_CR_AVG_Pos)    |
				(0					 	  << ADC_CR_BITS_Pos);
}

static uint32_t ADC_seq2pos(uint32_t seq)
{
	uint32_t pos = 0;
	
	switch(seq)
	{
	case ADC_SEQ0: pos = 0;  break;
	case ADC_SEQ1: pos = 8;  break;
	}
	
	return pos;
}

static uint32_t ADC_chn2idx(uint32_t chn)
{
	uint32_t idx = 0;
	
	switch(chn)
	{
	case ADC_CH0:  idx = 0;  break;
	case ADC_CH1:  idx = 1;  break;
	case ADC_CH2:  idx = 2;  break;
	case ADC_CH3:  idx = 3;  break;
	case ADC_CH4:  idx = 4;  break;
	case ADC_CH5:  idx = 5;  break;
	case ADC_CH6:  idx = 6;  break;
	case ADC_CH7:  idx = 7;  break;
	case ADC_CH8:  idx = 8;  break;
	case ADC_CH9:  idx = 9;  break;
	}
	
	return idx;
}

/****************************************************************************************************************************************** 
* 函数名称: ADC_SEQ_Init()
* 功能说明:	ADC序列初始化
* 输    入: ADC_TypeDef * ADCx		指定要设置的ADC，有效值包括ADC0、ADC1
*			uint32_t seq 			指定要设置的序列，有效值包括ADC_SEQ0、ADC_SEQ1
*			ADC_SEQ_InitStructure * initStruct		包含ADC序列设定定值的结构体
* 输    出: 无
* 注意事项: 无
******************************************************************************************************************************************/
void ADC_SEQ_Init(ADC_TypeDef * ADCx, uint32_t seq, ADC_SEQ_InitStructure * initStruct)
{
	uint32_t pos = ADC_seq2pos(seq);
	
	ADCx->SEQTRG &= ~(0xFFu << pos);
	ADCx->SEQTRG |= (initStruct->trig_src << pos);
	
	ADCx->SMPTIM &= ~(0xFFu << pos);
	ADCx->SMPTIM |= ((initStruct->samp_tim - 4) << pos);
	
	ADCx->SMPNUM &= ~(0xFFu << pos);
	ADCx->SMPNUM |= ((initStruct->conv_cnt - 1) << pos);
	
	ADCx->IE |= (initStruct->EOCIntEn << pos);
	
	if(initStruct->EOCIntEn)
		NVIC_EnableIRQ(ADC_IRQn);
	
	__IO uint32_t * SEQxCHN = (seq == ADC_SEQ0) ? &ADCx->SEQ0CHN : &ADCx->SEQ1CHN;
	*SEQxCHN = 0;
	for(int i = 0; i < 8; i++)
	{
		if(initStruct->channels[i] == 0)
		{
			*SEQxCHN |= 0xF << (i * 4);
			
			break;
		}
		
		*SEQxCHN |= ADC_chn2idx(initStruct->channels[i]) << (i * 4);
	}
}

/****************************************************************************************************************************************** 
* 函数名称: ADC_CMP_Init()
* 功能说明:	ADC比较功能初始化
* 输    入: ADC_TypeDef * ADCx		指定要设置的ADC，有效值包括ADC0、ADC1
*			uint32_t seq 			指定要设置的序列，有效值包括ADC_SEQ0、ADC_SEQ1
*			ADC_CMP_InitStructure * initStruct		包含ADC比较功能设定定值的结构体
* 输    出: 无
* 注意事项: 无
******************************************************************************************************************************************/
void ADC_CMP_Init(ADC_TypeDef * ADCx, uint32_t seq, ADC_CMP_InitStructure * initStruct)
{
	if(seq == ADC_SEQ0)
		ADCx->SEQ0CHK = (initStruct->UpperLimit << ADC_SEQ0CHK_MAX_Pos) |
						(initStruct->LowerLimit << ADC_SEQ0CHK_MIN_Pos);
	else
		ADCx->SEQ1CHK = (initStruct->UpperLimit << ADC_SEQ1CHK_MAX_Pos) |
						(initStruct->LowerLimit << ADC_SEQ1CHK_MIN_Pos);
	
	if(initStruct->UpperLimitIEn)
		ADC_INTEn(ADCx, seq, ADC_IT_CMP_MAX);
	
	if(initStruct->LowerLimitIEn)
		ADC_INTEn(ADCx, seq, ADC_IT_CMP_MIN);
	
	if(initStruct->UpperLimitIEn || initStruct->LowerLimitIEn)
		NVIC_EnableIRQ(ADC_IRQn);
}

/****************************************************************************************************************************************** 
* 函数名称:	ADC_Open()
* 功能说明:	ADC开启，可以软件启动、或硬件触发ADC转换
* 输    入: ADC_TypeDef * ADCx		指定要设置的ADC，有效值包括ADC0、ADC1
* 输    出: 无
* 注意事项: 无
******************************************************************************************************************************************/
void ADC_Open(ADC_TypeDef * ADCx)
{
	ADCx->CR &= ~ADC_CR_PWDN_Msk;
	
	int clkdiv = ((ADCx->CR & ADC_CR_CLKDIV_Msk) >> ADC_CR_CLKDIV_Pos) + 1;
	
	for(int i = 0; i < clkdiv * 32; i++) {}	// 退出 Powerdown 32 个采样时钟周期后才能工作
}

/****************************************************************************************************************************************** 
* 函数名称:	ADC_Close()
* 功能说明:	ADC关闭，无法软件启动、或硬件触发ADC转换
* 输    入: ADC_TypeDef * ADCx		指定要设置的ADC，有效值包括ADC0、ADC1
* 输    出: 无
* 注意事项: 无
******************************************************************************************************************************************/
void ADC_Close(ADC_TypeDef * ADCx)
{
	ADCx->CR |=  ADC_CR_PWDN_Msk;
}

/****************************************************************************************************************************************** 
* 函数名称:	ADC_Start()
* 功能说明:	软件触发模式下启动ADC转换
* 输    入: uint32_t ADC0_seq		指定要设置的ADC0序列，有效值为ADC_SEQ0、ADC_SEQ1及其组合（即“按位或”运算）
*			uint32_t ADC1_seq		指定要设置的ADC1序列，有效值为ADC_SEQ0、ADC_SEQ1及其组合（即“按位或”运算）
* 输    出: 无
* 注意事项: 无
******************************************************************************************************************************************/
void ADC_Start(uint32_t ADC0_seq, uint32_t ADC1_seq)
{
	ADC0->START |= (ADC0_seq << ADC_START_ADC0SEQ0_Pos) |
				   (ADC1_seq << ADC_START_ADC1SEQ0_Pos);
}

/****************************************************************************************************************************************** 
* 函数名称:	ADC_Stop()
* 功能说明:	软件触发模式下停止ADC转换
* 输    入: uint32_t ADC0_seq		指定要设置的ADC0序列，有效值为ADC_SEQ0、ADC_SEQ1及其组合（即“按位或”运算）
*			uint32_t ADC1_seq		指定要设置的ADC1序列，有效值为ADC_SEQ0、ADC_SEQ1及其组合（即“按位或”运算）
* 输    出: 无
* 注意事项: 无
******************************************************************************************************************************************/
void ADC_Stop(uint32_t ADC0_seq, uint32_t ADC1_seq)
{
	ADC0->START &= ~((ADC0_seq << ADC_START_ADC0SEQ0_Pos) |
					 (ADC1_seq << ADC_START_ADC1SEQ0_Pos));
}

/****************************************************************************************************************************************** 
* 函数名称:	ADC_Busy()
* 功能说明:	ADC 忙查询
* 输    入: ADC_TypeDef * ADCx		指定要设置的ADC，有效值包括ADC0、ADC1
* 输    出: bool					true 正在转换	false 空闲
* 注意事项: 无
******************************************************************************************************************************************/
bool ADC_Busy(ADC_TypeDef * ADCx)
{
	if(ADCx == ADC0)
		return ADC0->START & ADC_START_ADC0BUSY_Msk;
	else
		return ADC0->START & ADC_START_ADC1BUSY_Msk;
}

/****************************************************************************************************************************************** 
* 函数名称:	ADC_Read()
* 功能说明:	读取指定通道的转换结果
* 输    入: ADC_TypeDef * ADCx		指定要设置的ADC，有效值包括ADC0、ADC1
*			uint32_t chn			要读取的通道，有效值为ADC_CH0、ADC_CH1、... ...、ADC_CH10、ADC_CH11
* 输    出: uint32_t				读取到的转换结果
* 注意事项: 无
******************************************************************************************************************************************/
uint32_t ADC_Read(ADC_TypeDef * ADCx, uint32_t chn)
{
	uint32_t idx = ADC_chn2idx(chn);
	
	return ADCx->DATA[idx] & ADC_DATA_DATA_Msk;
}

/****************************************************************************************************************************************** 
* 函数名称:	ADC_DataAvailable()
* 功能说明:	指定序列是否有数据可读取
* 输    入: ADC_TypeDef * ADCx		指定要设置的ADC，有效值包括ADC0、ADC1
*			uint32_t chn			要读取的通道，有效值为ADC_CH0、ADC_CH1、... ...、ADC_CH10、ADC_CH11
* 输    出: uint32_t				1 有数据可读取    0 无数据
* 注意事项: 若在 ADC 转换过程中调用此函数，有一定概率在转换完成时 ADCx->DATA.FLAG 位无法置起
******************************************************************************************************************************************/
uint32_t ADC_DataAvailable(ADC_TypeDef * ADCx, uint32_t chn)
{
	uint32_t idx = ADC_chn2idx(chn);
	
	return (ADCx->DATA[idx] & ADC_DATA_FLAG_Msk) != 0;
}


/****************************************************************************************************************************************** 
* 函数名称:	ADC_INTEn()
* 功能说明:	中断使能
* 输    入: ADC_TypeDef * ADCx		指定要设置的ADC，有效值包括ADC0、ADC1
*			uint32_t seq			指定要设置的ADC序列，有效值为ADC_SEQ0、ADC_SEQ1
* 			uint32_t it				interrupt type，有效值包括ADC_IT_EOC、ADC_IT_CMP_MAX、ADC_IT_CMP_MIN 及其“或”
* 输    出: 无
* 注意事项: 无
******************************************************************************************************************************************/
void ADC_INTEn(ADC_TypeDef * ADCx, uint32_t seq, uint32_t it)
{
	uint32_t pos = ADC_seq2pos(seq);
	
	ADCx->IE |= (it << pos);
}

/****************************************************************************************************************************************** 
* 函数名称:	ADC_INTDis()
* 功能说明:	中断禁止
* 输    入: ADC_TypeDef * ADCx		指定要设置的ADC，有效值包括ADC0、ADC1
*			uint32_t seq			指定要设置的ADC序列，有效值为ADC_SEQ0、ADC_SEQ1
* 			uint32_t it				interrupt type，有效值包括ADC_IT_EOC、ADC_IT_CMP_MAX、ADC_IT_CMP_MIN 及其“或”
* 输    出: 无
* 注意事项: 无
******************************************************************************************************************************************/
void ADC_INTDis(ADC_TypeDef * ADCx, uint32_t seq, uint32_t it)
{
	uint32_t pos = ADC_seq2pos(seq);
	
	ADCx->IE &= ~(it << pos);
}

/****************************************************************************************************************************************** 
* 函数名称:	ADC_INTClr()
* 功能说明:	中断标志清除
* 输    入: ADC_TypeDef * ADCx		指定要设置的ADC，有效值包括ADC0、ADC1
*			uint32_t seq			指定要设置的ADC序列，有效值为ADC_SEQ0、ADC_SEQ1
* 			uint32_t it				interrupt type，有效值包括ADC_IT_EOC、ADC_IT_CMP_MAX、ADC_IT_CMP_MIN 及其“或”
* 输    出: 无
* 注意事项: 无
******************************************************************************************************************************************/
void ADC_INTClr(ADC_TypeDef * ADCx, uint32_t seq, uint32_t it)
{
	uint32_t pos = ADC_seq2pos(seq);
	
	ADCx->IF = (it << pos);
}

/****************************************************************************************************************************************** 
* 函数名称:	ADC_INTStat()
* 功能说明:	中断状态查询
* 输    入: ADC_TypeDef * ADCx		指定要设置的ADC，有效值包括ADC0、ADC1
*			uint32_t seq			指定要查询的ADC序列，有效值为ADC_SEQ0、ADC_SEQ1
* 			uint32_t it				interrupt type，有效值包括ADC_IT_EOC、ADC_IT_CMP_MAX、ADC_IT_CMP_MIN 及其“或”
* 输    出: uint32_t					1 中断发生    0 中断未发生
* 注意事项: 无
******************************************************************************************************************************************/
uint32_t ADC_INTStat(ADC_TypeDef * ADCx, uint32_t seq, uint32_t it)
{
	uint32_t pos = ADC_seq2pos(seq);
	
	return (ADCx->IF & (it << pos)) ? 1 : 0;
}
