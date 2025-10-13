#include "SWM221.h"

#include "IOI2C_MST.h"

static char TxBuff[128] = {0};
static uint32_t TxSize = 0;
static volatile uint32_t TxIndx = 0;

static char *RxBuff = 0;
static uint32_t RxSize = 0;
static volatile uint32_t RxIndx = 0;

static volatile uint32_t BitTim = 0;

static volatile uint32_t IsRead = 0;

static volatile uint32_t IsBusy = 0;


static void (*IOI2C_MST_Func)(void);

static void IOI2C_MST_Start(void);
static void IOI2C_MST_SendByte(void);
static void IOI2C_MST_ReadByte(void);
static void IOI2C_MST_Stop(void);

static void delayuS(uint32_t dly) { uint32_t i; for(i = 0; i < SystemCoreClock/1000000 * dly; i++) __NOP(); }

/****************************************************************************************************************************************** 
* 函数名称:	IOI2C_MST_Init()
* 功能说明: IOI2C主机初始化
* 输    入: uint32_t baud		IOI2C主机波特率，即位传输速率，单位bps
* 输    出: 无
* 注意事项: 无
******************************************************************************************************************************************/
void IOI2C_MST_Init(uint32_t baud)
{
#define IOI2C_PORT	GPIOA
#define IOI2C_SCL	PIN2
#define IOI2C_SDA	PIN3
#define IOI2C_TMR	BTIMR0
#define IOI2C_ISR	BTIMR0_Handler
	
	GPIO_Init(IOI2C_PORT, IOI2C_SCL, 1, 0, 0, 0);
#define I2C_SCL_LOW()		{ GPIO_ClrBit(IOI2C_PORT, IOI2C_SCL); delayuS(1); }
#define I2C_SCL_HIGH()		{ GPIO_SetBit(IOI2C_PORT, IOI2C_SCL); delayuS(1); }

	GPIO_Init(IOI2C_PORT, IOI2C_SDA, 0, 1, 0, 1);
#define I2C_SDA_OUTPUT()	IOI2C_PORT->DIR |= (1 << IOI2C_SDA);
#define I2C_SDA_LOW()		GPIO_ClrBit(IOI2C_PORT, IOI2C_SDA)
#define I2C_SDA_HIGH()		GPIO_SetBit(IOI2C_PORT, IOI2C_SDA)

#define I2C_SDA_INPUT()		IOI2C_PORT->DIR &=~(1 << IOI2C_SDA)
#define I2C_SDA_Value()		GPIO_GetBit(IOI2C_PORT, IOI2C_SDA)

	I2C_SDA_HIGH();
	I2C_SCL_HIGH();
	
	TIMR_Init(IOI2C_TMR, TIMR_MODE_TIMER, 10, SystemCoreClock/10/(baud*2), 1);	//波特率为baud，则时钟翻转速率为baud*2
	
	NVIC_SetPriority(BTIMR0_IRQn, 3);	// 将I2C主机的中断优先级设置的比从机低，因为从机来不及处理的话会返回NAK
}

/****************************************************************************************************************************************** 
* 函数名称:	IOI2C_MST_Send()
* 功能说明: IOI2C主机发送数据
* 输    入: char slvaddr	从机地址
*			char buff[]		要发送的数据
*			uint32_t size		要发送的数据的个数，最多128个
* 输    出: 无
* 注意事项: 只能在 IOI2C_MST_IsBusy() == 0 时发送
******************************************************************************************************************************************/
void IOI2C_MST_Send(char slvaddr, char buff[], uint32_t size)
{	
	uint32_t i;
	
	TxBuff[0] = (slvaddr << 1) | 0;
	for(i = 1; i <= size; i++)
		TxBuff[i] = buff[i-1];
	TxSize = i;
	TxIndx = 0;
	
	BitTim = 0;
	
	IsRead = 0;
	
	IsBusy = 1;
	
	IOI2C_MST_Func = IOI2C_MST_Start;
	TIMR_Start(IOI2C_TMR);
}

/****************************************************************************************************************************************** 
* 函数名称:	IOI2C_MST_Read()
* 功能说明: IOI2C主机读取数据
* 输    入: char slvaddr	从机地址
*			char buff[]		读取到的数据存入此数组
*			uint32_t size		要读取的数据的个数，最多128个
* 输    出: 无
* 注意事项: 只能在 IOI2C_MST_IsBusy() == 0 时读取，当再次 IOI2C_MST_IsBusy() == 0 时读取到的数据已经存储在buff[]中
******************************************************************************************************************************************/
void IOI2C_MST_Read(char slvaddr, char buff[], uint32_t size)
{	
	uint32_t i;
	
	TxBuff[0] = (slvaddr << 1) | 1;
	for(i = 1; i <= size; i++)
		TxBuff[i] = buff[i-1];
	TxSize = i;
	TxIndx = 0;
	
	RxBuff = buff;
	for(i = 0; i < size; i++)
		RxBuff[i] = 0;
	RxSize = i;
	RxIndx = 0;
	
	BitTim = 0;
	
	IsRead = 1;
	
	IsBusy = 1;
	
	IOI2C_MST_Func = IOI2C_MST_Start;
	TIMR_Start(IOI2C_TMR);
}

/****************************************************************************************************************************************** 
* 函数名称:	IOI2C_MST_IsBusy()
* 功能说明: IOI2C主机否正在发送/接收
* 输    入: 无
* 输    出: uint32_t		1 IOI2C正忙于发送/接收    0 IOI2C当前空闲，可以发起新的发送
* 注意事项: 无
******************************************************************************************************************************************/
uint32_t IOI2C_MST_IsBusy(void)
{
	return IsBusy;
}

static void IOI2C_MST_Start(void)
{
	I2C_SDA_LOW();
	
	IOI2C_MST_Func = IOI2C_MST_SendByte;
}

static void IOI2C_MST_SendByte(void)
{
    if(BitTim < 16)
    {
        if((BitTim % 2) == 0)
        {
            I2C_SCL_LOW();
			
			if(BitTim == 0) I2C_SDA_OUTPUT();	//发送上一个字符时会将SDA切成输入脚以接收ACK
			
			if(TxBuff[TxIndx] & (1 << (7 - BitTim/2)))
				I2C_SDA_HIGH();
			else
				I2C_SDA_LOW();
        }
        else
        {
            I2C_SCL_HIGH();
        }
    }
    else if(BitTim == 16)
    {
		I2C_SCL_LOW();
		
		I2C_SDA_INPUT();
	}
	else if(BitTim == 17)
	{
		I2C_SCL_HIGH();
    }
    else
    {
        if(I2C_SDA_Value())	//NACK
        {
            IOI2C_MST_Func = IOI2C_MST_Stop;
        }
        else if((TxIndx == 0) && IsRead)
        {
            IOI2C_MST_Func = IOI2C_MST_ReadByte;
        }
        else
        {
            if(++TxIndx == TxSize)
            {
                IOI2C_MST_Func = IOI2C_MST_Stop;
            }
        }
		
		BitTim = 0;
		goto noinc;
    }
	
	BitTim++;

noinc:
	;
}

static void IOI2C_MST_ReadByte(void)
{
    if(BitTim < 16)
    {
        if((BitTim % 2) == 0)
        {
			I2C_SCL_LOW();
			
			if(BitTim == 0) I2C_SDA_INPUT();	//接收上一个字符时会将SDA切成输出脚以发送ACK
        }
        else
        {
			I2C_SCL_HIGH();
			
			RxBuff[RxIndx] |=  I2C_SDA_Value() << (7 - BitTim/2);				
        }
    }
    else if(BitTim == 16)
	{
		I2C_SCL_LOW();
		
		I2C_SDA_OUTPUT();
		
		if(++RxIndx < RxSize)
		{
			I2C_SDA_LOW();
		}
		else
		{
			I2C_SDA_HIGH();
		}
	}
	else if(BitTim == 17)
	{
		I2C_SCL_HIGH();
	}
	else
	{
		if(RxIndx == RxSize)
		{
			IOI2C_MST_Func = IOI2C_MST_Stop;
		}
		
		BitTim = 0;
		goto noinc;
	}
	
	BitTim++;

noinc:
	;
}

static void IOI2C_MST_Stop(void)
{
    if(BitTim == 0)
    {
		I2C_SCL_LOW();
		
		I2C_SDA_LOW();
		I2C_SDA_OUTPUT();	//发送上一个字符时会将SDA切成输入脚以接收ACK
    }
    else
    {
		I2C_SCL_HIGH();
		
		I2C_SDA_HIGH();
		
		IsBusy = 0;
		
		TIMR_Stop(IOI2C_TMR);
    }
	
	BitTim++;
}

/****************************************************************************************************************************************** 
* 函数名称:	IOI2C_ISR()
* 功能说明: TIMER中断服务程序，以2倍SCL时钟频率执行
* 输    入: 无
* 输    出: 无
* 注意事项: 无
******************************************************************************************************************************************/
void IOI2C_ISR(void)
{	
	TIMR_INTClr(IOI2C_TMR, TIMR_IT_TO);
	
	if(IsBusy) IOI2C_MST_Func();
}
