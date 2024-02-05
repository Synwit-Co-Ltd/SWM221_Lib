/****************************************************************************************************************************************** 
* 文件名称:	GD25Q21.c
* 功能说明:	GD25Q21 SPI Flash 驱动程序
* 技术支持:	http://www.synwit.com.cn/e/tool/gbook/?bid=1
* 注意事项:
* 版本日期: V1.0.0		2017年10月26日
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

#include "GD25Q21.h"


/****************************************************************************************************************************************** 
* 函数名称:	GD25Q21_Init()
* 功能说明:	初始化GD25Q21使用的SPI接口，并读取芯片ID以判断其是否是GD25Q21芯片
* 输    入: 无
* 输    出: 无
* 注意事项: 无
******************************************************************************************************************************************/
void GD25Q21_Init(void)
{
	uint16_t chip_id;
	uint8_t manufacture_id, device_id;
	SPI_InitStructure SPI_initStruct;
	
	GPIO_Init(GPIOB, PIN15, 1, 0, 0, 0);
#define GD25Q21_Assert()	GPIO_ClrBit(GPIOB, PIN15)
#define GD25Q21_Deassert()	GPIO_SetBit(GPIOB, PIN15)
	GD25Q21_Deassert();
	
	PORT_Init(PORTM, PIN8,  PORTM_PIN8_SPI0_SCLK,  0);
	PORT_Init(PORTM, PIN9,  PORTM_PIN9_SPI0_MOSI,  1);	// 注意：MOSI的输入使能必须开启，因为QSPI模式下需要使用此线读取
	PORT_Init(PORTM, PIN10, PORTM_PIN10_SPI0_MISO, 1);
	
	/* for QSPI */
	PORT_Init(PORTB, PIN0,  PORTB_PIN0_SPI0_DATA2, 1);
	PORT_Init(PORTB, PIN1,  PORTB_PIN1_SPI0_DATA3, 1);
	
	SPI_initStruct.clkDiv = SPI_CLKDIV_32;
	SPI_initStruct.FrameFormat = SPI_FORMAT_SPI;
	SPI_initStruct.SampleEdge = SPI_FIRST_EDGE;
	SPI_initStruct.IdleLevel = SPI_LOW_LEVEL;
	SPI_initStruct.WordSize = 8;
	SPI_initStruct.Master = 1;
	SPI_initStruct.RXThreshold = 0;
	SPI_initStruct.RXThresholdIEn = 0;
	SPI_initStruct.TXThreshold = 0;
	SPI_initStruct.TXThresholdIEn = 0;
	SPI_initStruct.TXCompleteIEn = 0;
	SPI_Init(SPI0, &SPI_initStruct);
	
	SPI_Open(SPI0);
	
	chip_id = GD25Q21_ReadChipID();
	manufacture_id = chip_id >> 8;
	device_id = chip_id & 0xFF;
	
	if((manufacture_id == GD25Q21_MANUFACTOR_ID) && (device_id == GD25Q21_DEVICE_ID))
	{
		printf("GD25Q21 found\r\n");
		
		GD25Q21_QuadSwitch(1);
		while(GD25Q21_IsBusy()) __NOP();	// 等待写入操作完成
	}
	else
	{
		printf("Manufactor ID: %02X, Device ID: %02X, fail to identify\r\n", manufacture_id, device_id);
	}
}

/****************************************************************************************************************************************** 
* 函数名称:	GD25Q21_ReadChipID()
* 功能说明:	读取芯片ID
* 输    入: 无
* 输    出: uint16_t			芯片ID，共16位，高8位为厂商ID，低8位为Device ID
* 注意事项: 无
******************************************************************************************************************************************/
uint16_t GD25Q21_ReadChipID(void)
{
	uint8_t manufacture_id, device_id;
	
	GD25Q21_Assert();
	
	SPI_ReadWrite(SPI0, GD25Q21_CMD_READ_CHIP_ID);
	SPI_ReadWrite(SPI0, 0x00);
	SPI_ReadWrite(SPI0, 0x00);
	SPI_ReadWrite(SPI0, 0x00);
	
	manufacture_id = SPI_ReadWrite(SPI0, 0xFF);
	device_id      = SPI_ReadWrite(SPI0, 0xFF);
	
	GD25Q21_Deassert();
	
	return (manufacture_id << 8) | device_id;
}

/****************************************************************************************************************************************** 
* 函数名称:	GD25Q21_ReadStatus()
* 功能说明:	读取芯片状态寄存器
* 输    入: 无
* 输    出: uint16_t			芯片状态
* 注意事项: 无
******************************************************************************************************************************************/
uint16_t GD25Q21_ReadStatus(void)
{
	uint16_t status;
	
	GD25Q21_Assert();
		
	SPI_ReadWrite(SPI0, GD25Q21_CMD_READ_STATUS_L);
	status = SPI_ReadWrite(SPI0, 0xFF);
	
	GD25Q21_Deassert();
	
	GD25Q21_Assert();
	
	SPI_ReadWrite(SPI0, GD25Q21_CMD_READ_STATUS_H);
	status |= SPI_ReadWrite(SPI0, 0xFF) << 8;
	
	GD25Q21_Deassert();
		
	return status;
}

/****************************************************************************************************************************************** 
* 函数名称:	GD25Q21_IsBusy()
* 功能说明:	芯片忙检测，执行擦除、写入操作后需要执行忙检测以判断芯片是否已经完成操作
* 输    入: 无
* 输    出: uint32_t			1 芯片正在执行内部擦除、写入操作    0 芯片未在执行内部操作
* 注意事项: 无
******************************************************************************************************************************************/
uint32_t GD25Q21_IsBusy(void)
{
	return (GD25Q21_ReadStatus() & GD25Q21_STATUS_WIP_Msk) ? 1 : 0;
}

/****************************************************************************************************************************************** 
* 函数名称:	GD25Q21_QuadSwitch()
* 功能说明:	4线读开关
* 输    入: uint32_t			1 开启4线读写   0 关闭4线读写
* 输    出: uint16_t			芯片状态
* 注意事项: 无
******************************************************************************************************************************************/
void GD25Q21_QuadSwitch(uint32_t on)
{
	uint16_t status = GD25Q21_ReadStatus();
	
	if(on)
		status |=  (1 << GD25Q21_STATUS_QE_Pos);
	else
		status &= ~(1 << GD25Q21_STATUS_QE_Pos);
	
	GD25Q21_Assert();
	
	SPI_ReadWrite(SPI0, GD25Q21_CMD_WRITE_ENABLE);
	
	GD25Q21_Deassert();
	
	GD25Q21_Assert();
	
	SPI_ReadWrite(SPI0, GD25Q21_CMD_WRITE_STATUS);
	SPI_ReadWrite(SPI0, (status >>  0) & 0xFF);
	SPI_ReadWrite(SPI0, (status >>  8) & 0xFF);
	
	GD25Q21_Deassert();
}

/****************************************************************************************************************************************** 
* 函数名称:	GD25Q21_EraseSector()
* 功能说明:	扇区擦除
* 输    入: uint32_t addr		要擦除的扇区的地址，每扇区4K字节
* 输    出: 无
* 注意事项: 无
******************************************************************************************************************************************/
void GD25Q21_EraseSector(uint32_t addr)
{
	GD25Q21_Assert();
	
	SPI_ReadWrite(SPI0, GD25Q21_CMD_WRITE_ENABLE);
	
	GD25Q21_Deassert();
	
	GD25Q21_Assert();
		
	SPI_ReadWrite(SPI0, GD25Q21_CMD_ERASE_SECTOR);
	SPI_ReadWrite(SPI0, (addr >> 16) & 0xFF);
	SPI_ReadWrite(SPI0, (addr >>  8) & 0xFF);
	SPI_ReadWrite(SPI0, (addr >>  0) & 0xFF);
	
	GD25Q21_Deassert();
}

/****************************************************************************************************************************************** 
* 函数名称:	GD25Q21_ReadData()
* 功能说明:	从GD25Q21指定地址读取指定个字节数据
* 输    入: uint32_t addr		要读取的数据在GD25Q21上的地址
*			uint8_t buff[]		读取到的数据存储在buff中
*			uint32_t cnt		要读取数据的字节个数
* 输    出: 无
* 注意事项: 无
******************************************************************************************************************************************/
void GD25Q21_ReadData(uint32_t addr, uint8_t buff[], uint32_t cnt)
{
	uint32_t i;
	
	GD25Q21_Assert();
	
	SPI_ReadWrite(SPI0, GD25Q21_CMD_READ_DATA);
	SPI_ReadWrite(SPI0, (addr >> 16) & 0xFF);
	SPI_ReadWrite(SPI0, (addr >>  8) & 0xFF);
	SPI_ReadWrite(SPI0, (addr >>  0) & 0xFF);
	
	for(i = 0; i < cnt; i++)
	{
		buff[i] = SPI_ReadWrite(SPI0, 0xFF);
	}
	
	GD25Q21_Deassert();
}

#ifdef CHIP_SWM221
/****************************************************************************************************************************************** 
* 函数名称:	GD25Q21_ReadData_4bit()
* 功能说明:	从GD25Q21指定地址读取指定个字节数据，读取时使用4线读取
* 输    入: uint32_t addr		要读取的数据在GD25Q21上的地址
*			uint8_t buff[]		读取到的数据存储在buff中
*			uint32_t cnt		要读取数据的字节个数
* 输    出: 无
* 注意事项: 每次最多读1024个字节
******************************************************************************************************************************************/
void GD25Q21_ReadData_4bit(uint32_t addr, uint8_t buff[], uint32_t cnt)
{
	uint32_t i;
	
	SPI_Close(SPI0);
	SPI0->CTRL &= ~SPI_CTRL_FFS_Msk;
	SPI0->CTRL |= (3 << SPI_CTRL_FFS_Pos);
	SPI0->SPIMCR = (1 << SPI_SPIMCR_EN_Pos) |
				   ((cnt - 1) << SPI_SPIMCR_RDLEN_Pos) |
				   ((8 - 1)   << SPI_SPIMCR_DUMMY_Pos);
	SPI_Open(SPI0);
	
	GD25Q21_Assert();
	
	SPI_Write(SPI0, (GD25Q21_CMD_READ_DATA_4bit << 24) | (addr & 0xFFFFFF));
	while(SPI_IsRXEmpty(SPI0)) __NOP();
	buff[0] = SPI_Read(SPI0);
	
	for(i = 0; i < cnt; i++)
	{
		while(SPI_IsRXEmpty(SPI0)) __NOP();
		
		buff[i] = SPI_Read(SPI0);
	}
	
	GD25Q21_Deassert();
	
	SPI_Close(SPI0);
	SPI0->SPIMCR = 0;	// 必须先清除SPIMCR.EN，再改变CTRL.FFS
	SPI0->CTRL &= ~SPI_CTRL_FFS_Msk;
	SPI0->CTRL |= (0 << SPI_CTRL_FFS_Pos);
	SPI_Open(SPI0);
}

/****************************************************************************************************************************************** 
* 函数名称:	GD25Q21_ReadData_4bit_DMA()
* 功能说明: 同 GD25Q21_ReadData_4bit，但使用 DMA 读取SPI数据寄存器，可避免软件读取不及时导致的数据丢失
* 输    入: 同 GD25Q21_ReadData_4bit
* 输    出: 无
* 注意事项: 每次最多读1024个字节
******************************************************************************************************************************************/
void GD25Q21_ReadData_4bit_DMA(uint32_t addr, uint8_t buff[], uint32_t cnt)
{
	DMA_InitStructure DMA_initStruct;
	
	// SPI0 RX DMA
    DMA_initStruct.Mode = DMA_MODE_SINGLE;
    DMA_initStruct.Unit = DMA_UNIT_BYTE;
    DMA_initStruct.Count = cnt;
    DMA_initStruct.SrcAddr = (uint32_t)&SPI0->DATA;
    DMA_initStruct.SrcAddrInc = 0;
    DMA_initStruct.DstAddr = (uint32_t)buff;
    DMA_initStruct.DstAddrInc = 1;
    DMA_initStruct.Handshake = DMA_CH1_SPI0RX;
    DMA_initStruct.Priority = DMA_PRI_LOW;
    DMA_initStruct.INTEn = 0;
    DMA_CH_Init(DMA_CH1, &DMA_initStruct);

    DMA_CH_Open(DMA_CH1);
	
	
	SPI_Close(SPI0);
	SPI0->CTRL &= ~SPI_CTRL_FFS_Msk;
	SPI0->CTRL |= (3 << SPI_CTRL_FFS_Pos);
	SPI0->SPIMCR = (1 << SPI_SPIMCR_EN_Pos) |
				   ((cnt - 1) << SPI_SPIMCR_RDLEN_Pos) |
				   ((8 - 1)   << SPI_SPIMCR_DUMMY_Pos);
	SPI_Open(SPI0);
	
	GD25Q21_Assert();
	
	__disable_irq();
	
	SPI_Write(SPI0, (GD25Q21_CMD_READ_DATA_4bit << 24) | (addr & 0xFFFFFF));
	while(SPI_IsRXEmpty(SPI0)) __NOP();
	buff[0] = SPI_Read(SPI0);
	
	SPI0->CTRL |= (1 << SPI_CTRL_DMARXEN_Pos);
	
	__enable_irq();
	
	while(DMA_CH_INTStat(DMA_CH1, DMA_IT_DONE) == 0) __NOP();
	DMA_CH_INTClr(DMA_CH1, DMA_IT_DONE);
	
	GD25Q21_Deassert();
	
	SPI0->CTRL &= ~(1 << SPI_CTRL_DMARXEN_Pos);
	
	SPI_Close(SPI0);
	SPI0->SPIMCR = 0;
	SPI0->CTRL &= ~SPI_CTRL_FFS_Msk;
	SPI0->CTRL |= (0 << SPI_CTRL_FFS_Pos);
	SPI_Open(SPI0);
}
#endif

/****************************************************************************************************************************************** 
* 函数名称:	GD25Q21_WritePage()
* 功能说明:	向GD25Q21指定地址处写入指定个字节数据
* 输    入: uint32_t addr		数据要写入到GD25Q21的位置
*			uint8_t data[]		要写入的数据
*			uint16_t cnt		要写入的数据个数，每页256字节，每次写入的数据必须在同一页的地址范围内
* 输    出: 无
* 注意事项: 无
******************************************************************************************************************************************/
void GD25Q21_WritePage(uint32_t addr, uint8_t data[], uint16_t cnt)
{
	uint32_t i;
	
	GD25Q21_Assert();
	
	SPI_ReadWrite(SPI0, GD25Q21_CMD_WRITE_ENABLE);
	
	GD25Q21_Deassert();
	
	GD25Q21_Assert();
	
	SPI_ReadWrite(SPI0, GD25Q21_CMD_WRITE_PAGE);
	SPI_ReadWrite(SPI0, (addr >> 16) & 0xFF);
	SPI_ReadWrite(SPI0, (addr >>  8) & 0xFF);
	SPI_ReadWrite(SPI0, (addr >>  0) & 0xFF);
	
	for(i = 0; i < cnt; i++)
	{
		SPI_ReadWrite(SPI0, data[i]);
	}
	
	GD25Q21_Deassert();
}
