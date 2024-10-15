#include "SWM221.h"


int main(void)
{
	SystemInit();
	
	GPIO_Init(GPIOA, PIN5, 1, 0, 0, 0);			//输出， 接LED
	
	SYS->PVDCR = (1 << SYS_PVDCR_EN_Pos) |
				 (1 << SYS_PVDCR_IE_Pos) |
				 (4 << SYS_PVDCR_LVL_Pos);		//电源电压低于 3.7v 时产生中断
	NVIC_EnableIRQ(GPIOA3_GPIOC3_PVD_IRQn);
	
	SYS->LVRCR = (1 << SYS_LVRCR_EN_Pos)  |
				 (1 << SYS_LVRCR_LVL_Pos) |		//电源电压低于 2.0v 时复位芯片
				 (1 << SYS_LVRCR_WEN_Pos);
	
	while(1==1)
	{
		GPIO_ClrBit(GPIOA, PIN5);
	}
}

void GPIOA3_GPIOC3_PVD_Handler(void)
{
	SYS->PVDSR = (1 << SYS_PVDSR_IF_Pos);		//清除中断标志
	
	GPIO_SetBit(GPIOA, PIN5);
}
