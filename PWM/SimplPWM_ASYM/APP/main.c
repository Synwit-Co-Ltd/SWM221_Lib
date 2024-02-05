#include "SWM221.h"


int main(void)
{
	uint32_t i;
	PWM_InitStructure  PWM_initStruct;
	
	SystemInit();
		
	PORT_Init(PORTA, PIN3, PORTA_PIN3_PWM0A,  0);
	PORT_Init(PORTA, PIN2, PORTA_PIN2_PWM0AN, 0);
	PORT_Init(PORTA, PIN6, PORTA_PIN6_PWM0B,  0);
	PORT_Init(PORTA, PIN7, PORTA_PIN7_PWM0BN, 0);
	
	PORT_Init(PORTA, PIN5, PORTA_PIN5_PWM1A,  0);
	PORT_Init(PORTM, PIN4, PORTM_PIN4_PWM1AN, 0);
	PORT_Init(PORTA, PIN4, PORTA_PIN4_PWM1B,  0);
	PORT_Init(PORTM, PIN5, PORTM_PIN5_PWM1BN, 0);
	
	PWM_initStruct.Mode = PWM_ASYM_CENTER_ALIGNED;
	PWM_initStruct.Clkdiv = 6;					//F_PWM = 60M/6 = 10M
	PWM_initStruct.Period = 10000;				//10M/(10000 + 10000) = 500Hz
	PWM_initStruct.HdutyA =  2500;				//(2500+2500)/(10000+10000) = 25%
	PWM_initStruct.HdutyA2 = 2500;
	PWM_initStruct.DeadzoneA = 50;
	PWM_initStruct.IdleLevelA = 0;
	PWM_initStruct.IdleLevelAN= 0;
	PWM_initStruct.OutputInvA = 0;
	PWM_initStruct.OutputInvAN= 0;
	PWM_initStruct.HdutyB =  7500;				//(7500+7500)/(10000+10000) = 75%
	PWM_initStruct.HdutyB2 = 7500;
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
	
	while(1==1)
	{
		PWMG->RELOADEN = 0x00;		// 关闭工作寄存器加载更新，保证所有寄存器都更新完后再更新输出波形
		
		/* 使用非对称中心对齐模式，保持输出波形占空比不变，实现移相功能 */
#if 0
		if(PWM1->CMPA2 == 0)
		{
			PWM1->CMPA = 2500;
			PWM1->CMPA2 = 2500;
			PWM1->CMPB = 7500;
			PWM1->CMPB2 = 7500;
		}
		else
		{
			PWM1->CMPA += 250;		// 前半周期的高电平时长增加
			PWM1->CMPA2 -= 250;		// 后半周期的高电平时长减小，高电平向右移相
			PWM1->CMPB += 750;		// 注意：这里说的前半周期、后半周期是针对 PWM 计数器的计数方向说的，向上计数称作前半周期，向下计数称作后半周期
			PWM1->CMPB2 -= 750;		// 从PWM输出的波形上看，前半周期反而对应 PWM 高电平的右侧部分，后半周期反而对应 PWM 高电平的左侧部分
		}
#else
		if(PWM1->CMPA == 0)
		{
			PWM1->CMPA = 2500;
			PWM1->CMPA2 = 2500;
			PWM1->CMPB = 7500;
			PWM1->CMPB2 = 7500;
		}
		else
		{
			PWM1->CMPA -= 250;		// 前半周期的高电平时长增加
			PWM1->CMPA2 += 250;		// 后半周期的高电平时长减小，高电平向左移相
			PWM1->CMPB -= 750;
			PWM1->CMPB2 += 750;
		}
#endif
		PWMG->RELOADEN = 0x3F;
		
		for(i = 0; i < SystemCoreClock / 32; i++) __NOP();
	}
}

