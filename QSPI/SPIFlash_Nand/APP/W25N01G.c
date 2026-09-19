#include "SWM221.h"
#include "W25N01G.h"


/*******************************************************************************************************************************
* @brief	W25N01G init
* @param
* @return
*******************************************************************************************************************************/
void W25N01G_Init(void)
{
	QSPI_InitStructure QSPI_initStruct;
	
	PORT_Init(PORTC, PIN3,  PORTC_PIN3_QSPI0_SCLK,  0);
	PORT_Init(PORTC, PIN2,  PORTC_PIN2_QSPI0_SSEL,  0);
	PORT_Init(PORTA, PIN15, PORTA_PIN15_QSPI0_MOSI, 1);
	PORT_Init(PORTB, PIN0,  PORTB_PIN0_QSPI0_MISO,  1);
	PORT_Init(PORTB, PIN1,  PORTB_PIN1_QSPI0_D2,    1);
	PORT_Init(PORTB, PIN2,  PORTB_PIN2_QSPI0_D3,    1);
	PORTB->PULLU |= (1 << PIN2);	// when HOLD pin is low level, W25N01G don't excute any command
	
	QSPI_initStruct.Size = QSPI_Size_128MB;
	QSPI_initStruct.ClkDiv = 4;
	QSPI_initStruct.ClkMode = QSPI_ClkMode_3;
	QSPI_initStruct.SampleShift = (QSPI_initStruct.ClkDiv == 2) ? QSPI_SampleShift_1_SYSCLK : QSPI_SampleShift_NONE;
	QSPI_initStruct.IntEn = 0;
	QSPI_Init(QSPI0, &QSPI_initStruct);
	QSPI_Open(QSPI0);
	
	uint8_t reg = W25N01G_ReadReg(W25N_STATUS_REG2);
	
	reg |= (1 << W25N_STATUS_REG2_BUF_Pos);		// Buffer Read Mode
	
	W25N01G_WriteReg(W25N_STATUS_REG2, reg);
}


/*******************************************************************************************************************************
* @brief	read JEDEC ID from W25N01G
* @param
* @return	JEDEC ID
*******************************************************************************************************************************/
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


/*******************************************************************************************************************************
* @brief	W25N01G Flash block erase, block size is 128KB
* @param	page is page address to erase. 2KB per page, 64 page per block, so must meet page % 64 == 0 here.
* @param	wait: 1 wait for erase operation done, 0 send out erase command, and then immediately return.
* @return
*******************************************************************************************************************************/
void W25N01G_Erase(uint32_t page, uint8_t wait)
{
	QSPI_CmdStructure cmdStruct;
	QSPI_CmdStructClear(&cmdStruct);
	
	cmdStruct.InstructionMode 	 = QSPI_PhaseMode_1bit;
	cmdStruct.Instruction 		 = W25N_CMD_ERASE_BLOCK128KB;
	cmdStruct.AddressMode 		 = QSPI_PhaseMode_1bit;
	cmdStruct.AddressSize		 = QSPI_PhaseSize_24bit;	// the high 8bit is the dummy clock, and the low 16bit is the address
	cmdStruct.Address			 = page;
	cmdStruct.AlternateBytesMode = QSPI_PhaseMode_None;
	cmdStruct.DummyCycles 		 = 0;
	cmdStruct.DataMode 			 = QSPI_PhaseMode_None;
	
	QSPI_WriteEnable(QSPI0);
	
	QSPI_Command(QSPI0, QSPI_Mode_IndirectWrite, &cmdStruct);
	
	while(QSPI_Busy(QSPI0)) __NOP();
	
	if(wait)
		while(W25N01G_FlashBusy()) __NOP();
}


/*******************************************************************************************************************************
* @brief	W25N01G Flash page write
* @param	page is page address to write, 0-65535
* @param	buff is the data to be written to W25N01G, the number of bytes must be 2048
* @param	data_width is number of data line to use in data phase, can be 1 or 4
* @param	data_phase indicates whether do data phase in the function, or outside the function (by DMA for example)
* @return
*******************************************************************************************************************************/
void W25N01G_Write_(uint32_t page, uint8_t buff[2048], uint8_t data_width, uint8_t data_phase)
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
	
	if(data_phase == 0)
		return;
	
	if((uint32_t)buff % 4 == 0)	// word aligned
	{
		for(int i = 0; i < 2048 / 4; i++)
		{
			uint32_t * p_word = (uint32_t *)buff;
			
			while(QSPI_FIFOSpace(QSPI0) < 4) __NOP();
			
			QSPI0->DRW = p_word[i];
		}
	}
	else
	{
		for(int i = 0; i < 2048; i++)
		{
			while(QSPI_FIFOSpace(QSPI0) < 1) __NOP();
			
			QSPI0->DRB = buff[i];
		}
	}
	
	while(QSPI_Busy(QSPI0)) __NOP();
	
	W25N01G_Program_Execute(page);
	
	while(W25N01G_FlashBusy()) __NOP();
}


/* Program the Data Buffer content into the physical memory page */
void W25N01G_Program_Execute(uint32_t page)
{
	QSPI_CmdStructure cmdStruct;
	QSPI_CmdStructClear(&cmdStruct);
	
	cmdStruct.InstructionMode 	 = QSPI_PhaseMode_1bit;
	cmdStruct.Instruction 		 = W25N_CMD_PROGRAM_EXECUTE;
	cmdStruct.AddressMode 		 = QSPI_PhaseMode_1bit;
	cmdStruct.AddressSize 		 = QSPI_PhaseSize_24bit;	// the high 8bit is the dummy clock, and the low 16bit is the address
	cmdStruct.Address 			 = page;
	cmdStruct.AlternateBytesMode = QSPI_PhaseMode_None;
	cmdStruct.DummyCycles 		 = 0;
	cmdStruct.DataMode 			 = QSPI_PhaseMode_None;
	
	QSPI_Command(QSPI0, QSPI_Mode_IndirectWrite, &cmdStruct);
	
	while(QSPI_Busy(QSPI0)) __NOP();
}


/*******************************************************************************************************************************
* @brief	W25N01G Flash page read
* @param	page is page address to read, 0-65535
* @param	buff is the buffer used to save read data, and the size of it must be 2048
* @param	addr_width is number of data line to use in address phase, can be 1, 2 or 4
* @param	data_width is number of data line to use in data phase, can be 1, 2, or 4
* @param	data_phase indicates whether do data phase in the function, or outside the function (by DMA for example)
* @return
*******************************************************************************************************************************/
void W25N01G_Read_(uint32_t page, uint8_t buff[2048], uint8_t addr_width, uint8_t data_width, uint8_t data_phase)
{
	QSPI_CmdStructure cmdStruct;
	QSPI_CmdStructClear(&cmdStruct);
	
	/* Transfer the data of specified page into the 2112-Byte Data Buffer */
	cmdStruct.InstructionMode 	 = QSPI_PhaseMode_1bit;
	cmdStruct.Instruction 		 = W25N_CMD_PAGE_READ;
	cmdStruct.AddressMode 		 = QSPI_PhaseMode_1bit;
	cmdStruct.AddressSize 		 = QSPI_PhaseSize_24bit;	// the high 8bit is the dummy clock, and the low 16bit is the address
	cmdStruct.Address 			 = page;
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
	
	if(data_phase == 0)
		return;
	
	if((uint32_t)buff % 4 == 0)	// word aligned
	{
		for(int i = 0; i < 2048 / 4; i++)
		{
			uint32_t * p_word = (uint32_t *)buff;
			
			while(QSPI_FIFOCount(QSPI0) < 4) __NOP();
			
			p_word[i] = QSPI0->DRW;
		}
	}
	else
	{
		for(int i = 0; i < 2048; i++)
		{
			while(QSPI_FIFOCount(QSPI0) < 1) __NOP();
			
			buff[i] = QSPI0->DRB;
		}
	}
	
	QSPI_Abort(QSPI0);
}


/*******************************************************************************************************************************
* @brief	W25N01G busy query
* @param
* @return	1 SPI Flash is busy for internal erase or write, 0 SPI Flash internal operation done
*******************************************************************************************************************************/
bool W25N01G_FlashBusy(void)
{
	uint8_t reg = W25N01G_ReadReg(W25N_STATUS_REG3);
	
	bool busy = (reg & (1 << W25N_STATUS_REG3_BUSY_Pos));
	
	return busy;
}


/*******************************************************************************************************************************
* @brief	W25N01G write protection set
* @param
* @return
*******************************************************************************************************************************/
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
