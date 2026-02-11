#include "SWM221.h"


#define USE_GPIOA2_IRQ   1


int main(void)
{	
	SystemInit();
	
	GPIO_INIT(GPIOA, PIN5, GPIO_OUTPUT);		//输出，接LED
	
	GPIO_INIT(GPIOA, PIN2, GPIO_INPUT_PullUp);	//输入，上拉使能，接KEY
	
	EXTI_Init(GPIOA, PIN2, EXTI_FALL_EDGE);		//下降沿触发中断
	
#if USE_GPIOA2_IRQ
	NVIC_EnableIRQ(GPIOA2_GPIOC2_MPU_IRQn);
#else
	NVIC_EnableIRQ(GPIOA_IRQn);
#endif
	
	EXTI_Open(GPIOA, PIN2);
	
	while(1==1)
	{
	}
}


#if USE_GPIOA2_IRQ
void GPIOA2_GPIOC2_MPU_Handler(void)
#else
void GPIOA_Handler(void)
#endif
{
	if(EXTI_State(GPIOA, PIN2))
	{
		EXTI_Clear(GPIOA, PIN2);
		
		GPIO_InvBit(GPIOA, PIN5);
	}
}
