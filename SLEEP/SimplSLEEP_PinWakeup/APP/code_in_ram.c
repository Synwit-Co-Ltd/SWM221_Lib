#include "SWM221.h"


/* 注意：EnterSleepMode() 必须在RAM中执行，Keil下实现方法有：
   方法一、Scatter file
   方法二、code_in_ram.c 上右键 =》Options for File "code_in_ram.c" =》Properties =》Memory Assignment =》Code/Conts 选择 IRAM1
*/

void EnterSleepMode(void) 
{ 
		volatile int i;
	
		SYS->SLEEP |= (1 << SYS_SLEEP_SLEEP_Pos);
		
		GPIOA->DATAPIN5 = 0; //示波器测试用

		while((SYS->PAWKSR & (1 << PIN4)) == 0)
		{
				__NOP();
		}
		SYS->PAWKSR |= (1 << PIN4);							//清除唤醒状态

		for(i=0;i<16;i++)//30uS延时等待FLASH唤醒;至少需要20uS.
		{
			__NOP();
		}
    GPIOA->DATAPIN5 = 1;  //示波器测试用
}  
