#include "SWM221.h"


int main(void)
{	
	SystemInit();
		
	GPIO_Init(GPIOA, PIN5, 1, 0, 0, 0);			//输出， 接LED
	
	SYS->BODCR = (0 << SYS_BODCR_IE_Pos) |
				 (2 << SYS_BODCR_RSTLVL_Pos) |	//2.1V复位
				 (4 << SYS_BODCR_INTLVL_Pos);	//2.7V触发中断
	
	while(1==1)
	{
		while(SYS->BODSR & SYS_BODSR_ST_Msk)
		{
			GPIO_SetBit(GPIOA, PIN5);
		}
		
		GPIO_ClrBit(GPIOA, PIN5);
	}
}

