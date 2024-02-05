#include "SWM221.h"

#include "IOI2C_SLV.h"

static char TxBuff[128] = {0};
static uint32_t TxSize = 0;
static volatile uint32_t TxIndx = 0;

static char RxBuff[128] = {0};
static uint32_t RxSize = 128;			//最多能接收128个字节
static volatile uint32_t RxIndx = 0;

static volatile uint32_t BitTim = 0;

static volatile uint32_t IsBusy = 0;	//检测到Start置位，检测到Stop清零

static volatile uint32_t IsNACK = 0;	//发送数据时收到NACK


static uint32_t RxBuff2[128] = {0};		//接收到Stop时将RxBuff中数据搬运到这里，然后驱动可以继续用RxBuff接收数据位，用户可通过从RxBuff2中读走数据
static volatile uint32_t RxSize2 = 0;	//RxBuff2中有多少个数据可以被读出
static volatile uint32_t RxIndx2 = 0;

static void (*IOI2C_SLV_Func)(void);

static void IOI2C_SLV_SendByte(void);
static void IOI2C_SLV_ReadByte(void);

extern uint32_t IOI2C_SLV_EmptyCallback(char *TxBuff);

/****************************************************************************************************************************************** 
* 函数名称:	IOI2C_SLV_Init()
* 功能说明: IOI2C_SLV初始化
* 输    入: 无
* 输    出: 无
* 注意事项: 无
******************************************************************************************************************************************/
void IOI2C_SLV_Init(void)
{
#define IOI2C_PORT	GPIOA
#define IOI2C_SCL	PIN2
#define IOI2C_SDA	PIN3
#define IOI2C_IRQ	GPIOA_IRQn
#define IOI2C_ISR	GPIOA_Handler
	
	GPIO_Init(IOI2C_PORT, IOI2C_SCL, 0, 1, 0, 1);
#define I2C_SCL_Value()		GPIO_GetBit(IOI2C_PORT, IOI2C_SCL)

	GPIO_Init(IOI2C_PORT, IOI2C_SDA, 0, 1, 0, 1);
#define I2C_SDA_OUTPUT()	IOI2C_PORT->DIR |= (1 << IOI2C_SDA);
#define I2C_SDA_LOW()		GPIO_ClrBit(IOI2C_PORT, IOI2C_SDA)
#define I2C_SDA_HIGH()		GPIO_SetBit(IOI2C_PORT, IOI2C_SDA)

#define I2C_SDA_INPUT()		IOI2C_PORT->DIR &=~(1 << IOI2C_SDA)
#define I2C_SDA_Value()		GPIO_GetBit(IOI2C_PORT, IOI2C_SDA)	
	
	
	EXTI_Init(IOI2C_PORT, IOI2C_SCL, EXTI_BOTH_EDGE);
	EXTI_Init(IOI2C_PORT, IOI2C_SDA, EXTI_BOTH_EDGE);
	
	NVIC_EnableIRQ(IOI2C_IRQ);
	
	EXTI_Open(IOI2C_PORT, IOI2C_SCL);
	EXTI_Open(IOI2C_PORT, IOI2C_SDA);
}

/****************************************************************************************************************************************** 
* 函数名称:	IOI2C_SLV_RxNotEmpty()
* 功能说明: IOI2C从机接收缓冲区中是否有数据可取出
* 输    入: 无
* 输    出: uint32_t		0 没有可取出数据    >0 可取出的数据个数
* 注意事项: 无
******************************************************************************************************************************************/
uint32_t IOI2C_SLV_RxNotEmpty(void)
{
	return RxSize2;
}

/****************************************************************************************************************************************** 
* 函数名称:	IOI2C_SLV_FetchByte()
* 功能说明: 从IOI2C从机接收缓冲区中取出一个字节
* 输    入: 无
* 输    出: char		取出的字节
* 注意事项: 无
******************************************************************************************************************************************/
char IOI2C_SLV_FetchByte(void)
{
	RxSize2 -= 1;
	
	return RxBuff2[RxIndx2++];
}

/****************************************************************************************************************************************** 
* 函数名称:	IOI2C_SLV_FetchBytes()
* 功能说明: 从IOI2C从机接收缓冲区中取出n个字节
* 输    入: char buff[]		取出的数据存入此数组
*			uint32_t len		要取出的数据的个数，不得大于IOI2C_SLV_RxNotEmpty()返回值
* 输    出: 无
* 注意事项: 无
******************************************************************************************************************************************/
void IOI2C_SLV_FetchBytes(char buff[], uint32_t len)
{
	uint32_t i;
	
	for(i = 0; i < len; i++)
		buff[i] = RxBuff2[RxIndx2+i];
	RxIndx2 += len;
	RxSize2 -= len;
}


static void IOI2C_SLV_SendByte(void)
{
	if(IsNACK == 1)
	{
		I2C_SDA_INPUT();
		
		goto noinc;
	}
	
	if(TxSize == 0)
	{
		TxSize = IOI2C_SLV_EmptyCallback(TxBuff);
		
		TxIndx = 0;
	}
	
	if(BitTim == 0)	I2C_SDA_OUTPUT();	//发送上一个字符时会将SDA切成输入脚以接收ACK
	
	if(BitTim < 16)
	{
		if((BitTim % 2) == 0)
        {
			if(TxBuff[TxIndx] & (1 << (7 - BitTim/2)))
				I2C_SDA_HIGH();
			else
				I2C_SDA_LOW();
        }
	}
	else if(BitTim == 16)
	{
		I2C_SDA_INPUT();
	}
	else if(BitTim == 17)
	{
		if(I2C_SDA_Value())	//NACK
        {
            IsNACK = 1;
        }
        else
        {
            if(++TxIndx == TxSize)
            {
                TxIndx = TxSize - 1;
            }
        }
		
		BitTim = 0;
		goto noinc;
    }
	
	BitTim++;

noinc:
	;
}

static void IOI2C_SLV_ReadByte(void)
{
	if(BitTim == 0) I2C_SDA_INPUT();	//接收上一个字符时会将SDA切成输出脚以发送ACK
	
	if(BitTim < 16)
    {
        if((BitTim % 2) == 1)
        {
			if(I2C_SDA_Value() == 1)
				RxBuff[RxIndx] |=  (1 << (7 - BitTim/2));
			else
				RxBuff[RxIndx] &= ~(1 << (7 - BitTim/2));
        }
    }
    else if(BitTim == 16)
	{
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
		if((RxIndx == 1) && (RxBuff[0] & 0x01))
		{
			TxIndx = 0;
			
			IsNACK = 0;
			
			IOI2C_SLV_Func = IOI2C_SLV_SendByte;
		}
		
		BitTim = 0;
		goto noinc;
	}
	
	BitTim++;
	
noinc:
	;
}


/****************************************************************************************************************************************** 
* 函数名称:	IOI2C_ISR()
* 功能说明: IOI2C SCL、SDA引脚双边沿中断服务函数
* 输    入: 无
* 输    出: 无
* 注意事项: 无
******************************************************************************************************************************************/
void IOI2C_ISR(void)
{		
	uint32_t i;
	
	if(EXTI_State(IOI2C_PORT, IOI2C_SDA))
	{
		EXTI_Clear(IOI2C_PORT, IOI2C_SDA);
		
		if(I2C_SCL_Value() == 1)			//只有在SCL高电平时SDA的跳变才是Start和Stop
		{
			if(I2C_SDA_Value() == 0)		//Start
			{
				RxIndx = 0;
				
				BitTim = 0;
				
				IsBusy = 1;
				
				IOI2C_SLV_Func = IOI2C_SLV_ReadByte;
			}
			else							//Stop
			{
				IsBusy = 0;
				
				for(i = 0; i < RxIndx-1; i++)
				{
					RxBuff2[i] = RxBuff[1+i];	//RxBuff[]中第一个字节是从机地址
				}
				RxSize2 = i;
				RxIndx2 = 0;
			}
		}
	}
	else if(EXTI_State(IOI2C_PORT, IOI2C_SCL))
	{
		EXTI_Clear(IOI2C_PORT, IOI2C_SCL);
		
		if(IsBusy) IOI2C_SLV_Func();
	}
}
