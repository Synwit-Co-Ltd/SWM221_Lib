/*******************************************************************************************************************************
* @brief	Hardware divider driver
*
*
********************************************************************************************************************************
* @attention
*
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS WITH CODING INFORMATION 
* REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME. AS A RESULT, SYNWIT SHALL NOT BE HELD LIABLE 
* FOR ANY DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE CONTENT 
* OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING INFORMATION CONTAINED HEREIN IN CONN-
* -ECTION WITH THEIR PRODUCTS.
*
* COPYRIGHT 2012 Synwit Technology 
*******************************************************************************************************************************/
#include "SWM221.h"
#include "SWM221_div.h"


/****************************************************************************************************************************************** 
* 函数名称:	DIV_Init()
* 功能说明:	硬件除法器初始化
* 输    入: DIV_TypeDef * DIVx	指定要被设置的硬件除法器，有效值包括DIV
* 输    出: 无
* 注意事项: 无
******************************************************************************************************************************************/
void DIV_Init(DIV_TypeDef * DIVx)
{
	switch((uint32_t)DIVx)
	{
	case ((uint32_t)DIV):
		SYS->CLKEN0 |= (0x01 << SYS_CLKEN0_DIV_Pos);
		break;
	}
}
