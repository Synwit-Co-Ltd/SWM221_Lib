/****************************************************************************************************************************************** 
* 文件名称:	system_SWM221.c
* 功能说明:	SWM221单片机的时钟设置
* 技术支持:	http://www.synwit.com.cn/e/tool/gbook/?bid=1
* 注意事项:
* 版本日期: V1.0.0		2016年1月30日
* 升级记录: 
*
*
*******************************************************************************************************************************************
* @attention
*
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS WITH CODING INFORMATION 
* REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME. AS A RESULT, SYNWIT SHALL NOT BE HELD LIABLE 
* FOR ANY DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE CONTENT 
* OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING INFORMATION CONTAINED HEREIN IN CONN-
* -ECTION WITH THEIR PRODUCTS.
*
* COPYRIGHT 2012 Synwit Technology
*******************************************************************************************************************************************/ 
#include <stdint.h>
#include "SWM221.h"


/******************************************************************************************************************************************
 * 系统时钟设定
 *****************************************************************************************************************************************/
#define SYS_CLK_8MHz		3	 	//内部高频8MHz RC振荡器
#define SYS_CLK_XTAL		2		//外部晶体振荡器（4-24MHz）
#define SYS_CLK_PLL			1		//锁相环输出
#define SYS_CLK_32KHz		0		//内部低频32KHz RC振荡器

#define SYS_CLK   	SYS_CLK_PLL


#define SYS_CLK_DIV	SYS_CLK_DIV_1	//SYS_CLK 选择的时钟，经过 SYS_CLK_DIV 分频后用作系统时钟



#define __HSI		( 8000000UL)		//高速内部时钟
#define __LSI		(   32000UL)		//低速内部时钟
#define __HSE		( 8000000UL)		//高速外部时钟
#define __LSE		(   32768UL)		//低速外部时钟


/********************************** PLL 设定 **********************************************
 * PLL输出频率 = PLL输入时钟 / INDIV * FBDIV
 *****************************************************************************************/ 
#define SYS_PLL_SRC   	SYS_CLK_XTAL	//可取值SYS_CLK_8MHz、SYS_CLK_XTAL

#define PLL_IN_DIV		2

#define PLL_FB_DIV		15



uint32_t SystemCoreClock  = __HSI;   				//System Clock Frequency (Core Clock)
uint32_t CyclesPerUs      = (__HSI / 1000000); 		//Cycles per micro second


/****************************************************************************************************************************************** 
* 函数名称: SystemCoreClockUpdate()
* 功能说明: This function is used to update the variable SystemCoreClock and must be called whenever the core clock is changed
* 输    入: 
* 输    出: 
* 注意事项: 
******************************************************************************************************************************************/
void SystemCoreClockUpdate(void)    
{
	if(SYS->CLKSEL & SYS_CLKSEL_SYS_Msk)			//SYS  <= HRC
	{
		SystemCoreClock = __HSI;
	}
	else											//SYS  <= CLK
	{
		switch((SYS->CLKSEL & SYS_CLKSEL_CLK_Msk) >> SYS_CLKSEL_CLK_Pos)
		{
		case SYS_CLK_8MHz:
			SystemCoreClock = __HSI;
			break;
		
		case SYS_CLK_XTAL:
			SystemCoreClock = __HSE;
			break;
		
		case SYS_CLK_PLL:
		{
			uint32_t indiv = (SYS->PLLCR & SYS_PLLCR_INDIV_Msk) >> SYS_PLLCR_INDIV_Pos;
			uint32_t fbdiv = (SYS->PLLCR & SYS_PLLCR_FBDIV_Msk) >> SYS_PLLCR_FBDIV_Pos;
			
			if(SYS->PLLCR & SYS_PLLCR_INSEL_Msk)
			{
				SystemCoreClock = __HSE * fbdiv / indiv;
			}
			else
			{
				SystemCoreClock = __HSI * fbdiv / indiv;
			}
			break;
		}
		
		case SYS_CLK_32KHz:
			SystemCoreClock = __LSI;
			break;
		}
		
		SystemCoreClock /= (1 << ((SYS->CLKSEL & SYS_CLKSEL_CLK_DIVx_Msk) >> SYS_CLKSEL_CLK_DIVx_Pos));
	}
	
	CyclesPerUs = SystemCoreClock / 1000000;
}


/****************************************************************************************************************************************** 
* 函数名称: 
* 功能说明: The necessary initializaiton of systerm
* 输    入: 
* 输    出: 
* 注意事项: 
******************************************************************************************************************************************/
void SystemInit(void)
{
	SYS->CLKEN0 |= (1 << SYS_CLKEN0_ANAC_Pos);
	
	Flash_Param_at_xMHz(72);
	
	switchToHRC();
	
	switch(SYS_CLK)
	{
		case SYS_CLK_8MHz:
			switchOnHRC();
			break;
		
		case SYS_CLK_XTAL:
			switchOnXTAL();
			break;
		
		case SYS_CLK_PLL:
			switchOnPLL(SYS_PLL_SRC, PLL_IN_DIV, PLL_FB_DIV);
			break;
		
		case SYS_CLK_32KHz:
			switchOn32KHz();
			break;
	}
	
	switchToDIV(SYS_CLK, SYS_CLK_DIV);
	
	Flash_Param_at_xMHz(CyclesPerUs);
	
	Cache_Clear();						// Cache Clear
	
	FMC->CACHE |= FMC_CACHE_CEN_Msk;	// Cache Enable
	
	PORTB->PULLD &= ~((1 << PIN10) | (1 << PIN11));
	PORTB->PULLU &= ~((1 << PIN12) | (1 << PIN15));
}

void switchToHRC(void)
{
	SYS->RCCR |= (1 << SYS_RCCR_HON_Pos);
	
	for(int i = 0; i < CyclesPerUs; i++) {}
	
	SYS->CLKSEL |= (1 << SYS_CLKSEL_SYS_Pos);		//SYS <= HRC
	
	SystemCoreClockUpdate();
}

void switchToDIV(uint32_t src, uint32_t div)
{
	SYS->CLKSEL &=~(SYS_CLKSEL_CLK_Msk | SYS_CLKSEL_CLK_DIVx_Msk);
	SYS->CLKSEL |= (src << SYS_CLKSEL_CLK_Pos) |
				   (div << SYS_CLKSEL_CLK_DIVx_Pos);
	
	SYS->CLKDIVx_ON = 1;
	
	for(int i = 0; i < CyclesPerUs; i++) {}
	
	SYS->CLKSEL &=~(1 << SYS_CLKSEL_SYS_Pos);		//SYS <= CLK_DIVx
	
	SystemCoreClockUpdate();
}

void switchOnHRC(void)
{
	SYS->RCCR |= (1 << SYS_RCCR_HON_Pos);
}

void switchOnXTAL(void)
{
	PORTB->PULLU &= ~((1 << PIN11) | (1 << PIN12));
	PORTB->PULLD &= ~((1 << PIN11) | (1 << PIN12));
	
	PORT_Init(PORTB, PIN11, PORTB_PIN11_XTAL_IN,  0);
	PORT_Init(PORTB, PIN12, PORTB_PIN12_XTAL_OUT, 0);
	
	SYS->XTALCR |= (1 << SYS_XTALCR_ON_Pos) | (1 << SYS_XTALCR_DET_Pos);
}

void switchOnPLL(uint32_t src, uint32_t indiv, uint32_t fbdiv)
{
	if(src == SYS_CLK_XTAL)
		switchOnXTAL();
	else
		switchOnHRC();
	
	SYS->PLLCR = (0						<< SYS_PLLCR_PWRDN_Pos)  |
				 (1						<< SYS_PLLCR_OUTEN_Pos)  |
				 ((src == SYS_CLK_XTAL) << SYS_PLLCR_INSEL_Pos)  |
				 (0						<< SYS_PLLCR_BYPASS_Pos) |
				 (indiv					<< SYS_PLLCR_INDIV_Pos)  |
				 (fbdiv					<< SYS_PLLCR_FBDIV_Pos);
	
	while((SYS->PLLSR & SYS_PLLSR_LOCK_Msk) == 0) __NOP();		//等待PLL锁定
	
	SYS->PLLSR |= SYS_PLLSR_ENA_Msk;
}

void switchOn32KHz(void)
{
	SYS->RCCR |= (1 << SYS_RCCR_LON_Pos);
}
