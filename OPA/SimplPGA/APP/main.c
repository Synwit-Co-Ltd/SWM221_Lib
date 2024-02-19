#include "SWM221.h"

void SerialInit(void);

int main(void)
{	
	SystemInit();
	
	SerialInit();
	
	PORT_Init(PORTB, PIN8,  PORTB_PIN8_OPA0_INN,  0);
	PORT_Init(PORTB, PIN7,  PORTB_PIN7_OPA0_INP,  0);
	PORT_Init(PORTB, PIN9,  PORTB_PIN9_OPA0_OUT,  0);
	PORT_Init(PORTA, PIN13, PORTA_PIN13_OPA1_INN, 0);
	PORT_Init(PORTA, PIN9,  PORTA_PIN9_OPA1_INP,  0);
	PORT_Init(PORTA, PIN8,  PORTA_PIN8_OPA1_OUT,  0);
	PORT_Init(PORTB, PIN0,  PORTB_PIN0_OPA2_INN,  0);
	PORT_Init(PORTB, PIN1,  PORTB_PIN1_OPA2_INP,  0);
	PORT_Init(PORTB, PIN2,  PORTB_PIN2_OPA2_OUT,  0);
	
	SYS->PGA0CR = (1 << SYS_PGA0CR_EN_Pos) |
				  (1 << SYS_PGA0CR_MODE_Pos) |
				  (2 << SYS_PGA0CR_GAIN_Pos) |		// PGA ����ѡ��0 x1   1 x5   2 x10   3 x10
				  (2 << SYS_PGA0CR_ROUT_Pos);		// �������ѡ��0 open   1 100   2 1k   3 10k
	
	SYS->PGA1CR = (1 << SYS_PGA1CR_EN_Pos) |
				  (1 << SYS_PGA1CR_MODE_Pos) |
				  (2 << SYS_PGA1CR_GAIN_Pos) |
				  (2 << SYS_PGA1CR_ROUT_Pos);
	
	SYS->PGA2CR = (1 << SYS_PGA2CR_EN_Pos) |
				  (1 << SYS_PGA2CR_MODE_Pos) |
				  (2 << SYS_PGA2CR_GAIN_Pos) |
				  (2 << SYS_PGA2CR_ROUT_Pos);
	
	while(1==1)
	{
	}
}


void SerialInit(void)
{
	UART_InitStructure UART_initStruct;
	
	PORT_Init(PORTA, PIN0, PORTA_PIN0_UART0_RX, 0);	//GPIOA.0����ΪUART0 RXD
	PORT_Init(PORTA, PIN1, PORTA_PIN1_UART0_TX, 1);	//GPIOA.1����ΪUART0 TXD
 	
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
* ��������: fputc()
* ����˵��: printf()ʹ�ô˺������ʵ�ʵĴ��ڴ�ӡ����
* ��    ��: int ch		Ҫ��ӡ���ַ�
*			FILE *f		�ļ����
* ��    ��: ��
* ע������: ��
******************************************************************************************************************************************/
int fputc(int ch, FILE *f)
{
	UART_WriteByte(UART0, ch);
	
	while(UART_IsTXBusy(UART0));
 	
	return ch;
}