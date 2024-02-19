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
	QSPI_InitStructure QSPI_initStruct;
	
	PORT_Init(PORTC, PIN3,  PORTC_PIN3_QSPI0_SCLK,  0);
	PORT_Init(PORTC, PIN8,  PORTC_PIN2_QSPI0_SSEL,  0);
	PORT_Init(PORTA, PIN15, PORTA_PIN15_QSPI0_MOSI, 1);
	PORT_Init(PORTB, PIN0,  PORTB_PIN0_QSPI0_MISO,  1);
	PORT_Init(PORTB, PIN1,  PORTB_PIN1_QSPI0_D2,    1);
	PORT_Init(PORTB, PIN2,  PORTB_PIN2_QSPI0_D3,    1);
	
	QSPI_initStruct.Size = QSPI_Size_128MB;
	QSPI_initStruct.ClkDiv = 4;
	QSPI_initStruct.ClkMode = QSPI_ClkMode_3;
	QSPI_initStruct.SampleShift = QSPI_SampleShift_NONE;
	QSPI_initStruct.IntEn = 0;
	QSPI_Init(QSPI0, &QSPI_initStruct);
	QSPI_Open(QSPI0);
	
	uint8_t reg = W25N01G_ReadReg(W25N_STATUS_REG2);
	
	reg |= (1 << W25N_STATUS_REG2_BUF_Pos);		// Buffer Read Mode
	
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
	QSPI_CmdStructure cmdStruct;
	QSPI_CmdStructClear(&cmdStruct);
	
	cmdStruct.InstructionMode 	 = QSPI_PhaseMode_1bit;
	cmdStruct.Instruction 		 = W25N_CMD_READ_JEDEC;
	cmdStruct.AddressMode 		 = QSPI_PhaseMode_None;
	cmdStruct.AlternateBytesMode = QSPI_PhaseMode_None;
	cmdStruct.DummyCycles 		 = 8;
	cmdStruct.DataMode 			 = QSPI_PhaseMode_1bit;
	cmdStruct.DataCount 		 = 3;
	
	QSPI_Command(QSPI0, QSPI_Mode_IndirectRead, &cmdStruct);
	
	while(QSPI_FIFOCount(QSPI0) < 3) __NOP();
	
	return (QSPI0->DRB << 16) | (QSPI0->DRB << 8) | QSPI0->DRB;
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
	QSPI_CmdStructure cmdStruct;
	QSPI_CmdStructClear(&cmdStruct);
	
	cmdStruct.InstructionMode 	 = QSPI_PhaseMode_1bit;
	cmdStruct.Instruction 		 = W25N_CMD_ERASE_BLOCK128KB;
	cmdStruct.AddressMode 		 = QSPI_PhaseMode_1bit;
	cmdStruct.AddressSize		 = QSPI_PhaseSize_24bit;	// 高 8bit 为 dummy clock，低 16bit 为地址
	cmdStruct.Address			 = addr >> 12;				// Page Address
	cmdStruct.AlternateBytesMode = QSPI_PhaseMode_None;
	cmdStruct.DummyCycles 		 = 0;
	cmdStruct.DataMode 			 = QSPI_PhaseMode_None;
	
	QSPI_WriteEnable(QSPI0);
	
	QSPI_Command(QSPI0, QSPI_Mode_IndirectWrite, &cmdStruct);
	
	while(QSPI_Busy(QSPI0)) __NOP();
	
	if(wait)
		while(W25N01G_FlashBusy()) __NOP();
}


/****************************************************************************************************************************************** 
* 函数名称:	W25N01G_Write_()
* 功能说明:	W25N01G 页写入，页大小为 2112 Byte
* 输    入: uint32_t addr			要写入到的 SPI Flash 地址，必须是 0x1000（即 4096）的整数倍
*			uint8_t buff[2048]		要写入 SPI Flash 的数据，数组大小必须是 2048 字节，ECC 区域内容由硬件计算生成
*			uint8_t data_width		写入使用的数据线个数，有效值包括 1、4
* 输    出: 无
* 注意事项: 无
******************************************************************************************************************************************/
void W25N01G_Write_(uint32_t addr, uint8_t buff[2048], uint8_t data_width)
{
	QSPI_CmdStructure cmdStruct;
	QSPI_CmdStructClear(&cmdStruct);
	
	uint8_t instruction, dataMode;
	switch(data_width)
	{
	case 1:
		instruction = W25N_CMD_PAGE_PROGRAM;
		dataMode 	= QSPI_PhaseMode_1bit;
		break;
	
	case 4:
		instruction = W25N_CMD_PAGE_PROGRAM_4bit;
		dataMode 	= QSPI_PhaseMode_4bit;
		break;
	}
	
	cmdStruct.InstructionMode 	 = QSPI_PhaseMode_1bit;
	cmdStruct.Instruction 		 = instruction;
	cmdStruct.AddressMode 		 = QSPI_PhaseMode_1bit;
	cmdStruct.AddressSize 		 = QSPI_PhaseSize_16bit;
	cmdStruct.Address 			 = 0x000;					// Column Address
	cmdStruct.AlternateBytesMode = QSPI_PhaseMode_None;
	cmdStruct.DummyCycles 		 = 0;
	cmdStruct.DataMode 			 = dataMode;
	cmdStruct.DataCount 		 = 2048;
	
	QSPI_WriteEnable(QSPI0);
	
	QSPI_Command(QSPI0, QSPI_Mode_IndirectWrite, &cmdStruct);
	
	for(int i = 0; i < 2048 / 4; i++)
	{
		uint32_t * p_word = (uint32_t *)buff;
		
		while(QSPI_FIFOSpace(QSPI0) < 4) __NOP();
		
		QSPI0->DRW = p_word[i];
	}
	
	while(QSPI_Busy(QSPI0)) __NOP();
	
	
	/* Program the Data Buffer content into the physical memory page */
	QSPI_CmdStructClear(&cmdStruct);
	cmdStruct.InstructionMode 	 = QSPI_PhaseMode_1bit;
	cmdStruct.Instruction 		 = W25N_CMD_PROGRAM_EXECUTE;
	cmdStruct.AddressMode 		 = QSPI_PhaseMode_1bit;
	cmdStruct.AddressSize 		 = QSPI_PhaseSize_24bit;	// 高 8bit 为 dummy clock，低 16bit 为地址
	cmdStruct.Address 			 = addr >> 12;				// Page Address
	cmdStruct.AlternateBytesMode = QSPI_PhaseMode_None;
	cmdStruct.DummyCycles 		 = 0;
	cmdStruct.DataMode 			 = QSPI_PhaseMode_None;
	
	QSPI_Command(QSPI0, QSPI_Mode_IndirectWrite, &cmdStruct);
	
	while(QSPI_Busy(QSPI0)) __NOP();
	
	while(W25N01G_FlashBusy()) __NOP();
}


/****************************************************************************************************************************************** 
* 函数名称:	W25N01G_Read_()
* 功能说明:	W25N01G 页读取，页大小为 2112 Byte
* 输    入: uint32_t addr			要读取自的 SPI Flash 地址，必须是 0x1000（即 4096）的整数倍
*			uint8_t buff[2048]		读取到的数据写入此数组中，数组大小必须是 2048 字节，ECC 数据不读取
*			uint8_t addr_width		读取使用的地址线个数，有效值包括 1、2、4
*			uint8_t data_width		读取使用的数据线个数，有效值包括 1、2、4
* 输    出: 无
* 注意事项: 无
******************************************************************************************************************************************/
void W25N01G_Read_(uint32_t addr, uint8_t buff[2048], uint8_t addr_width, uint8_t data_width)
{
	QSPI_CmdStructure cmdStruct;
	QSPI_CmdStructClear(&cmdStruct);
	
	/* Transfer the data of specified page into the 2112-Byte Data Buffer */
	cmdStruct.InstructionMode 	 = QSPI_PhaseMode_1bit;
	cmdStruct.Instruction 		 = W25N_CMD_PAGE_READ;
	cmdStruct.AddressMode 		 = QSPI_PhaseMode_1bit;
	cmdStruct.AddressSize 		 = QSPI_PhaseSize_24bit;	// 高 8bit 为 dummy clock，低 16bit 为地址
	cmdStruct.Address 			 = addr >> 12;				// Page Address
	cmdStruct.AlternateBytesMode = QSPI_PhaseMode_None;
	cmdStruct.DummyCycles 		 = 0;
	cmdStruct.DataMode 			 = QSPI_PhaseMode_None;
	
	QSPI_Command(QSPI0, QSPI_Mode_IndirectWrite, &cmdStruct);
	
	while(QSPI_Busy(QSPI0)) __NOP();
	
	while(W25N01G_FlashBusy()) __NOP();
	
	
	uint8_t instruction, addressMode, dataMode, dummyCycles;
	switch((addr_width << 4) | data_width)
	{
	case 0x11:
		instruction 	   = W25N_CMD_FAST_READ;
		addressMode 	   = QSPI_PhaseMode_1bit;
		dummyCycles        = 8;
		dataMode 		   = QSPI_PhaseMode_1bit;
		break;
	
	case 0x12:
		instruction 	   = W25N_CMD_FAST_READ_2bit;
		addressMode 	   = QSPI_PhaseMode_1bit;
		dummyCycles        = 8;
		dataMode 		   = QSPI_PhaseMode_2bit;
		break;
	
	case 0x22:
		instruction 	   = W25N_CMD_FAST_READ_IO2bit;
		addressMode 	   = QSPI_PhaseMode_2bit;
		dummyCycles        = 4;
		dataMode 		   = QSPI_PhaseMode_2bit;
		break;
	
	case 0x14:
		instruction 	   = W25N_CMD_FAST_READ_4bit;
		addressMode 	   = QSPI_PhaseMode_1bit;
		dummyCycles        = 8;
		dataMode 		   = QSPI_PhaseMode_4bit;
		break;
	
	case 0x44:
		instruction 	   = W25N_CMD_FAST_READ_IO4bit;
		addressMode 	   = QSPI_PhaseMode_4bit;
		dummyCycles        = 4;
		dataMode 		   = QSPI_PhaseMode_4bit;
		break;
	}
	
	QSPI_CmdStructClear(&cmdStruct);
	cmdStruct.InstructionMode 	 = QSPI_PhaseMode_1bit;
	cmdStruct.Instruction 		 = instruction;
	cmdStruct.AddressMode 		 = addressMode;
	cmdStruct.AddressSize 		 = QSPI_PhaseSize_16bit;
	cmdStruct.Address 			 = 0x000;					// Column Address
	cmdStruct.AlternateBytesMode = QSPI_PhaseMode_None;
	cmdStruct.DummyCycles 		 = dummyCycles;
	cmdStruct.DataMode 			 = dataMode;
	cmdStruct.DataCount 		 = 2048;
	
	QSPI_Command(QSPI0, QSPI_Mode_IndirectRead, &cmdStruct);
		
	for(int i = 0; i < 2048 / 4; i++)
	{
		uint32_t * p_word = (uint32_t *)buff;
		
		while(QSPI_FIFOCount(QSPI0) < 4) __NOP();
		
		p_word[i] = QSPI0->DRW;
	}
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
	
	reg &= ~(W25N_STATUS_REG1_TB_Msk | W25N_STATUS_REG1_BP_Msk);
	reg |= (tb << W25N_STATUS_REG1_TB_Pos) |
		   (bp << W25N_STATUS_REG1_BP_Pos);
	
	W25N01G_WriteReg(W25N_STATUS_REG1, reg);
}


void W25N01G_WriteReg(uint8_t reg_addr, uint8_t data)
{
	QSPI_CmdStructure cmdStruct;
	QSPI_CmdStructClear(&cmdStruct);
	
	cmdStruct.InstructionMode 	 = QSPI_PhaseMode_1bit;
	cmdStruct.Instruction 		 = W25N_CMD_WRITE_STATUS_REG;
	cmdStruct.AddressMode 		 = QSPI_PhaseMode_1bit;
	cmdStruct.AddressSize		 = QSPI_PhaseSize_8bit;
	cmdStruct.Address			 = reg_addr;
	cmdStruct.AlternateBytesMode = QSPI_PhaseMode_None;
	cmdStruct.DummyCycles 		 = 0;
	cmdStruct.DataMode 			 = QSPI_PhaseMode_1bit;
	cmdStruct.DataCount 		 = 1;
	
	QSPI_Command(QSPI0, QSPI_Mode_IndirectWrite, &cmdStruct);
		
	QSPI0->DRB = data;
	
	while(QSPI_Busy(QSPI0)) __NOP();
}


uint8_t W25N01G_ReadReg(uint8_t reg_addr)
{
	QSPI_CmdStructure cmdStruct;
	QSPI_CmdStructClear(&cmdStruct);
	
	cmdStruct.InstructionMode 	 = QSPI_PhaseMode_1bit;
	cmdStruct.Instruction 		 = W25N_CMD_READ_STATUS_REG;
	cmdStruct.AddressMode 		 = QSPI_PhaseMode_1bit;
	cmdStruct.AddressSize		 = QSPI_PhaseSize_8bit;
	cmdStruct.Address			 = reg_addr;
	cmdStruct.AlternateBytesMode = QSPI_PhaseMode_None;
	cmdStruct.DummyCycles 		 = 0;
	cmdStruct.DataMode 			 = QSPI_PhaseMode_1bit;
	cmdStruct.DataCount 		 = 1;
	
	QSPI_Command(QSPI0, QSPI_Mode_IndirectRead, &cmdStruct);
	
	while(QSPI_FIFOEmpty(QSPI0)) __NOP();
	
	return QSPI0->DRB;
}
