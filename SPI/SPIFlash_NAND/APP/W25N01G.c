#include "SWM221.h"
#include "W25N01G.h"


/****************************************************************************************************************************************** 
* 函数名称:	W25N01G_Init()
* 功能说明:	W25N01G 初始化
* 输    入: 无
* 输    出: 无
* 注意事项: 无
******************************************************************************************************************************************/
void W25N01G_Init(void)
{
	SPI_InitStructure SPI_initStruct;
	
	GPIO_Init(GPIOB, PIN15, 1, 0, 0, 0);
#define W25N01G_Assert()	GPIO_ClrBit(GPIOB, PIN15)
#define W25N01G_Deassert()	GPIO_SetBit(GPIOB, PIN15)
	W25N01G_Deassert();
	
	PORT_Init(PORTB, PIN10, PORTB_PIN10_SPI0_SCLK, 0);
	PORT_Init(PORTB, PIN13, PORTB_PIN13_SPI0_MOSI, 0);
	PORT_Init(PORTB, PIN14, PORTB_PIN14_SPI0_MISO, 1);

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
	
	uint8_t reg = W25N01G_ReadReg(W25N_STATUS_REG2);
	
	reg &= ~(1 << W25N_STATUS_REG2_BUF_Pos);	// Continuous Read Mode
	
	W25N01G_WriteReg(W25N_STATUS_REG2, reg);
}


/****************************************************************************************************************************************** 
* 函数名称:	W25N01G_ReadJEDEC()
* 功能说明:	W25N01G 读取 JEDEC ID
* 输    入: 无
* 输    出: 无
* 注意事项: 无
******************************************************************************************************************************************/
uint32_t W25N01G_ReadJEDEC(void)
{
	uint8_t manufacture_id;
	uint16_t device_id;
	
	W25N01G_Assert();
	
	SPI_ReadWrite(SPI0, W25N_CMD_READ_JEDEC);
	SPI_ReadWrite(SPI0, 0x00);
	
	manufacture_id = SPI_ReadWrite(SPI0, 0xFF);
	device_id      = SPI_ReadWrite(SPI0, 0xFF) << 8;
	device_id     |= SPI_ReadWrite(SPI0, 0xFF);
	
	W25N01G_Deassert();
	
	return (manufacture_id << 16) | device_id;
}


/****************************************************************************************************************************************** 
* 函数名称:	W25N01G_Erase()
* 功能说明:	W25N01G 块擦除，块大小为 128KB
* 输    入: uint32_t addr			要擦除的 SPI Flash 地址，必须 128KB 对齐（即 addr 是 0x20000 的整数倍）
*			uint8_t wait			是否等待 SPI Flash 完成操作操作，1 等待完成   0 立即返回
* 输    出: 无
* 注意事项: 无
******************************************************************************************************************************************/
void W25N01G_Erase(uint32_t addr, uint8_t wait)
{
	uint32_t page_addr = addr >> 12;
	
	W25N01G_Assert();
	
	SPI_ReadWrite(SPI0, W25N_CMD_WRITE_ENABLE);
	
	W25N01G_Deassert();
	
	W25N01G_Assert();
	
	SPI_ReadWrite(SPI0, W25N_CMD_ERASE_BLOCK128KB);
	SPI_ReadWrite(SPI0, (page_addr >> 16) & 0xFF);
	SPI_ReadWrite(SPI0, (page_addr >>  8) & 0xFF);
	SPI_ReadWrite(SPI0, (page_addr >>  0) & 0xFF);
	
	W25N01G_Deassert();
	
	if(wait)
		while(W25N01G_FlashBusy()) __NOP();
}


/****************************************************************************************************************************************** 
* 函数名称:	W25N01G_Write()
* 功能说明:	W25N01G 页写入，页大小为 2112 Byte
* 输    入: uint32_t addr			要写入到的 SPI Flash 地址，必须是 0x1000（即 4096）的整数倍
*			uint8_t buff[2048]		要写入 SPI Flash 的数据，数组大小必须是 2048 字节，ECC 区域内容由硬件计算生成
* 输    出: 无
* 注意事项: 无
******************************************************************************************************************************************/
void W25N01G_Write(uint32_t addr, uint8_t buff[2048])
{
	uint32_t page_addr = addr >> 12;
	uint16_t column_addr = 0x00;
	
	W25N01G_Assert();
	
	SPI_ReadWrite(SPI0, W25N_CMD_WRITE_ENABLE);
	
	W25N01G_Deassert();
	
	/* Load the program data into the Data Buffer */
	W25N01G_Assert();
	
	SPI_ReadWrite(SPI0, W25N_CMD_PAGE_PROGRAM);
	SPI_ReadWrite(SPI0, (column_addr >> 8) & 0xFF);
	SPI_ReadWrite(SPI0, (column_addr >> 0) & 0xFF);
	
	for(int i = 0; i < 2048; i++)
	{
		SPI_ReadWrite(SPI0, buff[i]);
	}
	
	W25N01G_Deassert();
	
	/* Program the Data Buffer content into the physical memory page */
	W25N01G_Assert();
	
	SPI_ReadWrite(SPI0, W25N_CMD_PROGRAM_EXECUTE);
	SPI_ReadWrite(SPI0, (page_addr >> 16) & 0xFF);
	SPI_ReadWrite(SPI0, (page_addr >>  8) & 0xFF);
	SPI_ReadWrite(SPI0, (page_addr >>  0) & 0xFF);
	
	W25N01G_Deassert();
	
	while(W25N01G_FlashBusy()) __NOP();
}


/****************************************************************************************************************************************** 
* 函数名称:	W25N01G_Read()
* 功能说明:	W25N01G 页读取，页大小为 2112 Byte
* 输    入: uint32_t addr			要读取自的 SPI Flash 地址，必须是 0x1000（即 4096）的整数倍
*			uint8_t buff[2048]		读取到的数据写入此数组中，数组大小必须是 2048 字节，ECC 数据不读取
* 输    出: 无
* 注意事项: 无
******************************************************************************************************************************************/
void W25N01G_Read(uint32_t addr, uint8_t buff[2048])
{
	uint32_t page_addr = addr >> 12;
	
	/* Transfer the data of specified page into the 2112-Byte Data Buffer */
	W25N01G_Assert();
	
	SPI_ReadWrite(SPI0, W25N_CMD_PAGE_READ);
	SPI_ReadWrite(SPI0, (page_addr >> 16) & 0xFF);
	SPI_ReadWrite(SPI0, (page_addr >>  8) & 0xFF);
	SPI_ReadWrite(SPI0, (page_addr >>  0) & 0xFF);
	
	W25N01G_Deassert();
	
	while(W25N01G_FlashBusy()) __NOP();
	
	W25N01G_Assert();
	
	SPI_ReadWrite(SPI0, W25N_CMD_FAST_READ);
	SPI_ReadWrite(SPI0, 0x00);
	SPI_ReadWrite(SPI0, 0x00);
	SPI_ReadWrite(SPI0, 0x00);
	SPI_ReadWrite(SPI0, 0x00);
	
	for(int i = 0; i < 2048; i++)
	{
		buff[i] = SPI_ReadWrite(SPI0, 0xFF);
	}
	
	W25N01G_Deassert();
}


/****************************************************************************************************************************************** 
* 函数名称:	W25N01G_FlashBusy()
* 功能说明:	W25N01G 忙检测
* 输    入: 无
* 输    出: 无
* 注意事项: 无
******************************************************************************************************************************************/
bool W25N01G_FlashBusy(void)
{
	uint8_t reg = W25N01G_ReadReg(W25N_STATUS_REG3);
	
	bool busy = (reg & (1 << W25N_STATUS_REG3_BUSY_Pos));
	
	return busy;
}


/****************************************************************************************************************************************** 
* 函数名称:	W25N01G_FlashProtect()
* 功能说明:	W25N01G 写保护配置
* 输    入: 无
* 输    出: 无
* 注意事项: 无
******************************************************************************************************************************************/
void W25N01G_FlashProtect(uint8_t protect)
{
	uint8_t tb = (protect >> 4);
	uint8_t bp = (protect & 0xF);
	
	uint8_t reg = W25N01G_ReadReg(W25N_STATUS_REG1);
	
	reg &= ~(W25N_STATUS_REG1_TB_Msk | W25N_STATUS_REG1_BP_Msk | W25N_STATUS_REG1_WPE_Msk);
	reg |= (tb << W25N_STATUS_REG1_TB_Pos) |
		   (bp << W25N_STATUS_REG1_BP_Pos);
	
	W25N01G_WriteReg(W25N_STATUS_REG1, reg);
}


void W25N01G_WriteReg(uint8_t reg_addr, uint8_t data)
{	
	W25N01G_Assert();
	
	SPI_ReadWrite(SPI0, W25N_CMD_WRITE_STATUS_REG);
	SPI_ReadWrite(SPI0, reg_addr);
	SPI_ReadWrite(SPI0, data);
	
	W25N01G_Deassert();
}


uint8_t W25N01G_ReadReg(uint8_t reg_addr)
{
	W25N01G_Assert();
	
	SPI_ReadWrite(SPI0, W25N_CMD_READ_STATUS_REG);
	SPI_ReadWrite(SPI0, reg_addr);
	uint8_t data = SPI_ReadWrite(SPI0, 0xFF);
	
	W25N01G_Deassert();
	
	return data;
}
