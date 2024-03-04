#include "SWM221.h"
#include "IOI2C_SLV.h"

#define SLV_ADDR  0x6C

char mst_txbuff[4] = {0x37, 0x55, 0xAA, 0x78};
char mst_rxbuff[4] = {0};

volatile uint32_t mst_txindx = 0;
volatile uint32_t mst_rxindx = 0;

#define MST_OP_TX	  1
#define MST_OP_RX	  2
volatile uint32_t mst_oper = 0;

#define MST_RES_SUCC  1
#define MST_RES_FAIL  2
volatile uint32_t mst_result = 0;

void SerialInit(void);
void I2C_Mst_Init(void);

int main(void)
{
	uint32_t i;
	
	SystemInit();
	
	SerialInit();
	
	IOI2C_SLV_Init();
	
	I2C_Mst_Init();
	
	NVIC_SetPriority(I2C0_IRQn, 3);	// 设置从机优先级比主机高，因为若从机不能及时处理数据会响应NAK
		
	while(1==1)
	{
		/*************************** Master Write ************************************/
		mst_oper = MST_OP_TX;
		mst_result = 0;
		mst_txindx = 0;
		
		I2C_Start(I2C0, (SLV_ADDR << 1) | 0, 0);
		
		while(mst_result != MST_RES_SUCC)
		{
			if(mst_result == MST_RES_FAIL)
				goto nextloop;
		}
		
		printf("Master Send %X %X %X %X\r\n", mst_txbuff[0], mst_txbuff[1], mst_txbuff[2], mst_txbuff[3]);
		
		/********************************** Master Read *******************************/
		mst_oper = MST_OP_RX;
		mst_result = 0;
		mst_rxindx = 0;
		
		I2C_Start(I2C0, (SLV_ADDR << 1) | 1, 0);
		
		while(mst_result != MST_RES_SUCC)
		{
			if(mst_result == MST_RES_FAIL)
				goto nextloop;
		}
		
		printf("Master Read %X %X %X %X\r\n", mst_rxbuff[0], mst_rxbuff[1], mst_rxbuff[2], mst_rxbuff[3]);
		
		if((mst_txbuff[0] == mst_rxbuff[0]) && (mst_txbuff[1] == mst_rxbuff[1]) && (mst_txbuff[2] == mst_rxbuff[2]) && (mst_txbuff[3] == mst_rxbuff[3]))
			printf("Success\r\n");
		else
			printf("Fail\r\n");

nextloop:
		I2C_Stop(I2C0, 1);
		for(i = 0; i < SystemCoreClock/3; i++) __NOP();
	}
}


void I2C_Mst_Init(void)
{
	I2C_InitStructure I2C_initStruct;
	
	PORT_Init(PORTA, PIN6, PORTA_PIN6_I2C0_SCL, 1);		//GPIOA.6配置为I2C0 SCL引脚
	PORTA->OPEND |= (1 << PIN6);						//开漏输出
	PORTA->PULLU |= (1 << PIN6);						//使能上拉
	PORT_Init(PORTA, PIN7, PORTA_PIN7_I2C0_SDA, 1);		//GPIOA.7配置为I2C0 SDA引脚
	PORTA->OPEND |= (1 << PIN7);						//开漏输出
	PORTA->PULLU |= (1 << PIN7);						//使能上拉
	
	I2C_initStruct.Master = 1;
	I2C_initStruct.MstClk = 100000;
	I2C_initStruct.Addr10b = 0;
	I2C_initStruct.TXEmptyIEn = 0;
	I2C_initStruct.RXNotEmptyIEn = 0;
	I2C_Init(I2C0, &I2C_initStruct);
	
	I2C_INTEn(I2C0, I2C_IT_TX_DONE);
	/* 注意：此例程中不能使用 RX_NOT_EMPTY 中断替代 RX_DONE 中断，
	         因为当 RX_NOT_EMPTY 发生时，接收还未完成，I2C0->MCR.RD 还没清零，
	         这时候进入中断执行 I2C_Read() 设置 I2C0->MCR.RD 位没有作用，导致无法接收下一个数据
	*/
	I2C_INTEn(I2C0, I2C_IT_RX_DONE);
	NVIC_EnableIRQ(I2C0_IRQn);
	
	I2C_Open(I2C0);
}

void I2C0_Handler(void)
{
	if(I2C0->IF & I2C_IF_TXDONE_Msk)			//发送完成
	{
		I2C0->IF = I2C_IF_TXDONE_Msk;
		
		if(mst_oper == MST_OP_TX)
		{
			if(I2C_IsAck(I2C0))
			{
				if(mst_txindx < 4)
				{
					I2C_Write(I2C0, mst_txbuff[mst_txindx], 0);
					
					mst_txindx++;
				}
				else
				{
					I2C_Stop(I2C0, 0);
					
					mst_result = MST_RES_SUCC;
				}
			}
			else
			{
				if(mst_txindx == 0)
					printf("Slave send NACK for address\r\n");
				else
					printf("Slave send NACK for data\r\n");
				
				mst_result = MST_RES_FAIL;
			}
		}
		else // mst_oper == MST_OP_RX
		{
			if(I2C_IsAck(I2C0))
			{
				I2C_Read(I2C0, 1, 0);
			}
			else
			{
				printf("Slave send NACK for address\r\n");
				
				mst_result = MST_RES_FAIL;
			}
		}
	}
	else if(I2C0->IF & I2C_IF_RXDONE_Msk)		//接收完成
	{
		I2C0->IF = I2C_IF_RXDONE_Msk;
		
		if(mst_rxindx < 4)
		{
			mst_rxbuff[mst_rxindx] = I2C0->RXDATA;
			
			mst_rxindx++;
			
			if(mst_rxindx < 3)
			{
				I2C_Read(I2C0, 1, 0);
			}
			else if(mst_rxindx < 4)
			{
				I2C_Read(I2C0, 0, 0);
			}
			else
			{
				I2C_Stop(I2C0, 0);
				
				mst_result = MST_RES_SUCC;
			}
		}
	}
}


uint32_t IOI2C_SLV_EmptyCallback(char *TxBuff)
{
	uint32_t i = 0;
	
	while(IOI2C_SLV_RxNotEmpty())
		TxBuff[i++] = IOI2C_SLV_FetchByte();
	
	return i;
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
