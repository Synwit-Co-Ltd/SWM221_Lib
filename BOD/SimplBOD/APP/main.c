#include "SWM221.h"


int main(void)
{
	SystemInit();
	
	GPIO_Init(GPIOA, PIN5, 1, 0, 0, 0);			//����� ��LED
	
	SYS->BODCR = (1 << SYS_BODCR_EN_Pos) |
				 (1 << SYS_BODCR_IE_Pos) |
				 (4 << SYS_BODCR_LVL_Pos);		//��Դ��ѹ���� 3.7v ʱ�����ж�
	NVIC_EnableIRQ(GPIOA3_GPIOC3_BOD_IRQn);
	
	SYS->LVRCR = (1 << SYS_LVRCR_EN_Pos)  |
				 (1 << SYS_LVRCR_LVL_Pos) |		//��Դ��ѹ���� 2.0v ʱ��λоƬ
				 (1 << SYS_LVRCR_WEN_Pos);
	
	while(1==1)
	{
		GPIO_ClrBit(GPIOA, PIN5);
	}
}

void GPIOA3_GPIOC3_BOD_Handler(void)
{
	SYS->BODSR = (1 << SYS_BODSR_IF_Pos);		//����жϱ�־
	
	GPIO_SetBit(GPIOA, PIN5);
}
