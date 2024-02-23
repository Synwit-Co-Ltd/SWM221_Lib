#include "SWM221.h"


int main(void)
{	
	SystemInit();
	
	PORT_Init(PORTA, PIN14, PORTA_PIN14_TIMR0_OUT, 0);
	
	TIMR_Init(TIMR0, TIMR_MODE_OC, CyclesPerUs, 10000, 0);	//100Hz
	
	TIMR_OC_Init(TIMR0, 7500, 0, 1);						//7500/10000 = 75%
	
	TIMR_Start(TIMR0);
	
	PORT_Init(PORTA, PIN6, PORTA_PIN6_BTIMR0_OUT, 0);
	
	TIMR_Init(BTIMR0, TIMR_MODE_OC, CyclesPerUs, 10000, 0);
	
	TIMR_OC_Init(BTIMR0, 7500, 0, 1);
	
	TIMR_Start(BTIMR0);
	
	while(1==1)
	{
	}
}
