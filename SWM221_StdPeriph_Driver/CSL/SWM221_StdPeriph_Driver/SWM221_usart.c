/****************************************************************************************************************************************** 
* 文件名称:	SWM221_usart.c
* 功能说明:	SWM221单片机的USART串口功能驱动库
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
#include "SWM221_usart.h"


/****************************************************************************************************************************************** 
* 函数名称:	USART_Init()
* 功能说明:	USART串口初始化
* 输    入: USART_TypeDef * USARTx	指定要被设置的UART串口，有效值包括USART0
*			USART_InitStructure * initStruct    包含USART串口相关设定值的结构体
* 输    出: 无
* 注意事项: 无
******************************************************************************************************************************************/
void USART_Init(USART_TypeDef * USARTx, USART_InitStructure * initStruct)
{
	switch((uint32_t)USARTx)
	{
	case ((uint32_t)USART0):
		SYS->CLKEN0 |= (0x01 << SYS_CLKEN0_USART0_Pos);
		break;
	}
	
	USART_Close(USARTx);	//一些关键寄存器只能在串口关闭时设置
	
	USARTx->MR = (0 << USART_MR_MODE_Pos)  |
				 (0 << USART_MR_CLKS_Pos)  |
				 (1 << USART_MR_OVER8_Pos) |
				 (initStruct->DataBits << USART_MR_NBDATA_Pos) |
				 (initStruct->Parity   << USART_MR_PARITY_Pos) |
				 (initStruct->StopBits << USART_MR_NBSTOP_Pos);
	
	USARTx->BAUD = ((SystemCoreClock/initStruct->Baudrate / 8) << USART_BAUD_IDIV_Pos) |
				   ((SystemCoreClock/initStruct->Baudrate % 8) << USART_BAUD_FDIV_Pos);
	
	USARTx->RXTO = initStruct->TimeoutTime;
	
	USARTx->IER = (initStruct->RXReadyIEn << USART_IER_RXRDY_Pos) |
				  (initStruct->TXReadyIEn << USART_IER_TXRDY_Pos) |
				  (initStruct->TimeoutIEn << USART_IER_RXTO_Pos);
	
	if(initStruct->RXReadyIEn | initStruct->TXReadyIEn | initStruct->TimeoutIEn)
	{
		switch((uint32_t)USARTx)
		{
		case ((uint32_t)USART0): NVIC_EnableIRQ(USART0_IRQn); break;
		}
	}
}


/****************************************************************************************************************************************** 
* 函数名称:	USART_Open()
* 功能说明:	USART串口打开
* 输    入: USART_TypeDef * USARTx	指定要被设置的USART串口，有效值包括USART0
* 输    出: 无
* 注意事项: 无
******************************************************************************************************************************************/
void USART_Open(USART_TypeDef * USARTx)
{
	USARTx->CR = USART_CR_RXEN_Msk |
				 USART_CR_TXEN_Msk;
}


/****************************************************************************************************************************************** 
* 函数名称:	USART_Close()
* 功能说明:	USART串口关闭
* 输    入: USART_TypeDef * USARTx	指定要被设置的USART串口，有效值包括USART0
* 输    出: 无
* 注意事项: 无
******************************************************************************************************************************************/
void USART_Close(USART_TypeDef * USARTx)
{
	USARTx->CR = USART_CR_RXDIS_Msk |
				 USART_CR_TXDIS_Msk |
				 USART_CR_RSTRX_Msk |
				 USART_CR_RSTTX_Msk |
				 USART_CR_RSTSTA_Msk;
}


/****************************************************************************************************************************************** 
* 函数名称:	USART_SetBaudrate()
* 功能说明:	设置波特率
* 输    入: USART_TypeDef * USARTx	指定要被设置的USART串口，有效值包括USART0
*			uint32_t baudrate		要设置的波特率
* 输    出: 无
* 注意事项: 不要在串口工作时更改波特率，使用此函数前请先调用USART_Close()关闭串口
******************************************************************************************************************************************/
void USART_SetBaudrate(USART_TypeDef * USARTx, uint32_t baudrate)
{	
	USARTx->BAUD = ((SystemCoreClock/baudrate / 8) << USART_BAUD_IDIV_Pos) |
				   ((SystemCoreClock/baudrate % 8) << USART_BAUD_FDIV_Pos);
}


/****************************************************************************************************************************************** 
* 函数名称:	USART_GetBaudrate()
* 功能说明:	查询波特率
* 输    入: USART_TypeDef * USARTx	指定要被设置的USART串口，有效值包括USART0
* 输    出: uint32_t				当前波特率
* 注意事项: 无
******************************************************************************************************************************************/
uint32_t USART_GetBaudrate(USART_TypeDef * USARTx)
{	
	return SystemCoreClock/(((USARTx->BAUD & USART_BAUD_IDIV_Msk) >> USART_BAUD_IDIV_Pos) * 8 +
							((USARTx->BAUD & USART_BAUD_FDIV_Msk) >> USART_BAUD_FDIV_Pos));
}


/****************************************************************************************************************************************** 
* 函数名称:	USART_LINConfig()
* 功能说明:	USART LIN 模式配置
* 输    入: USART_TypeDef * USARTx	指定要被设置的USART串口，有效值包括USART0
*			uint32_t mode			LIN 模式选择，可取值有 USART_LIN_MASTER、USART_LIN_SLAVE
*			uint32_t checksum		校验和类型，可取值 USART_CHECKSUM_LIN13、USART_CHECKSUM_LIN20
*			uint32_t it				interrupt type，有效值有 USART_IT_LIN_ID、USART_IT_LIN_DONE 及其“或”
* 输    出: 无
* 注意事项: 无
******************************************************************************************************************************************/
void USART_LINConfig(USART_TypeDef * USARTx, uint32_t mode, uint32_t checksum, uint32_t it)
{
	USARTx->MR &= ~USART_MR_MODE_Msk;
	USARTx->MR |= (mode << USART_MR_MODE_Pos);
	
	USARTx->LINMR = (USART_LIN_IGNORE << USART_LINMR_NACT_Pos)   |
					(0				  << USART_LINMR_PARDIS_Pos) |
					(0				  << USART_LINMR_CHKDIS_Pos) |
					(checksum		  << USART_LINMR_CHKTYP_Pos) |
					(0				  << USART_LINMR_RDLMOD_Pos) |
					(0				  << USART_LINMR_FSMDIS_Pos) |
					(0				  << USART_LINMR_SYNCDIS_Pos);
	
	USART_INTEn(USARTx, it);
}


/****************************************************************************************************************************************** 
* 函数名称:	USART_LINStart()
* 功能说明:	USART LIN Master 启动传输
* 输    入: USART_TypeDef * USARTx	指定要被设置的USART串口，有效值包括USART0
*			uint32_t slave_id		从机 ID
*			uint32_t action			响应阶段的主机动作，可取值有 USART_LIN_PUBLISH、USART_LIN_SUBSCRIBE、USART_LIN_IGNORE
*			uint32_t count			响应阶段数据个数
*			uint32_t checksum		校验和类型，可取值 USART_CHECKSUM_LIN13、USART_CHECKSUM_LIN20
* 输    出: 无
* 注意事项: 无
******************************************************************************************************************************************/
void USART_LINStart(USART_TypeDef * USARTx, uint32_t slave_id, uint32_t action, uint32_t count, uint32_t checksum)
{
	USARTx->LINMR &= ~(USART_LINMR_NACT_Msk | USART_LINMR_DLC_Msk | USART_LINMR_CHKTYP_Msk);
	USARTx->LINMR |= (action 		<< USART_LINMR_NACT_Pos) |
					 ((count - 1)	<< USART_LINMR_DLC_Pos)  |
					 (checksum 		<< USART_LINMR_CHKTYP_Pos);
	
	USARTx->LINID = slave_id;
}


/****************************************************************************************************************************************** 
* 函数名称:	USART_LINResponse()
* 功能说明:	USART LIN Slave 响应设置
* 输    入: USART_TypeDef * USARTx	指定要被设置的USART串口，有效值包括USART0
*			uint32_t action			响应阶段的从机动作，可取值有 USART_LIN_PUBLISH、USART_LIN_SUBSCRIBE、USART_LIN_IGNORE
*			uint32_t count			响应阶段数据个数
*			uint32_t checksum		校验和类型，可取值 USART_CHECKSUM_LIN13、USART_CHECKSUM_LIN20
* 输    出: 无
* 注意事项: 无
******************************************************************************************************************************************/
void USART_LINResponse(USART_TypeDef * USARTx, uint32_t action, uint32_t count, uint32_t checksum)
{
	USARTx->LINMR &= ~(USART_LINMR_NACT_Msk | USART_LINMR_DLC_Msk | USART_LINMR_CHKTYP_Msk);
	USARTx->LINMR |= (action 		<< USART_LINMR_NACT_Pos) |
					 ((count - 1)	<< USART_LINMR_DLC_Pos)  |
					 (checksum 		<< USART_LINMR_CHKTYP_Pos);
}
