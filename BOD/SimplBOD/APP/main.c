#include "SWM221.h"


int main(void)
{
	SystemInit();
	
	GPIO_Init(GPIOA, PIN5, 1, 0, 0, 0);			//输出， 接LED
	
	SYS->BODCR = (1 << SYS_BODCR_EN_Pos) |
				 (1 << SYS_BODCR_IE_Pos) |
				 (4 << SYS_BODCR_LVL_Pos);		//电源电压低于 3.7v 时产生中断
	NVIC_EnableIRQ(GPIOA3_GPIOC3_BOD_IRQn);
	
	SYS->LVRCR = (1 << SYS_LVRCR_EN_Pos)  |
				 (1 << SYS_LVRCR_LVL_Pos) |		//电源电压低于 2.0v 时复位芯片
				 (1 << SYS_LVRCR_WEN_Pos);
	
	while(1==1)
	{
		GPIO_ClrBit(GPIOA, PIN5);
	}
}

void GPIOA3_GPIOC3_BOD_Handler(void)
{
	SYS->BODSR = (1 << SYS_BODSR_IF_Pos);		//清除中断标志
	
	GPIO_SetBit(GPIOA, PIN5);
}
