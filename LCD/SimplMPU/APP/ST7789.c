#include "SWM221.h"

#include "ST7789.h"


#define LCD_VPIX  320
#define LCD_HPIX  240


/****************************************************************************************************************************************** 
* 函数名称:	ST7789_Init()
* 功能说明: TFT液晶屏初始化，TFT使用ST7789驱动，分辨率320*240，使用 ZJY320P1600TG11 测试
* 输    入: 无
* 输    出: 无
* 注意事项: 无
******************************************************************************************************************************************/
void ST7789_Init(void)
{
	MPU_InitStructure MPU_initStruct;
	
	PORT_Init(PORTA, PIN8,  PORTA_PIN8_MPU_D0,  1);
	PORT_Init(PORTA, PIN9,  PORTA_PIN9_MPU_D1,  1);
	PORT_Init(PORTA, PIN10, PORTA_PIN10_MPU_D2, 1);
	PORT_Init(PORTA, PIN11, PORTA_PIN11_MPU_D3, 1);
	PORT_Init(PORTA, PIN12, PORTA_PIN12_MPU_D4, 1);
	PORT_Init(PORTA, PIN13, PORTA_PIN13_MPU_D5, 1);
	PORT_Init(PORTA, PIN14, PORTA_PIN14_MPU_D6, 1);
	PORT_Init(PORTA, PIN0,  PORTA_PIN0_MPU_D7,  1);
	
	PORT_Init(PORTB, PIN4,  PORTB_PIN4_MPU_CS,  0);
	PORT_Init(PORTB, PIN5,  PORTB_PIN5_MPU_RS,  0);
	PORT_Init(PORTB, PIN6,  PORTB_PIN6_MPU_WR,  0);
	PORT_Init(PORTB, PIN9,  PORTB_PIN9_MPU_RD,  0);
	
	MPU_initStruct.RDHoldTime = 2;
	MPU_initStruct.WRHoldTime = 2;
	MPU_initStruct.CSFall_WRFall = 1;
	MPU_initStruct.WRRise_CSRise = 1;
	MPU_initStruct.RDCSRise_Fall = 2;
	MPU_initStruct.WRCSRise_Fall = 2;
	MPU_Init(MPU, &MPU_initStruct);
	
	GPIO_Init(GPIOB, PIN2, 1, 0, 0, 0);		//屏幕背光
	GPIO_SetBit(GPIOB, PIN2);
	GPIO_Init(GPIOB, PIN3, 1, 0, 0, 0);		//屏幕复位
	GPIO_ClrBit(GPIOB, PIN3);
	for(int i = 0; i < 1000000; i++) __NOP();
	GPIO_SetBit(GPIOB, PIN3);
	for(int i = 0; i < 1000000; i++) __NOP();
	
	MPU_WR_REG(MPU, 0x04);
	
	uint32_t id = MPU->DR;
	
	id =  MPU->DR;
	id = (MPU->DR <<  8) | id;
	id = (MPU->DR << 16) | id;
	
	MPU_WR_REG(MPU, 0x11);		// Sleep out
	
	MPU_WR_REG(MPU, 0x36);		// Memory Access Control
	MPU_WR_DATA(MPU, 0x00);
	
	MPU_WR_REG(MPU, 0x3A);		// Interface pixel format
	MPU_WR_DATA(MPU, 0x05);
	
	//--------------------------------ST7789S Frame rate setting-------------------------
	MPU_WR_REG(MPU, 0xb2);
	MPU_WR_DATA(MPU, 0x0c);
	MPU_WR_DATA(MPU, 0x0c);
	MPU_WR_DATA(MPU, 0x00);
	MPU_WR_DATA(MPU, 0x33);
	MPU_WR_DATA(MPU, 0x33);
	
	MPU_WR_REG(MPU, 0xb7);
	MPU_WR_DATA(MPU, 0x35);
	
	//---------------------------------ST7789S Power setting-----------------------------
	MPU_WR_REG(MPU, 0xbb);
	MPU_WR_DATA(MPU, 0x20);
	
	MPU_WR_REG(MPU, 0xc0);
	MPU_WR_DATA(MPU, 0x2c);
	
	MPU_WR_REG(MPU,0xc2);
	MPU_WR_DATA(MPU, 0x01);
	
	MPU_WR_REG(MPU, 0xc3);
	MPU_WR_DATA(MPU, 0x0b);
	
	MPU_WR_REG(MPU, 0xc4);
	MPU_WR_DATA(MPU, 0x20);
	
	MPU_WR_REG(MPU, 0xc6);
	MPU_WR_DATA(MPU, 0x0f);
	
	MPU_WR_REG(MPU, 0xd0);
	MPU_WR_DATA(MPU, 0xa4);
	MPU_WR_DATA(MPU, 0xa1);

	//--------------------------------ST7789S Gamma setting------------------------------
	MPU_WR_REG(MPU, 0xe0);
	MPU_WR_DATA(MPU, 0xd0);
	MPU_WR_DATA(MPU, 0x03);
	MPU_WR_DATA(MPU, 0x09);
	MPU_WR_DATA(MPU, 0x0e);
	MPU_WR_DATA(MPU, 0x11);
	MPU_WR_DATA(MPU, 0x3d);
	MPU_WR_DATA(MPU, 0x47);
	MPU_WR_DATA(MPU, 0x55);
	MPU_WR_DATA(MPU, 0x53);
	MPU_WR_DATA(MPU, 0X1A);
	MPU_WR_DATA(MPU, 0x16);
	MPU_WR_DATA(MPU, 0x14);
	MPU_WR_DATA(MPU, 0x1F);
	MPU_WR_DATA(MPU, 0x22);
	
	MPU_WR_REG(MPU, 0xe1);
	MPU_WR_DATA(MPU, 0xd0);
	MPU_WR_DATA(MPU, 0x02);
	MPU_WR_DATA(MPU, 0x08);
	MPU_WR_DATA(MPU, 0x0D);
	MPU_WR_DATA(MPU, 0x12);
	MPU_WR_DATA(MPU, 0x2c);
	MPU_WR_DATA(MPU, 0x43);
	MPU_WR_DATA(MPU, 0x55);
	MPU_WR_DATA(MPU, 0x53);
	MPU_WR_DATA(MPU, 0x1E);
	MPU_WR_DATA(MPU, 0x1B);
	MPU_WR_DATA(MPU, 0x19);
	MPU_WR_DATA(MPU, 0x20);
	MPU_WR_DATA(MPU, 0x22);
	
	MPU_WR_REG(MPU, 0x29);
}


/****************************************************************************************************************************************** 
* 函数名称: ST7789_SetCursor()
* 功能说明: 
* 输    入: 
* 输    出: 
* 注意事项: 
******************************************************************************************************************************************/
void ST7789_SetCursor(uint16_t x, uint16_t y)
{
	MPU_WR_REG(MPU, 0x2A);
	MPU_WR_DATA(MPU, x >> 8);
	MPU_WR_DATA(MPU, x & 0xFF);
	
	MPU_WR_REG(MPU, 0x2B);
	MPU_WR_DATA(MPU, y >> 8);
	MPU_WR_DATA(MPU, y & 0xFF);
}

/****************************************************************************************************************************************** 
* 函数名称: ST7789_DrawPoint()
* 功能说明: 
* 输    入: 
* 输    出: 
* 注意事项: 
******************************************************************************************************************************************/
void ST7789_DrawPoint(uint16_t x, uint16_t y, uint16_t rgb)
{
	ST7789_SetCursor(x, y);
	
	MPU_WR_REG(MPU, 0x2C);
	MPU_WR_DATA(MPU, rgb >> 8);
	MPU_WR_DATA(MPU, rgb & 0xFF);
}

/****************************************************************************************************************************************** 
* 函数名称: ST7789_Clear()
* 功能说明: 
* 输    入: 
* 输    出: 
* 注意事项: 
******************************************************************************************************************************************/
void ST7789_Clear(uint16_t rgb)
{
	uint32_t i, j;
	
	ST7789_SetCursor(0, 0);
	
	MPU_WR_REG(MPU, 0x2C);
	
	for(i = 0; i < LCD_VPIX; i++)
	{
		for(j = 0; j < LCD_HPIX; j++)
		{
			MPU_WR_DATA(MPU, rgb >> 8);
			MPU_WR_DATA(MPU, rgb & 0xFF);
		}
	}
}

static uint32_t MPUDMA_Color;
/****************************************************************************************************************************************** 
* 函数名称: ST7789_DMAClear()
* 功能说明: 
* 输    入: 
* 输    出: 
* 注意事项: 
******************************************************************************************************************************************/
void ST7789_DMAClear(uint16_t color)
{
	DMA_InitStructure DMA_initStruct;
	
	MPUDMA_Color = color;
	
	ST7789_SetCursor(0, 0);
	
	DMA_initStruct.Mode = DMA_MODE_SINGLE;
	DMA_initStruct.Unit = DMA_UNIT_HALFWORD;
	DMA_initStruct.Count = LCD_HPIX * LCD_VPIX;
	DMA_initStruct.MemoryAddr = (uint32_t)&MPUDMA_Color;
	DMA_initStruct.MemoryAddrInc = 0;
	DMA_initStruct.PeripheralAddr = (uint32_t)&MPU->DR;
	DMA_initStruct.PeripheralAddrInc = 0;
	DMA_initStruct.Handshake = DMA_CH1_MPUTX;
	DMA_initStruct.Priority = DMA_PRI_LOW;
	DMA_initStruct.INTEn = 0;
	DMA_CH_Init(DMA_CH1, &DMA_initStruct);
	DMA_CH_Open(DMA_CH1);
	
	MPU->SR = MPU_SR_DMAEN_Msk;
	
	MPU->IR = 0x2C;
}


uint32_t ST7789_DMADone(void)
{
	if(DMA_CH_INTStat(DMA_CH1, DMA_IT_DONE))
	{
		DMA_CH_INTClr(DMA_CH1, DMA_IT_DONE);
		
		MPU->SR = 0;	// 完成 DMA 操作后要清除DMA使能
		
		return 1;
	}
	else
	{
		return 0;
	}
}
