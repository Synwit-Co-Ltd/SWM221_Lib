/*******************************************************************************************************************************
* @brief	Timer driver
*
*
********************************************************************************************************************************
* @attention
*
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS WITH CODING INFORMATION 
* REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME. AS A RESULT, SYNWIT SHALL NOT BE HELD LIABLE 
* FOR ANY DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE CONTENT 
* OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING INFORMATION CONTAINED HEREIN IN CONN-
* -ECTION WITH THEIR PRODUCTS.
*
* COPYRIGHT 2012 Synwit Technology 
*******************************************************************************************************************************/
#include "SWM221.h"
#include "SWM221_timr.h"


/*******************************************************************************************************************************
* @brief	Timer init
* @param	TIMRx is timer to init, can be TIMR0~2 and BTIMR0~3
* @param	mode for TIMR0~2 can be TIMR_MODE_TIMER, TIMR_MODE_COUNTER, TIMR_MODE_OC and TIMR_MODE_IC
*			mode for BTIMR0~3 can be TIMR_MODE_TIMER and TIMR_MODE_OC
* @param	prediv is prescale for counting clock, can be 1-256
* @param	period is counting period, can be 1-16777216
* @param	int_en is interrupt enable
* @return
*******************************************************************************************************************************/
void TIMR_Init(TIMR_TypeDef * TIMRx, uint32_t mode, uint16_t prediv, uint32_t period, uint32_t int_en)
{
	if((TIMRx == TIMR0) || (TIMRx == TIMR1) || (TIMRx == TIMR2))
	{
		SYS->CLKEN0 |= (0x01 << SYS_CLKEN0_TIMR_Pos);
	}
	else
	{
		SYS->CLKEN0 |= (0x01 << SYS_CLKEN0_BTIMR_Pos);
	}
	
	TIMR_Stop(TIMRx);
	
	TIMRx->CR &= ~(TIMR_CR_MODE_Msk | TIMR_CR_CLKSRC_Msk);
	TIMRx->CR |= (mode << TIMR_CR_CLKSRC_Pos);
	
	TIMRx->PREDIV = prediv - 1;
	
	TIMRx->LOAD = period - 1;
	
	TIMRx->IF = (1 << TIMR_IF_TO_Pos);		// clear interrupt flag
	if(int_en) TIMRx->IE |=  (1 << TIMR_IE_TO_Pos);
	else	   TIMRx->IE &= ~(1 << TIMR_IE_TO_Pos);
	
	switch((uint32_t)TIMRx)
	{
	case ((uint32_t)TIMR0):
		if(int_en) NVIC_EnableIRQ(TIMR0_IRQn);
		break;
	
	case ((uint32_t)TIMR1):
		if(int_en) NVIC_EnableIRQ(TIMR1_IRQn);
		break;
	
	case ((uint32_t)TIMR2):
		if(int_en) NVIC_EnableIRQ(GPIOB0_GPIOA8_TIMR2_IRQn);
		break;
	
	case ((uint32_t)BTIMR0):
		if(int_en) NVIC_EnableIRQ(BTIMR0_IRQn);
		break;
	
	case ((uint32_t)BTIMR1):
		if(int_en) NVIC_EnableIRQ(BTIMR1_IRQn);
		break;
	
	case ((uint32_t)BTIMR2):
		if(int_en) NVIC_EnableIRQ(BTIMR2_IRQn);
		break;
	
	case ((uint32_t)BTIMR3):
		if(int_en) NVIC_EnableIRQ(BTIMR3_IRQn);
		break;
	}
}

/*******************************************************************************************************************************
* @brief	Timer start
* @param	TIMRx is timer to start, can be TIMR0~2 and BTIMR0~3
* @return
*******************************************************************************************************************************/
void TIMR_Start(TIMR_TypeDef * TIMRx)
{
	switch((uint32_t)TIMRx)
	{
	case ((uint32_t)TIMR0):
		TIMRG->EN |= (1 << TIMRG_EN_TIMR0_Pos);
		break;
	
	case ((uint32_t)TIMR1):
		TIMRG->EN |= (1 << TIMRG_EN_TIMR1_Pos);
		break;
	
	case ((uint32_t)TIMR2):
		TIMRG->EN |= (1 << TIMRG_EN_TIMR2_Pos);
		break;
	
	case ((uint32_t)BTIMR0):
		BTIMRG->EN |= (1 << TIMRG_EN_TIMR0_Pos);
		break;
	
	case ((uint32_t)BTIMR1):
		BTIMRG->EN |= (1 << TIMRG_EN_TIMR1_Pos);
		break;
	
	case ((uint32_t)BTIMR2):
		BTIMRG->EN |= (1 << TIMRG_EN_TIMR2_Pos);
		break;
	
	case ((uint32_t)BTIMR3):
		BTIMRG->EN |= (1 << TIMRG_EN_TIMR3_Pos);
		break;
	}
}

/*******************************************************************************************************************************
* @brief	Timer stop
* @param	TIMRx is timer to stop, can be TIMR0~2 and BTIMR0~3
* @return
*******************************************************************************************************************************/
void TIMR_Stop(TIMR_TypeDef * TIMRx)
{
	switch((uint32_t)TIMRx)
	{
	case ((uint32_t)TIMR0):
		TIMRG->EN &= ~(1 << TIMRG_EN_TIMR0_Pos);
		break;
	
	case ((uint32_t)TIMR1):
		TIMRG->EN &= ~(1 << TIMRG_EN_TIMR1_Pos);
		break;
	
	case ((uint32_t)TIMR2):
		TIMRG->EN &= ~(1 << TIMRG_EN_TIMR2_Pos);
		break;
	
	case ((uint32_t)BTIMR0):
		BTIMRG->EN &= ~(1 << TIMRG_EN_TIMR0_Pos);
		break;
	
	case ((uint32_t)BTIMR1):
		BTIMRG->EN &= ~(1 << TIMRG_EN_TIMR1_Pos);
		break;
	
	case ((uint32_t)BTIMR2):
		BTIMRG->EN &= ~(1 << TIMRG_EN_TIMR2_Pos);
		break;
	
	case ((uint32_t)BTIMR3):
		BTIMRG->EN &= ~(1 << TIMRG_EN_TIMR3_Pos);
		break;
	}
}

/*******************************************************************************************************************************
* @brief	Timer halt, counter keep not change
* @param	TIMRx is timer to halt, can be TIMR0~2
* @return
*******************************************************************************************************************************/
void TIMR_Halt(TIMR_TypeDef * TIMRx)
{
	TIMRx->HALT = 1;
}

/*******************************************************************************************************************************
* @brief	Timer resume, continue counting from the halt
* @param	TIMRx is timer to resume, can be TIMR0~2
* @return
*******************************************************************************************************************************/
void TIMR_Resume(TIMR_TypeDef * TIMRx)
{
	TIMRx->HALT = 0;
}

/*******************************************************************************************************************************
* @brief	Timer current count value query
* @param	TIMRx is timer to query, can be TIMR0~2 and BTIMR0~3
* @return	current count value
*******************************************************************************************************************************/
uint32_t TIMR_GetCurValue(TIMR_TypeDef * TIMRx)
{
	return TIMRx->VALUE;
}

/*******************************************************************************************************************************
* @brief	Timer interrupt enable
* @param	TIMRx is timer to set, can be TIMR0~2 and BTIMR0~3
* @param	it for TIMR0~2 can be TIMR_IT_TO, TIMR_IT_OC, TIMR_IT_IC_LOW, TIMR_IT_IC_HIGH and their '|' operation
*			it for BTIMR0~3 can be TIMR_IT_TO, TIMR_IT_OC and their '|' operation
* @return
*******************************************************************************************************************************/
void TIMR_INTEn(TIMR_TypeDef * TIMRx, uint32_t it)
{
	TIMRx->IE |= it;
}

/*******************************************************************************************************************************
* @brief	Timer interrupt disable
* @param	TIMRx is timer to set, can be TIMR0~2 and BTIMR0~3
* @param	it for TIMR0~2 can be TIMR_IT_TO, TIMR_IT_OC, TIMR_IT_IC_LOW, TIMR_IT_IC_HIGH and their '|' operation
*			it for BTIMR0~3 can be TIMR_IT_TO, TIMR_IT_OC and their '|' operation
* @return
*******************************************************************************************************************************/
void TIMR_INTDis(TIMR_TypeDef * TIMRx, uint32_t it)
{
	TIMRx->IE &= ~it;
}

/*******************************************************************************************************************************
* @brief	Timer interrupt clear
* @param	TIMRx is timer to set, can be TIMR0~2 and BTIMR0~3
* @param	it for TIMR0~2 can be TIMR_IT_TO, TIMR_IT_OC, TIMR_IT_IC_LOW, TIMR_IT_IC_HIGH and their '|' operation
*			it for BTIMR0~3 can be TIMR_IT_TO, TIMR_IT_OC and their '|' operation
* @return
*******************************************************************************************************************************/
void TIMR_INTClr(TIMR_TypeDef * TIMRx, uint32_t it)
{
	TIMRx->IF = it;
}

/*******************************************************************************************************************************
* @brief	Timer interrupt state query
* @param	TIMRx is timer to set, can be TIMR0~2 and BTIMR0~3
* @param	it for TIMR0~2 can be TIMR_IT_TO, TIMR_IT_OC, TIMR_IT_IC_LOW, TIMR_IT_IC_HIGH and their '|' operation
*			it for BTIMR0~3 can be TIMR_IT_TO, TIMR_IT_OC and their '|' operation
* @return	1 interrupt happened, 0 interrupt not happen
*******************************************************************************************************************************/
uint32_t TIMR_INTStat(TIMR_TypeDef * TIMRx, uint32_t it)
{
	return (TIMRx->IF & it) ? 1 : 0;
}


/*******************************************************************************************************************************
* @brief	Timer output compare function init
* @param	TIMRx is timer to set, can be TIMR0~2 and BTIMR0~3
* @param	match: output level flip when the counter value decrease to match
* @param	match_int_en: whether generate interrupt when the counter value decrease to match
* @param	init_lvl is initial output level
* @return
*******************************************************************************************************************************/
void TIMR_OC_Init(TIMR_TypeDef * TIMRx, uint32_t match, uint32_t match_int_en, uint32_t init_lvl)
{
	TIMRx->OCMAT = match;
	if(init_lvl) TIMRx->OCCR |=  (1 << TIMR_OCCR_INITLVL_Pos);
	else         TIMRx->OCCR &= ~(1 << TIMR_OCCR_INITLVL_Pos);
	
	TIMRx->IF = (1 << TIMR_IF_OC0_Pos);		// clear interrupt flag
	if(match_int_en) TIMRx->IE |=  (1 << TIMR_IE_OC0_Pos);
	else             TIMRx->IE &= ~(1 << TIMR_IE_OC0_Pos);
	
	switch((uint32_t)TIMRx)
	{
	case ((uint32_t)TIMR0):
		if(match_int_en) NVIC_EnableIRQ(TIMR0_IRQn);
		break;
	
	case ((uint32_t)TIMR1):
		if(match_int_en) NVIC_EnableIRQ(TIMR1_IRQn);
		break;
	
	case ((uint32_t)TIMR2):
		if(match_int_en) NVIC_EnableIRQ(GPIOB0_GPIOA8_TIMR2_IRQn);
		break;
	
	case ((uint32_t)BTIMR0):
		if(match_int_en) NVIC_EnableIRQ(BTIMR0_IRQn);
		break;
	
	case ((uint32_t)BTIMR1):
		if(match_int_en) NVIC_EnableIRQ(BTIMR1_IRQn);
		break;
	
	case ((uint32_t)BTIMR2):
		if(match_int_en) NVIC_EnableIRQ(BTIMR2_IRQn);
		break;
	
	case ((uint32_t)BTIMR3):
		if(match_int_en) NVIC_EnableIRQ(BTIMR3_IRQn);
		break;
	}
}

/*******************************************************************************************************************************
* @brief	Timer output enable
* @param	TIMRx is timer to set, can be TIMR0~2 and BTIMR0~3
* @return
*******************************************************************************************************************************/
void TIMR_OC_OutputEn(TIMR_TypeDef * TIMRx)
{
	TIMRx->OCCR &= ~(TIMR_OCCR_FORCEEN_Pos);
}

/*******************************************************************************************************************************
* @brief	Timer output disable
* @param	TIMRx is timer to set, can be TIMR0~2 and BTIMR0~3
* @param	level is pin keep level after timer output disable
* @return
*******************************************************************************************************************************/
void TIMR_OC_OutputDis(TIMR_TypeDef * TIMRx, uint32_t level)
{
	if(level) TIMRx->OCCR |=  (1 << TIMR_OCCR_FORCELVL_Pos);
	else      TIMRx->OCCR &= ~(1 << TIMR_OCCR_FORCELVL_Pos);
	
	TIMRx->OCCR |= (TIMR_OCCR_FORCEEN_Pos);
}

/*******************************************************************************************************************************
* @brief	Timer match value set
* @param	TIMRx is timer to set, can be TIMR0~2 and BTIMR0~3
* @param	match is match value
* @return
*******************************************************************************************************************************/
void TIMR_OC_SetMatch(TIMR_TypeDef * TIMRx, uint32_t match)
{
	TIMRx->OCMAT = match;
}

/*******************************************************************************************************************************
* @brief	Timer match value query
* @param	TIMRx is timer to query, can be TIMR0~2 and BTIMR0~3
* @return	match value
*******************************************************************************************************************************/
uint32_t TIMR_OC_GetMatch(TIMR_TypeDef * TIMRx)
{
	return TIMRx->OCMAT;
}


/*******************************************************************************************************************************
* @brief	Timer input capture init
* @param	TIMRx is timer to init, can be TIMR0~2
* @param	captureH_int_en is high level capture interrupt enable
* @param	captureL_int_en is low level capture interrupt enable
* @return
*******************************************************************************************************************************/
void TIMR_IC_Init(TIMR_TypeDef * TIMRx, uint32_t captureH_int_en, uint32_t captureL_int_en)
{
	TIMRx->IF = (TIMR_IF_ICR_Msk | TIMR_IF_ICF_Msk);
	if(captureH_int_en) TIMRx->IE |=  (1 << TIMR_IE_ICF_Pos);
	else                TIMRx->IE &= ~(1 << TIMR_IE_ICF_Pos);
	if(captureL_int_en) TIMRx->IE |=  (1 << TIMR_IE_ICR_Pos);
	else                TIMRx->IE &= ~(1 << TIMR_IE_ICR_Pos);
	
	switch((uint32_t)TIMRx)
	{
	case ((uint32_t)TIMR0):
		if(captureH_int_en | captureL_int_en) NVIC_EnableIRQ(TIMR0_IRQn);
		break;
	
	case ((uint32_t)TIMR1):
		if(captureH_int_en | captureL_int_en) NVIC_EnableIRQ(TIMR1_IRQn);
		break;
	
	case ((uint32_t)TIMR2):
		if(captureH_int_en | captureL_int_en) NVIC_EnableIRQ(GPIOB0_GPIOA8_TIMR2_IRQn);
		break;
	}
}

/*******************************************************************************************************************************
* @brief	Timer input capture high level length result query
* @param	TIMRx is timer to init, can be TIMR0~2
* @return	high level length
*******************************************************************************************************************************/
uint32_t TIMR_IC_GetCaptureH(TIMR_TypeDef * TIMRx)
{
	return TIMRx->ICHIGH;
}

/*******************************************************************************************************************************
* @brief	Timer input capture low level length result query
* @param	TIMRx is timer to init, can be TIMR0~2
* @return	low level length
*******************************************************************************************************************************/
uint32_t TIMR_IC_GetCaptureL(TIMR_TypeDef * TIMRx)
{
	return TIMRx->ICLOW;
}
