#ifndef __SWM221_QSPI_H__
#define __SWM221_QSPI_H__


typedef struct {
	uint16_t Size;			// Flash ��С��˫ Flash ʱָ���� Flash ���ܴ�С��ȡֵ QSPI_Size_1MB��...��QSPI_Size_512MB
	uint8_t  Bank;			// ѡ�� SPI Flash ���ӵ� QSPI Bank����ȡֵ QSPI_Bank_1��QSPI_Bank_2��QSPI_Bank_Dual
	uint16_t ClkDiv;		// ��ȡֵ 2--256
	uint8_t  ClkMode;		// ��ȡֵ QSPI_ClkMode_0��QSPI_ClkMode_3
	uint8_t  SampleShift;	// ��ȡֵ QSPI_SampleShift_None��QSPI_SampleShift_Half_SPICLK��QSPI_SampleShift_1_SYSCLK��...
	uint8_t  IntEn;			// ��ȡֵ QSPI_IT_ERR��QSPI_IT_DONE��QSPI_IT_FFTHR��QSPI_IT_PSMAT��QSPI_IT_TO ���䡰��
} QSPI_InitStructure;

#define QSPI_Size_1MB		19
#define QSPI_Size_2MB		20
#define QSPI_Size_4MB		21
#define QSPI_Size_8MB		22
#define QSPI_Size_16MB		23
#define QSPI_Size_32MB		24
#define QSPI_Size_64MB		25
#define QSPI_Size_128MB		26
#define QSPI_Size_256MB		27
#define QSPI_Size_512MB		28

#define QSPI_Bank_1			0
#define QSPI_Bank_2			2
#define QSPI_Bank_Dual		1

#define QSPI_ClkMode_0		0
#define QSPI_ClkMode_3		1

#define QSPI_SampleShift_NONE						0x00000
#define QSPI_SampleShift_1_SYSCLK					0x00001
#define QSPI_SampleShift_2_SYSCLK					0x00010
#define QSPI_SampleShift_3_SYSCLK					0x00011
#define QSPI_SampleShift_4_SYSCLK					0x00100
#define QSPI_SampleShift_5_SYSCLK					0x00101
#define QSPI_SampleShift_6_SYSCLK					0x00110
#define QSPI_SampleShift_7_SYSCLK					0x00111
#define QSPI_SampleShift_Half_SPICLK				0x10000
#define QSPI_SampleShift_Half_SPICLK_PLUS_1_SYSCLK	0x10001
#define QSPI_SampleShift_Half_SPICLK_PLUS_2_SYSCLK	0x10010
#define QSPI_SampleShift_Half_SPICLK_PLUS_3_SYSCLK	0x10011
#define QSPI_SampleShift_Half_SPICLK_PLUS_4_SYSCLK	0x10100
#define QSPI_SampleShift_Half_SPICLK_PLUS_5_SYSCLK	0x10101
#define QSPI_SampleShift_Half_SPICLK_PLUS_6_SYSCLK	0x10110
#define QSPI_SampleShift_Half_SPICLK_PLUS_7_SYSCLK	0x10111


typedef struct {
	uint8_t  Instruction;			// ָ����
	uint8_t  InstructionMode;		// ��ȡֵ��QSPI_PhaseMode_None��QSPI_PhaseMode_1bit��QSPI_PhaseMode_2bit��QSPI_PhaseMode_4bit
	uint32_t Address;
	uint8_t  AddressMode;			// ��ȡֵ��QSPI_PhaseMode_None��QSPI_PhaseMode_1bit��QSPI_PhaseMode_2bit��QSPI_PhaseMode_4bit
	uint8_t  AddressSize;			// ��ȡֵ��QSPI_PhaseSize_8bit��QSPI_PhaseSize_16bit��QSPI_PhaseSize_24bit��QSPI_PhaseSize_32bit
	uint32_t AlternateBytes;
	uint8_t  AlternateBytesMode;	// ��ȡֵ��QSPI_PhaseMode_None��QSPI_PhaseMode_1bit��QSPI_PhaseMode_2bit��QSPI_PhaseMode_4bit
	uint8_t  AlternateBytesSize;	// ��ȡֵ��QSPI_PhaseSize_8bit��QSPI_PhaseSize_16bit��QSPI_PhaseSize_24bit��QSPI_PhaseSize_32bit
	uint8_t  DummyCycles;			// ��ȡֵ��0--31
	uint8_t  DataMode;				// ��ȡֵ��QSPI_PhaseMode_None��QSPI_PhaseMode_1bit��QSPI_PhaseMode_2bit��QSPI_PhaseMode_4bit
	uint32_t DataCount;				// Ҫ��д���ݵ��ֽڸ�����0 ��ʾһֱ��дֱ���洢��ĩβ
	uint8_t  SendInstOnlyOnce;
} QSPI_CmdStructure;

#define QSPI_PhaseMode_None		0	// there is no this phase
#define QSPI_PhaseMode_1bit		1	// ���ߴ���
#define QSPI_PhaseMode_2bit		2	// ˫�ߴ���
#define QSPI_PhaseMode_4bit		3	// ���ߴ���

#define QSPI_PhaseSize_8bit		0
#define QSPI_PhaseSize_16bit	1
#define QSPI_PhaseSize_24bit	2
#define QSPI_PhaseSize_32bit	3


#define QSPI_Mode_IndirectWrite		0
#define QSPI_Mode_IndirectRead		1
#define QSPI_Mode_AutoPolling		2
#define QSPI_Mode_MemoryMapped		3


#define QSPI_CMD_READ_JEDEC			0x9F
#define QSPI_CMD_FAST_READ			0x0B
#define QSPI_CMD_FAST_READ_2bit		0x3B
#define QSPI_CMD_FAST_READ_IO2bit	0xBB
#define QSPI_CMD_FAST_READ_4bit		0x6B
#define QSPI_CMD_FAST_READ_IO4bit	0xEB
#define QSPI_CMD_WRITE_ENABLE		0x06
#define QSPI_CMD_WRITE_DISABLE		0x04
#define QSPI_CMD_PAGE_PROGRAM		0x02
#define QSPI_CMD_PAGE_PROGRAM_4bit	0x32
#define QSPI_CMD_ERASE_CHIP			0x60
#define QSPI_CMD_ERASE_SECTOR 		0x20
#define QSPI_CMD_ERASE_BLOCK32KB	0x52
#define QSPI_CMD_ERASE_BLOCK64KB	0xD8
#define QSPI_CMD_READ_STATUS_REG1	0x05
#define QSPI_CMD_READ_STATUS_REG2	0x35
#define QSPI_CMD_READ_STATUS_REG3	0x15
#define QSPI_CMD_WRITE_STATUS_REG1	0x01
#define QSPI_CMD_WRITE_STATUS_REG2	0x31
#define QSPI_CMD_WRITE_STATUS_REG3	0x11
#define QSPI_CMD_WRITE_EXT_ADDR		0xC5	// Write Extended Address Register
#define QSPI_CMD_READ_EXT_ADDR		0xC8
#define QSPI_CMD_4BYTE_ADDR_ENTER	0xB7
#define QSPI_CMD_4BYTE_ADDR_EXIT	0xE9

#define QSPI_STATUS_REG1_BUSY_Pos	0
#define QSPI_STATUS_REG2_QUAD_Pos	1


/* Interrupt Type */
#define QSPI_IT_ERR   	(1 << QSPI_CR_ERR_Pos)
#define QSPI_IT_DONE   	(1 << QSPI_CR_DONE_Pos)
#define QSPI_IT_FFTHR	(1 << QSPI_CR_FFTHR_Pos)
#define QSPI_IT_PSMAT	(1 << QSPI_CR_PSMAT_Pos)
#define QSPI_IT_TO		(1 << QSPI_CR_TOIE_Pos)



void QSPI_Init(QSPI_TypeDef * QSPIx, QSPI_InitStructure * initStruct);
void QSPI_Open(QSPI_TypeDef * QSPIx);
void QSPI_Close(QSPI_TypeDef * QSPIx);

void QSPI_CmdStructClear(QSPI_CmdStructure * cmdStruct);
void QSPI_Command(QSPI_TypeDef * QSPIx, uint8_t cmdMode, QSPI_CmdStructure * cmdStruct);

void QSPI_Erase(QSPI_TypeDef * QSPIx, uint8_t cmd, uint32_t addr, uint8_t wait);
void QSPI_Write_(QSPI_TypeDef * QSPIx, uint32_t addr, uint8_t buff[], uint32_t count, uint8_t data_width);
#define QSPI_Write(QSPIx, addr, buff, count)	   QSPI_Write_(QSPIx, (addr), (buff), (count), 1)
#define QSPI_Write_4bit(QSPIx, addr, buff, count)  QSPI_Write_(QSPIx, (addr), (buff), (count), 4)
void QSPI_Read_(QSPI_TypeDef * QSPIx, uint32_t addr, uint8_t buff[], uint32_t count, uint8_t addr_width, uint8_t data_width);
#define QSPI_Read(QSPIx, addr, buff, count)			QSPI_Read_(QSPIx, (addr), (buff), (count), 1, 1);
#define QSPI_Read_2bit(QSPIx, addr, buff, count)	QSPI_Read_(QSPIx, (addr), (buff), (count), 1, 2);
#define QSPI_Read_4bit(QSPIx, addr, buff, count)	QSPI_Read_(QSPIx, (addr), (buff), (count), 1, 4);
#define QSPI_Read_IO2bit(QSPIx, addr, buff, count)	QSPI_Read_(QSPIx, (addr), (buff), (count), 2, 2);
#define QSPI_Read_IO4bit(QSPIx, addr, buff, count)	QSPI_Read_(QSPIx, (addr), (buff), (count), 4, 4);

void QSPI_MemoryMap(QSPI_TypeDef * QSPIx, uint8_t addr_width, uint8_t data_width);
void QSPI_MemoryMapClose(QSPI_TypeDef * QSPIx);

bool QSPI_FlashBusy(QSPI_TypeDef * QSPIx);
uint8_t QSPI_QuadState(QSPI_TypeDef * QSPIx);
void QSPI_QuadSwitch(QSPI_TypeDef * QSPIx, uint8_t on);
void QSPI_SendCmd(QSPI_TypeDef * QSPIx, uint8_t cmd);
uint32_t QSPI_ReadReg(QSPI_TypeDef * QSPIx, uint8_t cmd, uint8_t n_bytes);
void QSPI_WriteReg(QSPI_TypeDef * QSPIx, uint8_t cmd, uint32_t data, uint8_t n_bytes);

#define QSPI_ReadJEDEC(QSPIx)			QSPI_ReadReg(QSPIx, QSPI_CMD_READ_JEDEC, 3)
#define QSPI_WriteEnable(QSPIx)			QSPI_SendCmd(QSPIx, QSPI_CMD_WRITE_ENABLE)
#define QSPI_WriteDisable(QSPIx)		QSPI_SendCmd(QSPIx, QSPI_CMD_WRITE_DISABLE)
#define QSPI_4ByteAddrEnable(QSPIx)		QSPI_SendCmd(QSPIx, QSPI_CMD_4BYTE_ADDR_ENTER)
#define QSPI_4ByteAddrDisable(QSPIx)	QSPI_SendCmd(QSPIx, QSPI_CMD_4BYTE_ADDR_EXIT)


static inline bool QSPI_Busy(QSPI_TypeDef * QSPIx)
{
	return QSPIx->SR & QSPI_SR_BUSY_Msk;
}

static inline uint32_t QSPI_FIFOCount(QSPI_TypeDef * QSPIx)
{
	return (QSPIx->SR & QSPI_SR_FFLVL_Msk) >> QSPI_SR_FFLVL_Pos;
}

static inline uint32_t QSPI_FIFOSpace(QSPI_TypeDef * QSPIx)
{
	return 32 - QSPI_FIFOCount(QSPIx);
}

static inline bool QSPI_FIFOEmpty(QSPI_TypeDef * QSPIx)
{
	return QSPI_FIFOCount(QSPIx) == 0;
}

static inline void QSPI_BankSwitch(QSPI_TypeDef * QSPIx, uint8_t bank)
{
	QSPIx->CR &= ~(QSPI_CR_DUAL_Msk | QSPI_CR_BANK_Msk);
	QSPIx->CR |=  (bank << QSPI_CR_DUAL_Pos);
}

static inline void QSPI_CacheClear(QSPI_TypeDef * QSPIx)
{
	QSPIx->CACHE = QSPI_CACHE_CLR_Msk;
}

void QSPI_INTEn(QSPI_TypeDef * QSPIx, uint32_t it);
void QSPI_INTDis(QSPI_TypeDef * QSPIx, uint32_t it);
void QSPI_INTClr(QSPI_TypeDef * QSPIx, uint32_t it);
uint32_t QSPI_INTStat(QSPI_TypeDef * QSPIx, uint32_t it);


#endif //__SWM221_QSPI_H__