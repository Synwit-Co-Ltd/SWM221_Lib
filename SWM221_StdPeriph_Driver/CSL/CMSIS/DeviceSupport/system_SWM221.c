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
#define SYS_CLK_8MHz		0	 	//内部高频8MHz RC振荡器
#define SYS_CLK_1MHz		1		//内部高频1MHz RC振荡器
#define SYS_CLK_XTAL		2		//外部晶体振荡器（8-24MHz）
#define SYS_CLK_XTAL_DIV8	3		//外部晶体振荡器（8-24MHz） 8分频
#define SYS_CLK_PLL			4		//锁相环输出
#define SYS_CLK_PLL_DIV8	5		//锁相环输出 8分频
#define SYS_CLK_32KHz		6		//内部低频32KHz RC  振荡器

#define SYS_CLK   SYS_CLK_PLL


#define __HSI		( 8000000UL)		//高速内部时钟
#define __LSI		(   32000UL)		//低速内部时钟
#define __HSE		(12000000UL)		//高速外部时钟
#define __LSE		(   32000UL)		//低速外部时钟


/********************************** PLL 设定 **********************************************
 * VCO输出频率 = PLL输入时钟 / INDIV * 4 * FBDIV
 * PLL输出频率 = PLL输入时钟 / INDIV * 4 * FBDIV / OUTDIV = VCO输出频率 / OUTDIV         
 *****************************************************************************************/ 
#define SYS_PLL_SRC   	SYS_CLK_XTAL	//可取值SYS_CLK_8MHz、SYS_CLK_XTAL

#define PLL_IN_DIV		3

#define PLL_FB_DIV		30


#define PLL_OUT_DIV8	0
#define PLL_OUT_DIV4	1
#define PLL_OUT_DIV2	2

#define PLL_OUT_DIV		PLL_OUT_DIV8


uint32_t SystemCoreClock  = __HSI;   				//System Clock Frequency (Core Clock)
uint32_t CyclesPerUs      = (__HSI / 1000000); 		//Cycles per micro second


/****************************************************************************************************************************************** 
* 函数名称: 
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
		case 0:
			SystemCoreClock = __LSI;
			break;
		
		case 1:
			if(SYS->PLLCR & SYS_PLLCR_INSEL_Msk)		//PLL_IN <= HRC
			{
				SystemCoreClock = __HSI;
			}
			else										//PLL_IN <= XTAL
			{
				SystemCoreClock = __HSE;
			}
			
			SystemCoreClock = SystemCoreClock / PLL_IN_DIV * PLL_FB_DIV * 4 / (2 << (2 - PLL_OUT_DIV));
			break;
		
		case 3:
			SystemCoreClock = __HSE;
			break;
		
		case 4:
			SystemCoreClock = __HSI;
			break;
		}
		
		if(SYS->CLKSEL & SYS_CLKSEL_CLK_DIVx_Msk)  SystemCoreClock /= 8;
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
	
	Flash_Param_at_xMHz(90);
	
	switch(SYS_CLK)
	{
		case SYS_CLK_8MHz:
			switchTo8MHz();
			break;
		
		case SYS_CLK_1MHz:
			switchTo1MHz();
			break;
		
		case SYS_CLK_XTAL:
			switchToXTAL(0);
			break;
		
		case SYS_CLK_XTAL_DIV8:
			switchToXTAL(1);
			break;
		
		case SYS_CLK_PLL:
			switchToPLL(0);
			break;
		
		case SYS_CLK_PLL_DIV8:
			switchToPLL(1);
			break;

		case SYS_CLK_32KHz:
			switchTo32KHz();
			break;
	}
	
	SystemCoreClockUpdate();
	
	Flash_Param_at_xMHz(SystemCoreClock/1000000);
	
	FMC->CACHE = (1 << FMC_CACHE_CEN_Pos) | FMC_CACHE_CCLR_Msk;	// 清除 Cache
	__NOP(); __NOP(); __NOP(); __NOP(); __NOP(); __NOP(); __NOP();
	FMC->CACHE = (1 << FMC_CACHE_CEN_Pos) | (1 << FMC_CACHE_CPEN_Pos);
	
	PORTB->PULLD &= ~((1 << PIN10) | (1 << PIN11));
	PORTB->PULLU &= ~((1 << PIN12) | (1 << PIN14));
}

void switchTo8MHz(void)
{
	SYS->RCCR |= (1 << SYS_RCCR_HON_Pos);
	
	SYS->CLKSEL |= (1 << SYS_CLKSEL_SYS_Pos);		//SYS <= HRC
}

void switchTo1M5Hz(void)
{
	switchTo8MHz();
	
	SYS->CLKDIVx_ON = 1;
	
	SYS->CLKSEL &= ~SYS_CLKSEL_CLK_Msk;
	SYS->CLKSEL |= (4 << SYS_CLKSEL_CLK_Pos);		//CLK <= HRC

	SYS->CLKSEL |= (1 << SYS_CLKSEL_CLK_DIVx_Pos);
	
	SYS->CLKSEL &=~(1 << SYS_CLKSEL_SYS_Pos);		//SYS <= HRC/8
}

void switchToXTAL(uint32_t div8)
{
	uint32_t i;
	
	switchTo8MHz();
	
	PORTB->PULLU &= ~((1 << PIN11) | (1 << PIN12));
	PORTB->PULLD &= ~((1 << PIN11) | (1 << PIN12));
	PORT_Init(PORTB, PIN11, PORTB_PIN11_XTAL_IN,  0);
	PORT_Init(PORTB, PIN12, PORTB_PIN12_XTAL_OUT, 0);
	SYS->XTALCR |= (1 << SYS_XTALCR_ON_Pos) | (1 << SYS_XTALCR_DET_Pos);
	for(i = 0; i < 1000; i++) __NOP();
	
	SYS->CLKDIVx_ON = 1;
	
	SYS->CLKSEL &= ~SYS_CLKSEL_CLK_Msk;
	SYS->CLKSEL |= (3 << SYS_CLKSEL_CLK_Pos);		//CLK <= XTAL

	if(div8) SYS->CLKSEL |= (1 << SYS_CLKSEL_CLK_DIVx_Pos);
	else     SYS->CLKSEL &=~(1 << SYS_CLKSEL_CLK_DIVx_Pos);
	
	SYS->CLKSEL &=~(1 << SYS_CLKSEL_SYS_Pos);		//SYS <= XTAL
}

void switchToPLL(uint32_t div8)
{
	switchTo8MHz();
	
	PLLInit();
	
	SYS->CLKDIVx_ON = 1;
	
	SYS->CLKSEL &= ~SYS_CLKSEL_CLK_Msk;
	SYS->CLKSEL |= (1 << SYS_CLKSEL_CLK_Pos);		//CLK <= PLL

	if(div8)  SYS->CLKSEL |= (1 << SYS_CLKSEL_CLK_DIVx_Pos);
	else      SYS->CLKSEL &=~(1 << SYS_CLKSEL_CLK_DIVx_Pos);
	
	SYS->CLKSEL &=~(1 << SYS_CLKSEL_SYS_Pos);		//SYS <= PLL
}

void switchTo32KHz(void)
{
	switchTo8MHz();
	
	SYS->RCCR |= (1 << SYS_RCCR_LON_Pos);
	
	SYS->CLKDIVx_ON = 1;
	
	SYS->CLKSEL &= ~SYS_CLKSEL_CLK_Msk;
	SYS->CLKSEL |= (0 << SYS_CLKSEL_CLK_Pos);		//CLK <= LRC

	SYS->CLKSEL &=~(1 << SYS_CLKSEL_CLK_DIVx_Pos);

	SYS->CLKSEL &=~(1 << SYS_CLKSEL_SYS_Pos);		//SYS <= LRC
}


void PLLInit(void)
{	
	if(SYS_PLL_SRC == SYS_CLK_8MHz)
	{		
		SYS->RCCR |= (1 << SYS_RCCR_HON_Pos);
	}
	else if(SYS_PLL_SRC == SYS_CLK_XTAL)
	{		
		PORTB->PULLU &= ~((1 << PIN11) | (1 << PIN12));
		PORTB->PULLD &= ~((1 << PIN11) | (1 << PIN12));
		PORT_Init(PORTB, PIN11, PORTB_PIN11_XTAL_IN,  0);
		PORT_Init(PORTB, PIN12, PORTB_PIN12_XTAL_OUT, 0);
		SYS->XTALCR |= (1 << SYS_XTALCR_ON_Pos) | (1 << SYS_XTALCR_DET_Pos);
	}
	
	SYS->PLLCR = (0 		<< SYS_PLLCR_PWRDN_Pos) |
				 (1 		<< SYS_PLLCR_OUTEN_Pos) |
				 ((SYS_PLL_SRC == SYS_CLK_XTAL) << SYS_PLLCR_INSEL_Pos) |
				 (0 		 << SYS_PLLCR_BYPASS_Pos) |
				 (PLL_IN_DIV << SYS_PLLCR_INDIV_Pos)  |
				 (PLL_FB_DIV << SYS_PLLCR_FBDIV_Pos);
	
	while((SYS->PLLSR & SYS_PLLSR_LOCK_Msk) == 0) __NOP();		//等待PLL锁定
	
	SYS->PLLSR |= SYS_PLLSR_ENA_Msk;
}
