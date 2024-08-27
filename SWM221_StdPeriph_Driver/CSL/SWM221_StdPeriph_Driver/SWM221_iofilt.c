/****************************************************************************************************************************************** 
* 文件名称:	SWM221_iofilt.c
* 功能说明:	SWM221单片机的IO滤波器功能模块，对PAD到模块输入间的信号滤波，窄于指定宽度的脉冲视作毛刺，忽略
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
#include "SWM221_iofilt.h"


/****************************************************************************************************************************************** 
* 函数名称:	IOFILT_Init()
* 功能说明:	IO滤波器初始化
* 输    入: uint32_t IOFILTn	要初始化的滤波器，可取值 0-1
*			uint32_t signal		要对哪些信号进行滤波，当 IOFILTn = 0 时，可取值 IOFILT0_PB14、IOFILT0_PB4、IOFILT0_PB5、IOFILT0_PB6 及其“或”
*													  当 IOFILTn = 1 时，可取值 IOFILT1_ACMP0、IOFILT1_ACMP1 及其“或”
*			uint32_t width		被选信号上宽度小于 width 的脉冲被视作毛刺，过滤掉，可取值 IOFILT_WIDTH_250ns、IOFILT_WIDTH_500ns、...
* 输    出: 无
* 注意事项: 无
******************************************************************************************************************************************/
void IOFILT_Init(uint32_t IOFILTn, uint32_t signal, uint32_t width)
{
	SYS->CLKSEL &= ~SYS_CLKSEL_IOFILT_Msk;
	SYS->CLKSEL |= (0 << SYS_CLKSEL_IOFILT_Pos);	//滤波器时钟源：HRC
	
	SYS->CLKEN0 |= SYS_CLKEN0_IOFILT_Msk;
	for(int i = 0; i < 10; i++)  __NOP();
	
	*(&SYS->IOFILT0 + IOFILTn) = (signal << SYS_IOFILT_IO0EN_Pos)  |
								 (0      << SYS_IOFILT_CLKDIV_Pos) |
								 (width  << SYS_IOFILT_TIM_Pos);
}
