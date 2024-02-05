/****************************************************************************************************************************************** 
* 文件名称: SWM342_dma.c
* 功能说明:	SWM342单片机的DMA功能驱动库
* 技术支持:	http://www.synwit.com.cn/e/tool/gbook/?bid=1
* 注意事项: 
* 版本日期:	V1.0.0		2016年1月30日
* 升级记录:  
*
*
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
#include "SWM221_dma.h"


/****************************************************************************************************************************************** 
* 函数名称: DMA_CH_Init()
* 功能说明:	DMA通道初始化
* 输    入: uint32_t chn			指定要配置的通道，有效值有DMA_CH0、DMA_CH1
*			DMA_InitStructure * initStruct    包含DMA通道相关设定值的结构体
* 输    出: 无
* 注意事项: 无
******************************************************************************************************************************************/
void DMA_CH_Init(uint32_t chn, DMA_InitStructure * initStruct)
{	
	DMA_CH_Close(chn);		//关闭后配置
	
	DMA->CH[chn].CR = (initStruct->Mode 			 << DMA_CR_CIRC_Pos) |
					  (initStruct->Unit				 << DMA_CR_MSIZ_Pos) |
					  (initStruct->Unit				 << DMA_CR_PSIZ_Pos) |
					  (initStruct->MemoryAddrInc 	 << DMA_CR_MINC_Pos) |
					  (initStruct->PeripheralAddrInc << DMA_CR_PINC_Pos) |
					  (initStruct->Priority			 << DMA_CR_PL_Pos);
	
	DMA->CH[chn].NDT = ( initStruct->Count 		<< DMA_NDT_LEN_Pos) |
					   ((initStruct->Count / 2) << DMA_NDT_HALF_Pos);
	
	DMA->CH[chn].MAR = initStruct->MemoryAddr;
	DMA->CH[chn].PAR = initStruct->PeripheralAddr;
	
	switch(initStruct->Handshake & DMA_HS_MSK)
	{
	case DMA_HS_NO:
		DMA->CH[chn].MUX = 0;
		DMA->CH[chn].CR |= DMA_CR_DIR_Msk | DMA_CR_MEM2MEM_Msk;
		break;
	
	case DMA_HS_MRD:
		DMA->CH[chn].MUX = ((initStruct->Handshake & 0xF) << DMA_MUX_MRDHSSIG_Pos) | (1 << DMA_MUX_MRDHSEN_Pos);
		DMA->CH[chn].CR |= DMA_CR_DIR_Msk;
		break;
	
	case DMA_HS_MWR:
		DMA->CH[chn].MUX = ((initStruct->Handshake & 0xF) << DMA_MUX_MWRHSSIG_Pos) | (1 << DMA_MUX_MWRHSEN_Pos);
		DMA->CH[chn].CR &= ~DMA_CR_DIR_Msk;
		break;
	
	case DMA_HS_EXT | DMA_HS_MRD:
		DMA->CH[chn].MUX = ((initStruct->Handshake & 0xF) << DMA_MUX_EXTHSSIG_Pos) | (1 << DMA_MUX_EXTHSEN_Pos);
		DMA->CH[chn].CR |= DMA_CR_DIR_Msk;
		break;
	
	case DMA_HS_EXT | DMA_HS_MWR:
		DMA->CH[chn].MUX = ((initStruct->Handshake & 0xF) << DMA_MUX_EXTHSSIG_Pos) | (1 << DMA_MUX_EXTHSEN_Pos);
		DMA->CH[chn].CR &= ~DMA_CR_DIR_Msk;
		break;
	}

	DMA_CH_INTClr(chn, initStruct->INTEn);
	DMA_CH_INTEn(chn, initStruct->INTEn);
	
	if(initStruct->INTEn) NVIC_EnableIRQ(GPIOB1_GPIOA9_DMA_IRQn);
}

/****************************************************************************************************************************************** 
* 函数名称: DMA_CH_Open()
* 功能说明:	DMA通道开启，对于软件启动通道，开启后立即传输；对于硬件触发通道，开启后还需等出现触发信号后才开始搬运
* 输    入: uint32_t chn			指定要配置的通道，有效值有DMA_CH0、DMA_CH1
* 输    出: 无
* 注意事项: 无
******************************************************************************************************************************************/
void DMA_CH_Open(uint32_t chn)
{
	DMA->CH[chn].CR |= DMA_CR_EN_Msk;
}

/****************************************************************************************************************************************** 
* 函数名称: DMA_CH_Close()
* 功能说明:	DMA通道关闭
* 输    入: uint32_t chn			指定要配置的通道，有效值有DMA_CH0、DMA_CH1
* 输    出: 无
* 注意事项: 无
******************************************************************************************************************************************/
void DMA_CH_Close(uint32_t chn)
{
	DMA->CH[chn].CR &= ~DMA_CR_EN_Msk;
}


/****************************************************************************************************************************************** 
* 函数名称: DMA_CH_INTEn()
* 功能说明:	DMA中断使能
* 输    入: uint32_t chn			指定要配置的通道，有效值有DMA_CH0、DMA_CH1
*			uint32_t it				interrupt type，有效值有 DMA_IT_DONE、DMA_IT_HALF、DMA_IT_ERROR
* 输    出: 无
* 注意事项: 无
******************************************************************************************************************************************/
void DMA_CH_INTEn(uint32_t chn, uint32_t it)
{
	DMA->CH[chn].CR |= it;
}

/****************************************************************************************************************************************** 
* 函数名称: DMA_CH_INTDis()
* 功能说明:	DMA中断禁止
* 输    入: uint32_t chn			指定要配置的通道，有效值有DMA_CH0、DMA_CH1
*			uint32_t it				interrupt type，有效值有 DMA_IT_DONE、DMA_IT_HALF、DMA_IT_ERROR
* 输    出: 无
* 注意事项: 无
******************************************************************************************************************************************/
void DMA_CH_INTDis(uint32_t chn, uint32_t it)
{
	DMA->CH[chn].CR &= ~it;
}

/****************************************************************************************************************************************** 
* 函数名称: DMA_CH_INTClr()
* 功能说明:	DMA中断标志清除
* 输    入: uint32_t chn			指定要配置的通道，有效值有DMA_CH0、DMA_CH1
*			uint32_t it				interrupt type，有效值有 DMA_IT_DONE、DMA_IT_HALF、DMA_IT_ERROR
* 输    出: 无
* 注意事项: 无
******************************************************************************************************************************************/
void DMA_CH_INTClr(uint32_t chn, uint32_t it)
{
	DMA->IFC = (it << (chn * 4));
}

/****************************************************************************************************************************************** 
* 函数名称: DMA_CH_INTStat()
* 功能说明:	DMA中断状态查询
* 输    入: uint32_t chn			指定要配置的通道，有效值有DMA_CH0、DMA_CH1
*			uint32_t it				interrupt type，有效值有 DMA_IT_DONE、DMA_IT_HALF、DMA_IT_ERROR
* 输    出: uint32_t				 0 指定中断未发生    非0 指定中断已发生   
* 注意事项: 无
******************************************************************************************************************************************/
uint32_t DMA_CH_INTStat(uint32_t chn, uint32_t it)
{
	return DMA->IF & (it << (chn * 4));
}
