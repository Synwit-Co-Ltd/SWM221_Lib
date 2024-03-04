/****************************************************************************************************************************************** 
* 文件名称: SWM342_qspi.c
* 功能说明:	SWM342单片机的QSPI模块驱动库
* 技术支持:	http://www.synwit.com.cn/e/tool/gbook/?bid=1
* 注意事项: 
* 版本日期:	V1.1.0		2017年10月25日
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
#include "SWM221.h"
#include "SWM221_qspi.h"


static uint8_t AddressSize;


/****************************************************************************************************************************************** 
* 函数名称:	QSPI_Init()
* 功能说明:	QSPI 初始化
* 输    入: QSPI_TypeDef * QSPIx	指定要被设置的QSPI接口，有效值包括QSPI0
*			QSPI_InitStructure * initStruct		包含QSPI接口相关设定值的结构体
* 输    出: 无
* 注意事项: 无
******************************************************************************************************************************************/
void QSPI_Init(QSPI_TypeDef * QSPIx, QSPI_InitStructure * initStruct)
{
	switch((uint32_t)QSPIx)
	{
	case ((uint32_t)QSPI0):
		SYS->CLKEN0 |= (0x01 << SYS_CLKEN0_QSPI0_Pos);
		break;
	}
	
	QSPI_Close(QSPIx);
	
	QSPIx->CR = ((initStruct->SampleShift >> 4)	<< QSPI_CR_SSHIFT_Pos) |
				(15 							<< QSPI_CR_FFTHR_Pos)  |
				(initStruct->IntEn 				<< QSPI_CR_ERRIE_Pos)  |
				((initStruct->ClkDiv - 1) 		<< QSPI_CR_CLKDIV_Pos);
	
	QSPIx->DCR = (initStruct->ClkMode 	<< QSPI_DCR_CLKMOD_Pos) |
				 (3						<< QSPI_DCR_CSHIGH_Pos) |
				 (initStruct->Size		<< QSPI_DCR_FLSIZE_Pos);
	
	AddressSize = initStruct->Size / 8;
	
	QSPIx->SSTRIM = initStruct->SampleShift & 0x0F;
	
	QSPIx->CS0TO = 100;
	
	QSPIx->FCR = 0x1B;
	if(initStruct->IntEn)
	{
		switch((uint32_t)QSPIx)
		{
		case ((uint32_t)QSPI0):	NVIC_EnableIRQ(QSPI0_IRQn); break;
		}
	}
}


/****************************************************************************************************************************************** 
* 函数名称:	QSPI_Open()
* 功能说明:	QSPI接口打开
* 输    入: QSPI_TypeDef * QSPIx	指定要被设置的QSPI接口，有效值包括QSPI0
* 输    出: 无
* 注意事项: 无
******************************************************************************************************************************************/
void QSPI_Open(QSPI_TypeDef * QSPIx)
{
	QSPIx->CR |= QSPI_CR_EN_Msk;
}


/****************************************************************************************************************************************** 
* 函数名称:	QSPI_Close()
* 功能说明:	QSPI接口关闭
* 输    入: QSPI_TypeDef * QSPIx	指定要被设置的QSPI接口，有效值包括QSPI0
* 输    出: 无
* 注意事项: 无
******************************************************************************************************************************************/
void QSPI_Close(QSPI_TypeDef * QSPIx)
{
	QSPIx->CR &= ~QSPI_CR_EN_Msk;
}


void QSPI_CmdStructClear(QSPI_CmdStructure * cmdStruct)
{
	cmdStruct->Instruction		  = 0;
	cmdStruct->InstructionMode	  = 0;
	cmdStruct->Address 			  = 0;
	cmdStruct->AddressMode		  = 0;
	cmdStruct->AddressSize		  = 0;
	cmdStruct->AlternateBytes 	  = 0;
	cmdStruct->AlternateBytesMode = 0;
	cmdStruct->AlternateBytesSize = 0;
	cmdStruct->DummyCycles		  = 0;
	cmdStruct->DataMode			  = 0;
	cmdStruct->DataCount		  = 0;
	cmdStruct->SendInstOnlyOnce   = 0;
}


/****************************************************************************************************************************************** 
* 函数名称:	QSPI_Command()
* 功能说明:	QSPI命令执行
* 输    入: QSPI_TypeDef * QSPIx	指定要被设置的QSPI接口，有效值包括QSPI0
*			QSPI_CmdStructure * cmdStruct	要执行命令的内容
* 输    出: 无
* 注意事项: 无
******************************************************************************************************************************************/
void QSPI_Command(QSPI_TypeDef * QSPIx, uint8_t cmdMode, QSPI_CmdStructure * cmdStruct)
{
	if(cmdStruct->AlternateBytesMode != QSPI_PhaseMode_None)
		QSPIx->ABR = cmdStruct->AlternateBytes;
	
	if(cmdStruct->DataMode != QSPI_PhaseMode_None)
		QSPIx->DLR = cmdStruct->DataCount - 1;
	
	QSPIx->CCR = (cmdStruct->Instruction		<< QSPI_CCR_CODE_Pos)   |
				 (cmdStruct->InstructionMode	<< QSPI_CCR_IMODE_Pos)  |
				 (cmdStruct->AddressMode		<< QSPI_CCR_AMODE_Pos)  |
				 (cmdStruct->AddressSize		<< QSPI_CCR_ASIZE_Pos)  |
				 (cmdStruct->AlternateBytesMode	<< QSPI_CCR_ABMODE_Pos) |
				 (cmdStruct->AlternateBytesSize	<< QSPI_CCR_ABSIZE_Pos) |
				 (cmdStruct->DummyCycles		<< QSPI_CCR_DUMMY_Pos)  |
				 (cmdStruct->DataMode			<< QSPI_CCR_DMODE_Pos)  |
				 (cmdMode						<< QSPI_CCR_MODE_Pos)   |
				 (cmdStruct->SendInstOnlyOnce	<< QSPI_CCR_SIOO_Pos);
	
	if(cmdStruct->AddressMode != QSPI_PhaseMode_None)
		QSPIx->AR = cmdStruct->Address;
}


/****************************************************************************************************************************************** 
* 函数名称:	QSPI_Erase()
* 功能说明:	QSPI Flash 擦除
* 输    入: QSPI_TypeDef * QSPIx	指定要被设置的QSPI接口，有效值包括QSPI0
*			uint8_t cmd				擦除命令 ，有效值包括 QSPI_CMD_ERASE_SECTOR、QSPI_CMD_ERASE_BLOCK32KB、QSPI_CMD_ERASE_BLOCK64KB
*			uint32_t addr			要擦除的 SPI Flash 地址
*			uint8_t wait			是否等待 SPI Flash 完成操作操作，1 等待完成   0 立即返回
* 输    出: 无
* 注意事项: 若 wait == 0 立即返回，需要在随后调用 QSPI_FlashBusy() 检查 SPI Flash 完成操作后，再执行读、写操作
******************************************************************************************************************************************/
void QSPI_Erase(QSPI_TypeDef * QSPIx, uint8_t cmd, uint32_t addr, uint8_t wait)
{
	QSPI_CmdStructure cmdStruct;
	QSPI_CmdStructClear(&cmdStruct);
	
	cmdStruct.InstructionMode 	 = QSPI_PhaseMode_1bit;
	cmdStruct.Instruction 		 = cmd;
	cmdStruct.AddressMode 		 = QSPI_PhaseMode_1bit;
	cmdStruct.AddressSize		 = AddressSize;
	cmdStruct.Address			 = addr;
	cmdStruct.AlternateBytesMode = QSPI_PhaseMode_None;
	cmdStruct.DummyCycles 		 = 0;
	cmdStruct.DataMode 			 = QSPI_PhaseMode_None;
	
	QSPI_WriteEnable(QSPIx);
	
	QSPI_Command(QSPIx, QSPI_Mode_IndirectWrite, &cmdStruct);
		
	while(QSPI_Busy(QSPIx)) __NOP();
	
	if(wait)
		while(QSPI_FlashBusy(QSPIx)) __NOP();
}


/****************************************************************************************************************************************** 
* 函数名称:	QSPI_Write_()
* 功能说明:	QSPI Flash 写入
* 输    入: QSPI_TypeDef * QSPIx	指定要被设置的QSPI接口，有效值包括QSPI0
*			uint32_t addr			要写入到的 SPI Flash 地址
*			uint8_t buff[]			要写入 SPI Flash 的数据
*			uint32_t count			要写入 SPI Flash 的数据个数，最大 256，且写入数据不能跨页
*			uint8_t data_width		写入使用的数据线个数，有效值包括 1、4
* 输    出: 无
* 注意事项: 无
******************************************************************************************************************************************/
void QSPI_Write_(QSPI_TypeDef * QSPIx, uint32_t addr, uint8_t buff[], uint32_t count, uint8_t data_width)
{
	QSPI_CmdStructure cmdStruct;
	QSPI_CmdStructClear(&cmdStruct);
	
	uint8_t instruction, dataMode;
	switch(data_width)
	{
	case 1:
		instruction = QSPI_CMD_PAGE_PROGRAM;
		dataMode 	= QSPI_PhaseMode_1bit;
		break;
	
	case 4:
		instruction = QSPI_CMD_PAGE_PROGRAM_4bit;
		dataMode 	= QSPI_PhaseMode_4bit;
		break;
	}
	
	cmdStruct.InstructionMode 	 = QSPI_PhaseMode_1bit;
	cmdStruct.Instruction 		 = instruction;
	cmdStruct.AddressMode 		 = QSPI_PhaseMode_1bit;
	cmdStruct.AddressSize 		 = AddressSize;
	cmdStruct.Address 			 = addr;
	cmdStruct.AlternateBytesMode = QSPI_PhaseMode_None;
	cmdStruct.DummyCycles 		 = 0;
	cmdStruct.DataMode 			 = dataMode;
	cmdStruct.DataCount 		 = count;
	
	QSPI_WriteEnable(QSPIx);
	
	QSPI_Command(QSPIx, QSPI_Mode_IndirectWrite, &cmdStruct);
		
	uint32_t n_word = count / 4;
	
	for(int i = 0; i < n_word; i++)
	{
		uint32_t * p_word = (uint32_t *)buff;
		
		while(QSPI_FIFOSpace(QSPIx) < 4) __NOP();
		
		QSPIx->DRW = p_word[i];
	}
	
	if((count % 4) / 2)
	{
		uint16_t * p_half = (uint16_t *)&buff[n_word * 4];
		
		while(QSPI_FIFOSpace(QSPIx) < 2) __NOP();
		
		QSPIx->DRH = p_half[0];
	}
	
	if(count % 2)
	{
		while(QSPI_FIFOSpace(QSPIx) < 1) __NOP();
		
		QSPIx->DRB = buff[count - 1];
	}
	
	while(QSPI_Busy(QSPIx)) __NOP();
	
	while(QSPI_FlashBusy(QSPIx)) __NOP();
}


/****************************************************************************************************************************************** 
* 函数名称:	QSPI_Read_()
* 功能说明:	QSPI Flash 读取
* 输    入: QSPI_TypeDef * QSPIx	指定要被设置的QSPI接口，有效值包括QSPI0
*			uint32_t addr			要读取自的 SPI Flash 地址
*			uint8_t buff[]			读取到的数据写入此数组中
*			uint32_t count			要读取数据的个数
*			uint8_t addr_width		读取使用的地址线个数，有效值包括 1、2、4
*			uint8_t data_width		读取使用的数据线个数，有效值包括 1、2、4
* 输    出: 无
* 注意事项: 无
******************************************************************************************************************************************/
void QSPI_Read_(QSPI_TypeDef * QSPIx, uint32_t addr, uint8_t buff[], uint32_t count, uint8_t addr_width, uint8_t data_width)
{
	QSPI_CmdStructure cmdStruct;
	QSPI_CmdStructClear(&cmdStruct);
	
	uint8_t instruction, addressMode, dataMode, dummyCycles;
	uint8_t alternateBytesMode, alternateBytesSize, alternateBytes;
	switch((addr_width << 4) | data_width)
	{
	case 0x11:
		instruction 	   = QSPI_CMD_FAST_READ;
		addressMode 	   = QSPI_PhaseMode_1bit;
		alternateBytesMode = QSPI_PhaseMode_None;
		alternateBytesSize = 0;
		dummyCycles        = 8;
		dataMode 		   = QSPI_PhaseMode_1bit;
		break;
	
	case 0x12:
		instruction 	   = QSPI_CMD_FAST_READ_2bit;
		addressMode 	   = QSPI_PhaseMode_1bit;
		alternateBytesMode = QSPI_PhaseMode_None;
		alternateBytesSize = 0;
		dummyCycles        = 8;
		dataMode 		   = QSPI_PhaseMode_2bit;
		break;
	
	case 0x22:
		instruction 	   = QSPI_CMD_FAST_READ_IO2bit;
		addressMode 	   = QSPI_PhaseMode_2bit;
		alternateBytesMode = QSPI_PhaseMode_2bit;
		alternateBytesSize = QSPI_PhaseSize_8bit;
		alternateBytes     = 0xFF;
		dummyCycles        = 0;
		dataMode 		   = QSPI_PhaseMode_2bit;
		break;
	
	case 0x14:
		instruction 	   = QSPI_CMD_FAST_READ_4bit;
		addressMode 	   = QSPI_PhaseMode_1bit;
		alternateBytesMode = QSPI_PhaseMode_None;
		alternateBytesSize = 0;
		dummyCycles        = 8;
		dataMode 		   = QSPI_PhaseMode_4bit;
		break;
	
	case 0x44:
		instruction 	   = QSPI_CMD_FAST_READ_IO4bit;
		addressMode 	   = QSPI_PhaseMode_4bit;
		alternateBytesMode = QSPI_PhaseMode_4bit;
		alternateBytesSize = QSPI_PhaseSize_8bit;
		alternateBytes     = 0xFF;
		dummyCycles        = 4;
		dataMode 		   = QSPI_PhaseMode_4bit;
		break;
	}
	
	cmdStruct.InstructionMode 	 = QSPI_PhaseMode_1bit;
	cmdStruct.Instruction 		 = instruction;
	cmdStruct.AddressMode 		 = addressMode;
	cmdStruct.AddressSize 		 = AddressSize;
	cmdStruct.Address 			 = addr;
	cmdStruct.AlternateBytesMode = alternateBytesMode;
	cmdStruct.AlternateBytesSize = alternateBytesSize;
	cmdStruct.AlternateBytes 	 = alternateBytes;
	cmdStruct.DummyCycles 		 = dummyCycles;
	cmdStruct.DataMode 			 = dataMode;
	cmdStruct.DataCount 		 = count;
	
	QSPI_Command(QSPIx, QSPI_Mode_IndirectRead, &cmdStruct);
		
	uint32_t n_word = count / 4;
	
	for(int i = 0; i < n_word; i++)
	{
		uint32_t * p_word = (uint32_t *)buff;
		
		while(QSPI_FIFOCount(QSPIx) < 4) __NOP();
		
		p_word[i] = QSPIx->DRW;
	}
	
	if((count % 4) / 2)
	{
		uint16_t * p_half = (uint16_t *)&buff[n_word * 4];
		
		while(QSPI_FIFOCount(QSPIx) < 2) __NOP();
		
		p_half[0] = QSPIx->DRH;
	}
	
	if(count % 2)
	{
		while(QSPI_FIFOCount(QSPIx) < 1) __NOP();
		
		buff[count - 1] = QSPIx->DRB;
	}
}


/****************************************************************************************************************************************** 
* 函数名称:	QSPI_FlashBusy()
* 功能说明:	QSPI Flash 忙查询
* 输    入: QSPI_TypeDef * QSPIx	指定要被设置的QSPI接口，有效值包括QSPI0
* 输    出: bool					SPI Flash 是否正在执行内部擦、写操作
* 注意事项: 无
******************************************************************************************************************************************/
bool QSPI_FlashBusy(QSPI_TypeDef * QSPIx)
{
	uint16_t reg = QSPI_ReadReg(QSPIx, QSPI_CMD_READ_STATUS_REG1, 1);
	
	bool busy = (reg & (1 << QSPI_STATUS_REG1_BUSY_Pos));
	
	return busy;
}


/****************************************************************************************************************************************** 
* 函数名称:	QSPI_QuadState()
* 功能说明:	QSPI Flash QE 查询
* 输    入: QSPI_TypeDef * QSPIx	指定要被设置的QSPI接口，有效值包括QSPI0
* 输    出: uint8_t					1 QE 使能   0 QE 禁止
* 注意事项: 不同品牌 SPI Flash 中，QE 在 Status Register 中的位置可能不同
******************************************************************************************************************************************/
uint8_t QSPI_QuadState(QSPI_TypeDef * QSPIx)
{
	uint8_t reg = QSPI_ReadReg(QSPIx, QSPI_CMD_READ_STATUS_REG2, 1);
	
	return (reg & (1 << QSPI_STATUS_REG2_QUAD_Pos)) ? 1 : 0;
}


/****************************************************************************************************************************************** 
* 函数名称:	QSPI_QuadSwitch()
* 功能说明:	QSPI Flash QE 开关
* 输    入: QSPI_TypeDef * QSPIx	指定要被设置的QSPI接口，有效值包括QSPI0
*			uint8_t on				1 QE 使能   0 QE 禁止
* 输    出: 无
* 注意事项: 不同品牌 SPI Flash 中，QE 在 Status Register 中的位置可能不同
******************************************************************************************************************************************/
void QSPI_QuadSwitch(QSPI_TypeDef * QSPIx, uint8_t on)
{
	uint8_t reg = QSPI_ReadReg(QSPIx, QSPI_CMD_READ_STATUS_REG2, 1);
	
	if(on)
		reg |=  (1 << QSPI_STATUS_REG2_QUAD_Pos);
	else
		reg &= ~(1 << QSPI_STATUS_REG2_QUAD_Pos);
	
	QSPI_WriteEnable(QSPIx);
	
	QSPI_WriteReg(QSPIx, QSPI_CMD_WRITE_STATUS_REG2, reg, 1);
	
	while(QSPI_FlashBusy(QSPIx)) __NOP();
}


/****************************************************************************************************************************************** 
* 函数名称:	QSPI_SendCmd()
* 功能说明:	QSPI 命令发送，适用于只有命令阶段的情况
* 输    入: QSPI_TypeDef * QSPIx	指定要被设置的QSPI接口，有效值包括QSPI0
*			uint8_t cmd				要发送的命令
* 输    出: 无
* 注意事项: 无
******************************************************************************************************************************************/
void QSPI_SendCmd(QSPI_TypeDef * QSPIx, uint8_t cmd)
{
	QSPI_CmdStructure cmdStruct;
	QSPI_CmdStructClear(&cmdStruct);
	
	cmdStruct.InstructionMode 	 = QSPI_PhaseMode_1bit;
	cmdStruct.Instruction 		 = cmd;
	cmdStruct.AddressMode 		 = QSPI_PhaseMode_None;
	cmdStruct.AlternateBytesMode = QSPI_PhaseMode_None;
	cmdStruct.DummyCycles 		 = 0;
	cmdStruct.DataMode 			 = QSPI_PhaseMode_None;
	
	QSPI_Command(QSPIx, QSPI_Mode_IndirectWrite, &cmdStruct);
	
	while(QSPI_Busy(QSPIx)) __NOP();
}


/****************************************************************************************************************************************** 
* 函数名称:	QSPI_ReadReg()
* 功能说明:	QSPI Flash 寄存器读取，适用于只有命令、数据阶段，且数据不多于4字节的情况
* 输    入: QSPI_TypeDef * QSPIx	指定要被设置的QSPI接口，有效值包括QSPI0
*			uint8_t cmd				寄存器读取命令
*			uint8_t n_bytes			要读取的字节数，可取值 1、2、3、4
* 输    出: uint32_t				读取到的数据，MSB
* 注意事项: 无
******************************************************************************************************************************************/
uint32_t QSPI_ReadReg(QSPI_TypeDef * QSPIx, uint8_t cmd, uint8_t n_bytes)
{
	QSPI_CmdStructure cmdStruct;
	QSPI_CmdStructClear(&cmdStruct);
	
	cmdStruct.InstructionMode 	 = QSPI_PhaseMode_1bit;
	cmdStruct.Instruction 		 = cmd;
	cmdStruct.AddressMode 		 = QSPI_PhaseMode_None;
	cmdStruct.AlternateBytesMode = QSPI_PhaseMode_None;
	cmdStruct.DummyCycles 		 = 0;
	cmdStruct.DataMode 			 = QSPI_PhaseMode_1bit;
	cmdStruct.DataCount 		 = n_bytes;
	
	QSPI_Command(QSPIx, QSPI_Mode_IndirectRead, &cmdStruct);
	
	while(QSPI_FIFOCount(QSPIx) < n_bytes) __NOP();
	
	uint32_t data = 0;
	for(int i = n_bytes; i > 0; i--)
		((uint8_t *)&data)[i-1] = QSPIx->DRB;
	
	return data;
}


/****************************************************************************************************************************************** 
* 函数名称:	QSPI_WriteReg()
* 功能说明:	QSPI Flash 寄存器写入，适用于只有命令、数据阶段，且数据不多于4字节的情况
* 输    入: QSPI_TypeDef * QSPIx	指定要被设置的QSPI接口，有效值包括QSPI0
*			uint8_t cmd				寄存器写入命令
*			uint32_t data			要写入寄存器的数据，MSB
*			uint8_t n_bytes			要写入寄存器的字节数，可取值 1、2、3、4
* 输    出: 无
* 注意事项: 无
******************************************************************************************************************************************/
void QSPI_WriteReg(QSPI_TypeDef * QSPIx, uint8_t cmd, uint32_t data, uint8_t n_bytes)
{
	QSPI_CmdStructure cmdStruct;
	QSPI_CmdStructClear(&cmdStruct);
	
	cmdStruct.InstructionMode 	 = QSPI_PhaseMode_1bit;
	cmdStruct.Instruction 		 = cmd;
	cmdStruct.AddressMode 		 = QSPI_PhaseMode_None;
	cmdStruct.AlternateBytesMode = QSPI_PhaseMode_None;
	cmdStruct.DummyCycles 		 = 0;
	cmdStruct.DataMode 			 = QSPI_PhaseMode_1bit;
	cmdStruct.DataCount 		 = n_bytes;
	
	QSPI_Command(QSPIx, QSPI_Mode_IndirectWrite, &cmdStruct);
	
	for(int i = n_bytes; i > 0; i--)
		QSPIx->DRB = ((uint8_t *)&data)[i-1];
	
	while(QSPI_Busy(QSPIx)) __NOP();
}


/****************************************************************************************************************************************** 
* 函数名称:	QSPI_INTEn()
* 功能说明:	中断使能
* 输    入: QSPI_TypeDef * QSPIx	指定要被设置的QSPI接口，有效值包括QSPI0
* 			uint32_t it				interrupt type，可取值 QSPI_IT_ERR、QSPI_IT_DONE、QSPI_IT_FFTHR、QSPI_IT_PSMAT、QSPI_IT_TO 及其“或”
* 输    出: 无
* 注意事项: 无
******************************************************************************************************************************************/
void QSPI_INTEn(QSPI_TypeDef * QSPIx, uint32_t it)
{
	QSPIx->CR |=  (it << 16);
}

/****************************************************************************************************************************************** 
* 函数名称:	QSPI_INTDis()
* 功能说明:	中断禁止
* 输    入: QSPI_TypeDef * QSPIx	指定要被设置的QSPI接口，有效值包括QSPI0
* 			uint32_t it				interrupt type，可取值 QSPI_IT_ERR、QSPI_IT_DONE、QSPI_IT_FFTHR、QSPI_IT_PSMAT、QSPI_IT_TO 及其“或”
* 输    出: 无
* 注意事项: 无
******************************************************************************************************************************************/
void QSPI_INTDis(QSPI_TypeDef * QSPIx, uint32_t it)
{
	QSPIx->CR &= ~(it << 16);
}

/****************************************************************************************************************************************** 
* 函数名称:	QSPI_INTClr()
* 功能说明:	中断标志清除
* 输    入: QSPI_TypeDef * QSPIx	指定要被设置的QSPI接口，有效值包括QSPI0
* 			uint32_t it				interrupt type，可取值 QSPI_IT_ERR、QSPI_IT_DONE、QSPI_IT_PSMAT、QSPI_IT_TO 及其“或”
* 输    出: 无
* 注意事项: 无
******************************************************************************************************************************************/
void QSPI_INTClr(QSPI_TypeDef * QSPIx, uint32_t it)
{
	QSPIx->FCR = it;
}

/****************************************************************************************************************************************** 
* 函数名称:	QSPI_INTStat()
* 功能说明:	中断状态查询
* 输    入: QSPI_TypeDef * QSPIx	指定要被设置的QSPI接口，有效值包括QSPI0
* 			uint32_t it				interrupt type，可取值 QSPI_IT_ERR、QSPI_IT_DONE、QSPI_IT_FFTHR、QSPI_IT_PSMAT、QSPI_IT_TO 及其“或”
* 输    出: uint32_t				0 中断未发生    非0 中断已发生
* 注意事项: 无
******************************************************************************************************************************************/
uint32_t QSPI_INTStat(QSPI_TypeDef * QSPIx, uint32_t it)
{
	return QSPIx->SR & it;
}
