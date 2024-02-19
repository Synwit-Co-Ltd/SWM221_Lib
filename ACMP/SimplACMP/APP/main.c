#include "SWM221.h"

void SerialInit(void);

#define ACMP_VN_DAC

int main(void)
{
	SystemInit();
	
	SerialInit();
	
	PORT_Init(PORTB, PIN6,  PORTB_PIN6_ACMP0_INP0, 0);
	PORT_Init(PORTB, PIN5,  PORTB_PIN5_ACMP0_INP1, 0);
	PORT_Init(PORTB, PIN4,  PORTB_PIN4_ACMP0_INP2, 0);
	PORT_Init(PORTB, PIN3,  PORTB_PIN3_ACMP0_INN,  0);
	PORT_Init(PORTA, PIN14, PORTA_PIN14_ACMP1_INP, 0);
	PORT_Init(PORTA, PIN12, PORTA_PIN12_ACMP1_INN, 0);
	
	SYS->ACMP0CR |= (1 << SYS_ACMP0CR_EN_Pos)    |
				    (1 << SYS_ACMP0CR_HYS_Pos)   |
#ifdef ACMP_VN_DAC
					(1 << SYS_ACMP0CR_VNSEL_Pos) |
#else
					(0 << SYS_ACMP0CR_VNSEL_Pos) |
#endif
					(0 << SYS_ACMP0CR_VPSEL_Pos) |
				    (1 << SYS_ACMP0CR_IE_Pos);
	
#ifdef ACMP_VN_DAC
	SYS->ACMPCR = (1   << SYS_ACMPCR_DACEN_Pos) |
				  (0   << SYS_ACMPCR_DACVR_Pos) |
				  (100 << SYS_ACMPCR_DACDR_Pos);	// Vout = 100 / 255 * VDD
#endif

	NVIC_EnableIRQ(ACMP_IRQn);
	
	while(1==1)
	{
		printf("ACMP0->OUT = %d\r\n", (SYS->ACMPSR & SYS_ACMPSR_CMP0OUT_Msk) ? 1 : 0);
		
		for(int i = 0; i < SystemCoreClock/10; i++) __NOP();
	}
}


void ACMP_Handler(void)
{
	if(SYS->ACMPSR & SYS_ACMPSR_CMP0IF_Msk)
	{
		SYS->ACMPSR = (1 << SYS_ACMPSR_CMP0IF_Pos);
		
		printf("ACMP0 Interrupt Happened\r\n");
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
	UART_initStruct.RXThresholdIEn = 0;
	UART_initStruct.TXThresholdIEn = 0;
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
