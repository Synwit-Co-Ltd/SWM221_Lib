#include "SWM221.h"

#define PIN_CS		PIN2
#define PIN_CLK		PIN3
#define PIN_MOSI	PIN4
#define PIN_MISO	PIN5


void IOSPI_Init(void)	// Mode: 3
{
	GPIO_Init(GPIOA, PIN_CS,   1, 0, 0, 0);
	GPIO_Init(GPIOA, PIN_CLK,  1, 0, 0, 0);
	GPIO_Init(GPIOA, PIN_MOSI, 1, 0, 0, 0);
	GPIO_Init(GPIOA, PIN_MISO, 0, 1, 0, 0);
	
	GPIO_SetBit(GPIOA, PIN_CS);
	GPIO_SetBit(GPIOA, PIN_CLK);
}

void IOSPI_CS_Low(void)
{
	GPIO_ClrBit(GPIOA, PIN_CS);
}

void IOSPI_CS_High(void)
{
	GPIO_SetBit(GPIOA, PIN_CS);
}

static void delay(uint32_t us)
{
	uint32_t i;
	
	for(i = 0; i < CyclesPerUs / 4 * us; i++) __NOP();
}

uint32_t IOSPI_ReadWrite(uint32_t data, uint32_t len)
{
	uint32_t i;
	uint32_t recv = 0;
	
	for(i = 0; i < len; i++)
	{
		GPIO_ClrBit(GPIOA, PIN_CLK);
		if(data & (1 << (len - 1 - i)))
			GPIO_SetBit(GPIOA, PIN_MOSI);
		else
			GPIO_ClrBit(GPIOA, PIN_MOSI);
		
		delay(5);
		
		GPIO_SetBit(GPIOA, PIN_CLK);
		if(GPIO_GetBit(GPIOA, PIN_MISO))
			recv |= (1 << (len - 1 - i));
		
		delay(5);
	}
	
	return recv;
}
