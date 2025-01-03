#include "SWM221.h"


/* 注意：EnterSleepMode() 必须在RAM中执行，Keil下实现方法有：
   方法一、Scatter file
   方法二、code_in_ra.c上右键 =》Options for File "SWM221_sleep.c" =》Properties =》Memory Assignment =》Code/Conts 选择 IRAM1
*/

void EnterSleepMode(void) 
{ 
		volatile int i;
	
		SYS->SLEEP |= (1 << SYS_SLEEP_SLEEP_Pos);
			
		while((SYS->PAWKSR & (1 << PIN4)) == 0)
		{
				__NOP();
		}
		SYS->PAWKSR |= (1 << PIN4);							//清除唤醒状态

		for(i=0;i<1000;i++)//等待FLASH唤醒,至少需要20uS.
		{
			__NOP();
		}
}  
