#include "SWM221.h"


#define EEPROM_ADDR	  0x008000


#define RWLEN  48
uint8_t WrBuff[RWLEN] = {0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
						 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
						 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F};
uint8_t RdBuff[RWLEN] = {0};


void SerialInit(void);

int main(void)
{
	int i;
	QSPI_InitStructure QSPI_initStruct;
	
	SystemInit();
	
	SerialInit();
	
	PORT_Init(PORTC, PIN3,  PORTC_PIN3_QSPI0_SCLK,  0);
	PORT_Init(PORTC, PIN2,  PORTC_PIN2_QSPI0_SSEL,  0);
	PORT_Init(PORTA, PIN15, PORTA_PIN15_QSPI0_MOSI, 1);
	PORT_Init(PORTB, PIN0,  PORTB_PIN0_QSPI0_MISO,  1);
	PORT_Init(PORTB, PIN1,  PORTB_PIN1_QSPI0_D2,    1);
	PORT_Init(PORTB, PIN2,  PORTB_PIN2_QSPI0_D3,    1);
	
	QSPI_initStruct.Size = QSPI_Size_16MB;
	QSPI_initStruct.ClkDiv = 4;
	QSPI_initStruct.ClkMode = QSPI_ClkMode_3;
	QSPI_initStruct.SampleShift = QSPI_SampleShift_NONE;
	QSPI_initStruct.IntEn = 0;
	QSPI_Init(QSPI0, &QSPI_initStruct);
	QSPI_Open(QSPI0);
	
	int id = QSPI_ReadJEDEC(QSPI0);
	printf("SPI Flash JEDEC: %06X\n", id);
	
	
	int quad = QSPI_QuadState(QSPI0);
	if(quad == 0)
	{
		QSPI_QuadSwitch(QSPI0, 1);
		
		quad = QSPI_QuadState(QSPI0);
	}
	printf("SPI Flash Quad %s\n", quad ? "enabled" : "disabled");
	
	
	QSPI_Erase(QSPI0, QSPI_CMD_ERASE_SECTOR, EEPROM_ADDR, 1);
	
	QSPI_Read(QSPI0, EEPROM_ADDR, RdBuff, RWLEN);
	
	printf("\n\nAfter Erase: \n");
	for(i = 0; i < RWLEN; i++) printf("0x%02X, ", RdBuff[i]);
	
	
	QSPI_Write(QSPI0, EEPROM_ADDR, WrBuff, RWLEN);
	
	QSPI_Read(QSPI0, EEPROM_ADDR, RdBuff, RWLEN);
	
	printf("\n\nAfter Write: \n");
	for(i = 0; i < RWLEN; i++) printf("0x%02X, ", RdBuff[i]);
	
	
	QSPI_Read_2bit(QSPI0, EEPROM_ADDR, RdBuff, RWLEN);
	
	printf("\n\nDual Read: \n");
	for(i = 0; i < RWLEN; i++) printf("0x%02X, ", RdBuff[i]);
	
	
	QSPI_Read_IO2bit(QSPI0, EEPROM_ADDR, RdBuff, RWLEN);
	
	printf("\n\nDual IO Read: \n");
	for(i = 0; i < RWLEN; i++) printf("0x%02X, ", RdBuff[i]);
	
	
	QSPI_Erase(QSPI0, QSPI_CMD_ERASE_SECTOR, EEPROM_ADDR, 1);
	QSPI_Write_4bit(QSPI0, EEPROM_ADDR, WrBuff, RWLEN);
	
	QSPI_Read_4bit(QSPI0, EEPROM_ADDR, RdBuff, RWLEN);
	
	printf("\n\nQuad Read: \n");
	for(i = 0; i < RWLEN; i++) printf("0x%02X, ", RdBuff[i]);
	
	
	QSPI_Read_IO4bit(QSPI0, EEPROM_ADDR, RdBuff, RWLEN);
	
	printf("\n\nQuad IO Read: \n");
	for(i = 0; i < RWLEN; i++) printf("0x%02X, ", RdBuff[i]);
   	
	while(1==1)
	{
	}
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

/****************************************************************************************************************************************** 
* 函数名称: fputc()
* 功能说明: printf()使用此函数完成实际的串口打印动作
* 输    入: int ch		要打印的字符
*			FILE *f		文件句柄
* 输    出: 无
* 注意事项: 无
******************************************************************************************************************************************/
int fputc(int ch, FILE *f)
{
	UART_WriteByte(UART0, ch);
	
	while(UART_IsTXBusy(UART0));
 	
	return ch;
}
