#ifndef __W25N01G_H__
#define __W25N01G_H__

#include <stdbool.h>


#define W25N_PAGE_SIZE	2048


#define W25N_CMD_READ_JEDEC			0x9F
#define W25N_CMD_PAGE_READ			0x13	// Transfer the data of specified page into the 2112-Byte Data Buffer
#define W25N_CMD_FAST_READ			0x0B
#define W25N_CMD_FAST_READ_2bit		0x3B
#define W25N_CMD_FAST_READ_IO2bit	0xBB
#define W25N_CMD_FAST_READ_4bit		0x6B
#define W25N_CMD_FAST_READ_IO4bit	0xEB
#define W25N_CMD_WRITE_ENABLE		0x06
#define W25N_CMD_WRITE_DISABLE		0x04
#define W25N_CMD_PAGE_PROGRAM		0x02	// Load the program data into the 2112-Byte Data Buffer
#define W25N_CMD_PAGE_PROGRAM_4bit	0x32
#define W25N_CMD_PROGRAM_EXECUTE	0x10
#define W25N_CMD_ERASE_BLOCK128KB	0xD8
#define W25N_CMD_READ_STATUS_REG	0x05
#define W25N_CMD_WRITE_STATUS_REG	0x01


#define W25N_STATUS_REG1			0xA0
#define W25N_STATUS_REG2			0xB0
#define W25N_STATUS_REG3			0xC0

#define W25N_STATUS_REG1_WPE_Pos	1		// 硬件写保护使能位，关闭时引脚 /WP & /HOLD 用作 IO
#define W25N_STATUS_REG1_WPE_Msk	(0x01 << W25N_STATUS_REG1_WPE_Pos)
#define W25N_STATUS_REG1_TB_Pos		2
#define W25N_STATUS_REG1_TB_Msk		(0x01 << W25N_STATUS_REG1_TB_Pos)
#define W25N_STATUS_REG1_BP_Pos		3
#define W25N_STATUS_REG1_BP_Msk		(0x0F << W25N_STATUS_REG1_BP_Pos)

#define W25N_STATUS_REG2_BUF_Pos	3		// 1 Buffer Read Mode   0 Continuous Read Mode

#define W25N_STATUS_REG2_ECC_Pos	4		// ECC Enable Bit

#define W25N_STATUS_REG3_BUSY_Pos	0


#define W25N_PROTECT_None			0x00
#define W25N_PROTECT_Upper_256KB	0x01
#define W25N_PROTECT_Upper_512KB	0x02
#define W25N_PROTECT_Upper_1MB		0x03
#define W25N_PROTECT_Upper_2MB		0x04
#define W25N_PROTECT_Lower_256KB	0x11
#define W25N_PROTECT_Lower_512KB	0x12
#define W25N_PROTECT_Lower_1MB		0x13
#define W25N_PROTECT_Lower_2MB		0x14
#define W25N_PROTECT_All			0x1F



void W25N01G_Init(void);
uint32_t W25N01G_ReadJEDEC(void);

void W25N01G_Erase(uint32_t addr, uint8_t wait);
void W25N01G_Write(uint32_t addr, uint8_t buff[2048]);
void W25N01G_Read(uint32_t addr, uint8_t buff[2048]);
void W25N01G_Read_4bit_DMA(uint32_t addr, uint8_t buff[2048]);

bool W25N01G_FlashBusy(void);
void W25N01G_FlashProtect(uint8_t protect);
void W25N01G_WriteReg(uint8_t reg_addr, uint8_t data);
uint8_t W25N01G_ReadReg(uint8_t reg_addr);


#endif // __W25N01G_H__
