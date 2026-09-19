#include <string.h>
#include "SWM221.h"
#include "W25N01G.h"


#define RW_PAGE	64


#define N_DATA  48

uint8_t RdBuff[W25N_PAGE_SIZE] = {0};
uint8_t WrBuff[W25N_PAGE_SIZE] = {
	0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
	0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
	0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
};


void W25N01G_Write_DMA(uint32_t page, uint8_t buff[2048], uint8_t data_width);
void W25N01G_Read_DMA(uint32_t page, uint8_t buff[2048], uint8_t addr_width, uint8_t data_width);
void SerialInit(void);

int main(void)
{
	int i;
	
	SystemInit();
	
	SerialInit();
	
	W25N01G_Init();
	
	int id = W25N01G_ReadJEDEC();
	printf("SPI Flash JEDEC: %06X\n", id);
	
	W25N01G_FlashProtect(W25N_PROTECT_Upper_1MB);
	
	
	W25N01G_Erase(RW_PAGE, 1);
	
	memset(RdBuff, 0x00, W25N_PAGE_SIZE);
	W25N01G_Read(RW_PAGE, RdBuff);
	
	printf("\n\nAfter Erase: \n");
	for(i = 0; i < N_DATA; i++) printf("0x%02X, ", RdBuff[i]);
	
	
	W25N01G_Write(RW_PAGE, WrBuff);
	
	memset(RdBuff, 0x00, W25N_PAGE_SIZE);
	W25N01G_Read(RW_PAGE, RdBuff);
	
	printf("\n\nAfter Write: \n");
	for(i = 0; i < N_DATA; i++) printf("0x%02X, ", RdBuff[i]);
	
	
	memset(RdBuff, 0x00, W25N_PAGE_SIZE);
	W25N01G_Read_2bit(RW_PAGE, RdBuff);
	
	printf("\n\nDual Read: \n");
	for(i = 0; i < N_DATA; i++) printf("0x%02X, ", RdBuff[i]);
	
	
	memset(RdBuff, 0x00, W25N_PAGE_SIZE);
	W25N01G_Read_IO2bit(RW_PAGE, RdBuff);
	
	printf("\n\nDual IO Read: \n");
	for(i = 0; i < N_DATA; i++) printf("0x%02X, ", RdBuff[i]);
	
	
	W25N01G_Erase(RW_PAGE, 1);
	W25N01G_Write_4bit(RW_PAGE, WrBuff);
	
	memset(RdBuff, 0x00, W25N_PAGE_SIZE);
	W25N01G_Read_4bit(RW_PAGE, RdBuff);
	
	printf("\n\nQuad Read: \n");
	for(i = 0; i < N_DATA; i++) printf("0x%02X, ", RdBuff[i]);
	
	
	memset(RdBuff, 0x00, W25N_PAGE_SIZE);
	W25N01G_Read_IO4bit(RW_PAGE, RdBuff);
	
	printf("\n\nQuad IO Read: \n");
	for(i = 0; i < N_DATA; i++) printf("0x%02X, ", RdBuff[i]);
	
	
	W25N01G_Erase(RW_PAGE, 1);
	W25N01G_Write_DMA(RW_PAGE, WrBuff, 4);
	
	memset(RdBuff, 0x00, W25N_PAGE_SIZE);
	W25N01G_Read_DMA(RW_PAGE, RdBuff, 4, 4);
	
	printf("\n\nDMA Read: \n");
	for(i = 0; i < N_DATA; i++) printf("0x%02X, ", RdBuff[i]);
	
	
	/* 2MB Read/write check */
#if 1
	uint32_t addr;
	uint32_t *wrbuf = (uint32_t *)WrBuff;
	uint32_t *rdbuf = (uint32_t *)RdBuff;
	
	for(addr = 0; addr < 0x200000; addr += W25N_BLOCK_SIZE)
	{
		W25N01G_Erase(addr / W25N_PAGE_SIZE, 1);
	}
	
	for(addr = 0; addr < 0x200000; addr += W25N_PAGE_SIZE)
	{
		for(i = 0; i < W25N_PAGE_SIZE; i += 4)
			wrbuf[i / 4] = addr + i;
		
		W25N01G_Write_4bit(addr / W25N_PAGE_SIZE, (uint8_t *)wrbuf);
	}
	
	for(addr = 0; addr < 0x200000; addr += W25N_PAGE_SIZE)
	{
		W25N01G_Read_4bit(addr / W25N_PAGE_SIZE, (uint8_t *)rdbuf);
		
		for(i = 0; i < W25N_PAGE_SIZE; i += 4)
		{
			if(rdbuf[i / 4] != addr + i)
			{
				printf("\n\nError: expected 0x%08X, get 0x%08X\n", addr + i, rdbuf[i / 4]);
				while(1) __NOP();
			}
		}
	}
	
	if(addr == 0x200000)
		printf("\n\nPass\n");
#endif
	
	
	while(1==1)
	{
	}
}


void W25N01G_Write_DMA(uint32_t page, uint8_t buff[2048], uint8_t data_width)
{
	static bool dma_inited = false;
	
	if(!dma_inited)
	{
		DMA_InitStructure DMA_initStruct;
		
		DMA_initStruct.Mode = DMA_MODE_SINGLE;
		DMA_initStruct.Unit = DMA_UNIT_BYTE;
		DMA_initStruct.Count = 2048;
		DMA_initStruct.MemoryAddr = (uint32_t)buff;
		DMA_initStruct.MemoryAddrInc = 1;
		DMA_initStruct.PeripheralAddr = (uint32_t)&QSPI0->DRB;
		DMA_initStruct.PeripheralAddrInc = 0;
		DMA_initStruct.Handshake = DMA_CH0_QSPI0TX;
		DMA_initStruct.Priority = DMA_PRI_LOW;
		DMA_initStruct.INTEn = 0;
		DMA_CH_Init(DMA_CH0, &DMA_initStruct);
		
		dma_inited = true;
	}
	
	DMA_CH_SetAddrAndCount(DMA_CH0, (uint32_t)buff, 2048);
	
	QSPI_DMAEnable(QSPI0, QSPI_Mode_IndirectWrite);
	
	W25N01G_Write_(page, buff, data_width, 0);
	
	DMA_CH_Open(DMA_CH0);
	
	while(DMA_CH_INTStat(DMA_CH0, DMA_IT_DONE) == 0) __NOP();
    DMA_CH_INTClr(DMA_CH0, DMA_IT_DONE);
	
	/* When QSPI busy, writing to the QSPI->CR register is invalid */
	while(QSPI_Busy(QSPI0)) __NOP();
	
	QSPI_DMADisable(QSPI0);
	
	W25N01G_Program_Execute(page);
	
	while(W25N01G_FlashBusy()) __NOP();
}


void W25N01G_Read_DMA(uint32_t page, uint8_t buff[2048], uint8_t addr_width, uint8_t data_width)
{
	static bool dma_inited = false;
	
	if(!dma_inited)
	{
		DMA_InitStructure DMA_initStruct;
		
		DMA_initStruct.Mode = DMA_MODE_SINGLE;
		DMA_initStruct.Unit = DMA_UNIT_BYTE;
		DMA_initStruct.Count = 2048;
		DMA_initStruct.MemoryAddr = (uint32_t)buff;
		DMA_initStruct.MemoryAddrInc = 1;
		DMA_initStruct.PeripheralAddr = (uint32_t)&QSPI0->DRB;
		DMA_initStruct.PeripheralAddrInc = 0;
		DMA_initStruct.Handshake = DMA_CH1_QSPI0RX;
		DMA_initStruct.Priority = DMA_PRI_LOW;
		DMA_initStruct.INTEn = 0;
		DMA_CH_Init(DMA_CH1, &DMA_initStruct);
		
		dma_inited = true;
	}
	
	DMA_CH_SetAddrAndCount(DMA_CH1, (uint32_t)buff, 2048);
	
	QSPI_DMAEnable(QSPI0, QSPI_Mode_IndirectRead);
	
	W25N01G_Read_(page, buff, addr_width, data_width, 0);
	
	DMA_CH_Open(DMA_CH1);
	
	while(DMA_CH_INTStat(DMA_CH1, DMA_IT_DONE) == 0) __NOP();
    DMA_CH_INTClr(DMA_CH1, DMA_IT_DONE);
	
	QSPI_DMADisable(QSPI0);
	
	QSPI_Abort(QSPI0);
}


void SerialInit(void)
{
	UART_InitStructure UART_initStruct;
	
	PORT_Init(PORTA, PIN0, PORTA_PIN0_UART0_RX, 1);	//GPIOA.0配置为UART0 RXD
	PORT_Init(PORTA, PIN1, PORTA_PIN1_UART0_TX, 0);	//GPIOA.1配置为UART0 TXD
 	
 	UART_initStruct.Baudrate = 57600;
	UART_initStruct.DataBits = UART_DATA_8BIT;
	UART_initStruct.Parity = UART_PARITY_NONE;
	UART_initStruct.StopBits = UART_STOP_1BIT;
	UART_initStruct.RXThreshold = 3;
	UART_initStruct.RXThresholdIEn = 0;
	UART_initStruct.TXThreshold = 3;
	UART_initStruct.TXThresholdIEn = 0;
	UART_initStruct.TimeoutTime = 10;
	UART_initStruct.TimeoutIEn = 0;
 	UART_Init(UART0, &UART_initStruct);
	UART_Open(UART0);
}

int fputc(int ch, FILE *f)
{
	UART_WriteByte(UART0, ch);
	
	while(UART_IsTXBusy(UART0));
 	
	return ch;
}
