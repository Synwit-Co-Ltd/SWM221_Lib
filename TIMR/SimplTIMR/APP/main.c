#include "SWM221.h"


int main(void)
{	
	SystemInit();
	
	GPIO_Init(GPIOA, PIN4, 1, 0, 0, 0);			//输出，接LED
	GPIO_Init(GPIOA, PIN5, 1, 0, 0, 0);			//输出，接LED
	
	TIMR_Init(TIMR0, TIMR_MODE_TIMER, CyclesPerUs, 500000, 1);		//每0.5秒钟触发一次中断
	TIMR_Start(TIMR0);
	
	TIMR_Init(BTIMR0, TIMR_MODE_TIMER, CyclesPerUs, 500000, 1);		//每0.5秒钟触发一次中断
	TIMR_Start(BTIMR0);
	
	while(1==1)
	{
	}
}

void TIMR0_Handler(void)
{
	TIMR_INTClr(TIMR0);
	
	GPIO_InvBit(GPIOA, PIN4);	//反转LED亮灭状态
}

void BTIMR0_Handler(void)
{	
	TIMR_INTClr(BTIMR0);
	
	GPIO_InvBit(GPIOA, PIN5);	//反转LED亮灭状态
}
