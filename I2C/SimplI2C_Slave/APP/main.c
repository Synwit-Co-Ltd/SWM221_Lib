#include "SWM221.h"

#include <string.h>
#include "IOI2C_MST.h"

#define SLV_ADDR  0x6C

char mst_txbuff[4] = {0x37, 0x55, 0xAA, 0x78};
char mst_rxbuff[4] = {0};
char slv_rxbuff[5] = {0};	//第一个数据是地址
char slv_txbuff[4] = {0};

volatile uint32_t slv_rxindx = 0;
volatile uint32_t slv_txindx = 0;

void SerialInit(void);
void I2C_Slv_Init(void);

int main(void)
{	
	uint32_t i;
	
	SystemInit();
	
	SerialInit();
	
	IOI2C_MST_Init(100000);
	
	I2C_Slv_Init();
	
	while(1==1)
	{
		IOI2C_MST_Send(SLV_ADDR, mst_txbuff, 4);
		while(IOI2C_MST_IsBusy());
		
		printf("Master Send %X %X %X %X\r\n", mst_txbuff[0], mst_txbuff[1], mst_txbuff[2], mst_txbuff[3]);
		
		IOI2C_MST_Read(SLV_ADDR, mst_rxbuff, 4);
		while(IOI2C_MST_IsBusy());
		
		printf("Master Read %X %X %X %X\r\n", mst_rxbuff[0], mst_rxbuff[1], mst_rxbuff[2], mst_rxbuff[3]);
		
		if((mst_txbuff[0] == mst_rxbuff[0]) && (mst_txbuff[1] == mst_rxbuff[1]) && (mst_txbuff[2] == mst_rxbuff[2]) && (mst_txbuff[3] == mst_rxbuff[3]))
			printf("Success\r\n");
		else
			printf("Fail\r\n");
		
		for(i = 0; i < SystemCoreClock/10; i++) __NOP();
	}
}


void I2C_Slv_Init(void)
{
	I2C_InitStructure I2C_initStruct;
	
	PORT_Init(PORTA, PIN6, PORTA_PIN6_I2C0_SCL, 1);		//GPIOA.6配置为I2C0 SCL引脚
	PORTA->OPEND |= (1 << PIN6);						//开漏输出
	PORTA->PULLU |= (1 << PIN6);						//使能上拉
	PORT_Init(PORTA, PIN7, PORTA_PIN7_I2C0_SDA, 1);		//GPIOA.7配置为I2C0 SDA引脚
	PORTA->OPEND |= (1 << PIN7);						//开漏输出
	PORTA->PULLU |= (1 << PIN7);						//使能上拉
	
	I2C_initStruct.Master = 0;
	I2C_initStruct.MstClk = 0;
	I2C_initStruct.Addr10b = 0;
	I2C_initStruct.SlvAddr = SLV_ADDR;
	I2C_initStruct.TXEmptyIEn = 1;
	I2C_initStruct.RXNotEmptyIEn = 1;
	I2C_initStruct.SlvSTADetIEn  = 1;
	I2C_initStruct.SlvSTODetIEn  = 1;
	I2C_Init(I2C0, &I2C_initStruct);
	
	I2C_Open(I2C0);
}


void I2C0_Handler(void)
{
	uint32_t i;
	
	if(I2C0->IF & I2C_IF_RXSTA_Msk)					//收到起始位
	{
		I2C0->IF = (1 << I2C_IF_RXSTA_Pos);
		
		slv_rxindx = 0;
		
		I2C0->TR = (1 << I2C_TR_TXCLR_Pos);			//有数据时无法写入
		I2C0->TXDATA = slv_txbuff[0];
		slv_txindx = 1;
	}
	else if(I2C0->IF & I2C_IF_RXNE_Msk)				//接收寄存器非空
	{
		slv_rxbuff[slv_rxindx] = I2C0->RXDATA;
		if(slv_rxindx < 4) slv_rxindx++;
	}
	else if(I2C0->IF & I2C_IF_TXE_Msk)				//发送寄存器空
	{
		I2C0->TXDATA = slv_txbuff[slv_txindx];
		if(slv_txindx < 3) slv_txindx++;
	}
	else if(I2C0->IF & I2C_IF_RXSTO_Msk)			//收到停止位
	{
		I2C0->IF = (1 << I2C_IF_RXSTO_Pos);
		
		for(i = 0; i < 4; i++) slv_txbuff[i] = slv_rxbuff[1+i];
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
