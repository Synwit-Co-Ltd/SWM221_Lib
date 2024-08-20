/****************************************************************************************************************************************** 
* 文件名称:	SWM221_flash.c
* 功能说明:	使用芯片的IAP功能将片上Flash模拟成EEPROM来保存数据，掉电后不丢失
* 技术支持:	http://www.synwit.com.cn/e/tool/gbook/?bid=1
* 注意事项:
* 版本日期: V1.0.0		2016年1月30日
* 升级记录: 
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
#include "SWM221.h"
#include "SWM221_flash.h"


typedef int  (*IAP_Flash_Param_t)(uint32_t param, uint32_t flag);
typedef int  (*IAP_Flash_Erase_t)(uint32_t sector, uint32_t flag);
typedef int  (*IAP_Flash_Write_t)(uint32_t flash_addr, uint32_t ram_addr, uint32_t count, uint32_t flag);


IAP_Flash_Param_t IAP_Flash_Param = (IAP_Flash_Param_t)0x010004D1;
IAP_Flash_Erase_t IAP_Flash_Erase = (IAP_Flash_Erase_t)0x01000401;
IAP_Flash_Write_t IAP_Flash_Write = (IAP_Flash_Write_t)0x01000461;


/****************************************************************************************************************************************** 
* 函数名称: FLASH_Erase()
* 功能说明:	片内Flash擦除
* 输    入: uint32_t addr		擦除地址，扇区大小为512 Byte
* 输    出: uint32_t			FLASH_RES_OK、FLASH_RES_TO、FLASH_RES_ERR
* 注意事项: 无
******************************************************************************************************************************************/
uint32_t FLASH_Erase(uint32_t addr)
{
	__disable_irq();
	
	IAP_Flash_Erase(addr / 0x200, 0x0B11FFAC);
	
	FMC->CACHE |= FMC_CACHE_CCLR_Msk;
	
	__enable_irq();
	
	return FLASH_RES_OK;
}


/****************************************************************************************************************************************** 
* 函数名称: FLASH_Write()
* 功能说明:	片内Flash写入
* 输    入: uint32_t addr		写入地址
*			uint32_t buff[]		要写入的数据
*			uint32_t count		要写入数据的个数，以字为单位，且必须是2的整数倍，即最少写入2个字
* 输    出: uint32_t			FLASH_RES_OK、FLASH_RES_TO、FLASH_RES_ERR
* 注意事项: 写入数据个数必须是2的整数倍，即最少写入2个字
******************************************************************************************************************************************/
uint32_t FLASH_Write(uint32_t addr, uint32_t buff[], uint32_t count)
{
	__disable_irq();
	
	IAP_Flash_Write(addr, (uint32_t)buff, count/2, 0x0B11FFAC);
	
	FMC->CACHE |= FMC_CACHE_CCLR_Msk;
	
	__enable_irq();
	
	return FLASH_RES_OK;
}


/****************************************************************************************************************************************** 
* 函数名称: Flash_Param_at_xMHz()
* 功能说明:	将Flash参数设置成xMHz主频下运行时所需的参数
* 输    入: uint32_t x		系统主频，单位MHz
* 输    出: 无
* 注意事项: 无
******************************************************************************************************************************************/
void Flash_Param_at_xMHz(uint32_t x)
{
	__disable_irq();
	
	IAP_Flash_Param(1000 / x, 0x0B11FFAC);
	
	__enable_irq();
}
