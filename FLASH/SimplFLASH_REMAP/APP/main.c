#include "SWM221.h"

/* 注意：main.c中的main()函数必须在RAM中执行，因为其中会REMAP操作，代码所在Flash可能会被重映射到其他地址
*/


#define EEPROM_ADDR	  0x7000

uint32_t WrBuff[20] = {0x14141414, 0x15151515, 0x16161616, 0x17171717, 0x18181818,
				       0x19191919, 0x1A1A1A1A, 0x1B1B1B1B, 0x1C1C1C1C, 0x1D1D1D1D,
				       0x1E1E1E1E, 0x1F1F1F1F, 0x20202020, 0x21212121, 0x22222222,
				       0x23232323, 0x24242424, 0x25252525, 0x26262626, 0x27272727};
uint32_t RdBuff[20];

void SerialInit(void);

int main(void)
{
	uint32_t i;
	
	SystemInit();
	
	SerialInit();
	
	
	printf("\r\nAfter Erase: \r\n");
	
	FLASH_Erase(EEPROM_ADDR);
	
	/* 将EEPROM_ADDR处的Flash映射到0地址，然后从0地址读取数据 */
	FMC->REMAP = (1 << FMC_REMAP_ON_Pos) | ((EEPROM_ADDR/0x800) << FMC_REMAP_OFFSET_Pos);
	FMC->CACHE |= FMC_CACHE_CCLR_Msk;

	for(i = 0; i < 20; i++) RdBuff[i] = ((volatile uint32_t *)0x00)[i];
	FMC->REMAP = (0 << FMC_REMAP_ON_Pos);
	FMC->CACHE |= FMC_CACHE_CCLR_Msk;

	for(i = 0; i < 20; i++) printf("0x%08X, ", RdBuff[i]);

	
	printf("\r\nAfter Write: \r\n");
   	
	FLASH_Write(EEPROM_ADDR, WrBuff, 20);
	
	/* 将EEPROM_ADDR处的Flash映射到0地址，然后从0地址读取数据 */
	FMC->REMAP = (1 << FMC_REMAP_ON_Pos) | ((EEPROM_ADDR/0x800) << FMC_REMAP_OFFSET_Pos);
	FMC->CACHE |= FMC_CACHE_CCLR_Msk;

	for(i = 0; i < 20; i++) RdBuff[i] = ((volatile uint32_t *)0x00)[i];
	FMC->REMAP = (0 << FMC_REMAP_ON_Pos);
	FMC->CACHE |= FMC_CACHE_CCLR_Msk;

	for(i = 0; i < 20; i++) printf("0x%08X, ", RdBuff[i]);
   	
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
