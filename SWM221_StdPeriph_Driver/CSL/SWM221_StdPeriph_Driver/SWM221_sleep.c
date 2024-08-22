/****************************************************************************************************************************************** 
* 文件名称:	SWM221_sleep.c
* 功能说明:	SWM221单片机的Sleep功能驱动库
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
#include "SWM221_sleep.h"


/* 注意：EnterSleepMode() 必须在RAM中执行，Keil下实现方法有：
   方法一、Scatter file
   方法二、SWM221_sleep.c上右键 =》Options for File "SWM221_sleep.c" =》Properties =》Memory Assignment =》Code/Conts 选择 IRAM1
*/


#if defined ( __ICCARM__ )
__ramfunc
#endif
void EnterSleepMode(void)
{	
	__NOP();__NOP();__NOP();
	SYS->SLEEP |= (1 << SYS_SLEEP_SLEEP_Pos);
}
