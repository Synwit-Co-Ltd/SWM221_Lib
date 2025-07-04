#include "SWM221.h"


int main(void)
{
	PWM_InitStructure  PWM_initStruct;
	
	SystemInit();
	
	GPIO_Init(GPIOA, PIN8, 1, 0, 0, 0);			//调试指示引脚
	
	PORT_Init(PORTA, PIN3, PORTA_PIN3_PWM0A,  0);
	PORT_Init(PORTA, PIN2, PORTA_PIN2_PWM0AN, 0);
	PORT_Init(PORTA, PIN6, PORTA_PIN6_PWM0B,  0);
	PORT_Init(PORTA, PIN7, PORTA_PIN7_PWM0BN, 0);
	
	PORT_Init(PORTA, PIN5, PORTA_PIN5_PWM1A,  0);
	PORT_Init(PORTA, PIN4, PORTA_PIN4_PWM1AN, 0);
	PORT_Init(PORTA, PIN1, PORTA_PIN1_PWM1B,  0);
	PORT_Init(PORTA, PIN0, PORTA_PIN0_PWM1BN, 0);
	
	PWM_initStruct.Mode = PWM_EDGE_ALIGNED;
	PWM_initStruct.Clkdiv = 6;					//F_PWM = 60M/6 = 10M
	PWM_initStruct.Period = 1000;				//10M/1000 = 10KHz
	PWM_initStruct.HdutyA =  250;				//250/1000 = 25%
	PWM_initStruct.DeadzoneA = 50;
	PWM_initStruct.IdleLevelA = 0;
	PWM_initStruct.IdleLevelAN= 0;
	PWM_initStruct.OutputInvA = 0;
	PWM_initStruct.OutputInvAN= 0;
	PWM_initStruct.HdutyB =  500;				//500/1000 = 50%
	PWM_initStruct.DeadzoneB = 5;
	PWM_initStruct.IdleLevelB = 0;
	PWM_initStruct.IdleLevelBN= 0;
	PWM_initStruct.OutputInvB = 0;
	PWM_initStruct.OutputInvBN= 0;
	PWM_initStruct.UpOvfIE    = 0;
	PWM_initStruct.DownOvfIE  = 0;
	PWM_initStruct.UpCmpAIE   = 0;
	PWM_initStruct.DownCmpAIE = 0;
	PWM_initStruct.UpCmpBIE   = 0;
	PWM_initStruct.DownCmpBIE = 0;
	PWM_Init(PWM0, &PWM_initStruct);
	PWM_Init(PWM1, &PWM_initStruct);
	
	PWM_Start(PWM0_MSK | PWM1_MSK);
	
	/* 刹车（Brake）功能演示 */
#if 0
#if 1
	PORT_Init(PORTB, PIN14, PORTB_PIN14_PWM_BRK0, 1);	//PB14切换为PWM_BRK0功能
	PORTB->PULLU |= (1 << PIN14);
	PORT_Init(PORTB, PIN6, PORTB_PIN6_PWM_BRK1, 1);		//PB6 切换为PWM_BRK1功能
	PORTB->PULLU |= (1 << PIN6);
#else	// 比较器输出作为刹车信号
	PORT_Init(PORTB, PIN6,  PORTB_PIN6_ACMP0_INP0, 0);
	PORT_Init(PORTB, PIN5,  PORTB_PIN5_ACMP0_INP1, 0);
	PORT_Init(PORTB, PIN4,  PORTB_PIN4_ACMP0_INP2, 0);
	PORT_Init(PORTB, PIN3,  PORTB_PIN3_ACMP0_INN,  0);
	PORT_Init(PORTA, PIN14, PORTA_PIN14_ACMP1_INP, 0);
	PORT_Init(PORTA, PIN12, PORTA_PIN12_ACMP1_INN, 0);
	
	SYS->ACMP0CR |= (1 << SYS_ACMP0CR_EN_Pos)    |
				    (1 << SYS_ACMP0CR_HYS_Pos)   |
					(1 << SYS_ACMP0CR_VNSEL_Pos) |
					(0 << SYS_ACMP0CR_VPSEL_Pos) |
				    (1 << SYS_ACMP0CR_TOPWM_Pos);
					
	SYS->ACMP1CR |= (1 << SYS_ACMP1CR_EN_Pos)    |
				    (1 << SYS_ACMP1CR_HYS_Pos)   |
					(1 << SYS_ACMP1CR_VNSEL_Pos) |
					(0 << SYS_ACMP1CR_VPSEL_Pos) |
				    (1 << SYS_ACMP1CR_TOPWM_Pos);
	
	SYS->ACMPCR = (1   << SYS_ACMPCR_DACEN_Pos) |
				  (0   << SYS_ACMPCR_DACVR_Pos) |
				  (100 << SYS_ACMPCR_DACDR_Pos);	// Vout = 100 / 255 * VDD
#endif
	PWM_BrkInPolarity(PWM_BRK0 | PWM_BRK1, 0);		// PWM_BRK0、PWM_BRK1 低电平刹车
	
	PWM_BrkConfig(PWM0, PWM_CH_A, PWM_BRK0 | PWM_BRK1, 0, 1, 1, 0);	//PWM0通道A 受刹车输入 PWM_BRK0和PWM_BRK1 控制
	PWM_BrkConfig(PWM0, PWM_CH_B, PWM_BRK0 | PWM_BRK1, 0, 1, 1, 0);	//PWM0通道B 受刹车输入 PWM_BRK0和PWM_BRK1 控制
	
	/* 刹车（Brake）中断功能演示 */
#if 0
	PWM_BrkIntEn(PWM_BRKIT_BRK0);
	NVIC_EnableIRQ(PWMBRK_IRQn);
#endif
#endif

	/* 软件刹车（Software Brake）功能演示 */
#if 0
	PWM_BrkConfig(PWM0, PWM_CH_A, 0, 0, 1, 1, 0);	//PWM0通道A 不受刹车输入影响，只能软件刹车
	PWM_BrkConfig(PWM0, PWM_CH_B, 0, 0, 1, 1, 0);	//PWM0通道B 不受刹车输入影响，只能软件刹车
	
	while(1)
	{
		GPIO_SetBit(GPIOA, PIN8);
		PWMG->SWBRK |= (PWMG_SWBRK_PWM0A_Msk |		//软件触发PWM0A和PWM0B的刹车动作
						PWMG_SWBRK_PWM0B_Msk);
		for(int i = 0; i < 300000; i++) {}
		
		GPIO_ClrBit(GPIOA, PIN8);
		PWMG->SWBRK &=~(PWMG_SWBRK_PWM0A_Msk |		//撤销PWM0A和PWM0B的软件刹车，恢复正常输出
						PWMG_SWBRK_PWM0B_Msk);
		for(int i = 0; i < 900000; i++) {}
	}
#endif

	while(1==1)
	{
	}
}


void PWMBRK_Handler(void)
{
	if(PWM_BrkIntStat(PWM_BRKIT_BRK0))
	{
		PWM_BrkIntDis(PWM_BRKIT_BRK0);	//必须关掉刹车中断，否则刹车期间不停触发中断
		
		GPIO_InvBit(GPIOA, PIN8);
	}
}
