#include "SWM221.h"


int main(void)
{
	uint32_t i;
	PWM_InitStructure  PWM_initStruct;
	
	SystemInit();
	
	GPIO_Init(GPIOA, PIN8, 1, 0, 0, 0);			//调试指示引脚
	
	PORT_Init(PORTA, PIN3, PORTA_PIN3_PWM0A,  0);
	PORT_Init(PORTA, PIN2, PORTA_PIN2_PWM0AN, 0);
	PORT_Init(PORTA, PIN6, PORTA_PIN6_PWM0B,  0);
	PORT_Init(PORTA, PIN7, PORTA_PIN7_PWM0BN, 0);
	
	PORT_Init(PORTA, PIN5, PORTA_PIN5_PWM1A,  0);
	PORT_Init(PORTM, PIN4, PORTM_PIN4_PWM1AN, 0);
	PORT_Init(PORTA, PIN4, PORTA_PIN4_PWM1B,  0);
	PORT_Init(PORTM, PIN5, PORTM_PIN5_PWM1BN, 0);
	
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
	PORT_Init(PORTB, PIN14, PORTB_PIN14_PWM_BRK0, 1);	//PB14切换为PWM_BRK0功能
	PORTB->PULLU |= (1 << PIN14);
	PORT_Init(PORTB, PIN6, PORTB_PIN6_PWM_BRK1, 1);		//PB6 切换为PWM_BRK1功能
	PORTB->PULLU |= (1 << PIN6);
	
	PWM_BrkInPolarity(PWM_BRK0 | PWM_BRK1 | PWM_BRK2, 0);			//PWM_BRK0、PWM_BRK1、PWM_BRK2 低电平刹车
	
	PWM_BrkConfig(PWM0, PWM_CH_A, PWM_BRK0 | PWM_BRK1, 0, 1, 1, 0);	//PWM0通道A 受刹车输入 PWM_BRK0和PWM_BRK1 控制
	PWM_BrkConfig(PWM0, PWM_CH_B, PWM_BRK0 | PWM_BRK1, 0, 1, 1, 0);	//PWM0通道B 受刹车输入 PWM_BRK0和PWM_BRK1 控制
	PWM_BrkConfig(PWM1, PWM_CH_A, PWM_BRK2, 0, 1, 1, 0);			//PWM1通道A 受刹车输入 PWM_BRK2 控制
	
	/* ACMP3_OUT => PWM_BRK2 */
#if 1
	PORT_Init(PORTA, PIN14, PORTA_PIN14_ACMP3_INP, 0);
// 	PORT_Init(PORTB, PIN14, PORTB_PIN14_ACMP3_INN, 0);
	
	SYS->ACMPCR |= (1 << SYS_ACMPCR_CMP3ON_Pos)  | 
				   (1 << SYS_ACMPCR_CMP3HYS_Pos) | 
			       (0 << SYS_ACMPCR_CMP3IE_Pos);	//开启迟滞、关闭中断

	SYS->DACCR = (1 << SYS_DACCR_EN_Pos) |
				 (100 << SYS_DACCR_DATA_Pos);		//内部VREF电压：SYS->DACCR.DATA / 255 * 5V
	SYS->ACMPCR2 |= (1 << SYS_ACMPCR2_3NVR_Pos);	//ACMP3 N输入端接内部VREF
	
#if 0	//ACMP3输出宽度低于 1/12us * 4096 = 340us 的脉冲视作毛刺，过滤掉
	IOFILT_Init(1, IOFILT1_ACMP3, IOFILT_WIDTH_4096);
	
	while(1)	// 将 PA8 与 PA14 连接，当低脉冲宽度大于 340us 时会出现刹车，否则不出现刹车
	{
		GPIO_ClrBit(GPIOA, PIN8);
		for(i = 0; i < CyclesPerUs*65; i++) __NOP();
		
		GPIO_SetBit(GPIOA, PIN8);
		for(i = 0; i < CyclesPerUs*1000; i++) __NOP();
	}
#endif
#endif
	
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
		for(i = 0; i < 300000; i++) __NOP();
		
		GPIO_ClrBit(GPIOA, PIN8);
		PWMG->SWBRK &=~(PWMG_SWBRK_PWM0A_Msk |		//撤销PWM0A和PWM0B的软件刹车，恢复正常输出
						PWMG_SWBRK_PWM0B_Msk);
		for(i = 0; i < 900000; i++) __NOP();
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
