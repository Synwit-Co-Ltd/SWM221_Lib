#ifndef __SWM221_DMA_H__
#define __SWM221_DMA_H__


typedef struct {
	uint8_t  Mode;				//DMA_MODE_SINGLE、DMA_MODE_CIRCLE
	
	uint8_t  Unit;				//DMA_UNIT_BYTE、DMA_UNIT_HALFWORD、DMA_UNIT_WORD
	
	uint32_t Count;				//传输 Unit 个数，最大取值0xFFFF
	
	uint32_t MemoryAddr;
	
	uint32_t PeripheralAddr;
	
	uint8_t  MemoryAddrInc;		//0 地址固定    1 地址递增
	
	uint8_t  PeripheralAddrInc;
	
	uint8_t  Handshake;			//传输握手信号：DMA_HS_NO、DMA_CH0_UART0TX、DMA_CH0_SPI0TX、... ...
	
	uint8_t  Priority;			//DMA_PRI_HIGHEST、DMA_PRI_VERY_HIGH、...、DMA_PRI_LOWEST
	
	uint32_t  INTEn;			//中断使能，有效值有 DMA_IT_DONE、DMA_IT_HALF、DMA_IT_ERROR 及其“或”
} DMA_InitStructure;


#define DMA_CH0		0
#define DMA_CH1		1

#define DMA_MODE_SINGLE			0		// 单次模式，传输完成后停止
#define DMA_MODE_CIRCLE			1		// 环形模式，传输完成后从头执行下一轮传输

#define DMA_UNIT_BYTE		0
#define DMA_UNIT_HALFWORD	1
#define DMA_UNIT_WORD		2

#define DMA_PRI_HIGHEST		0
#define DMA_PRI_VERY_HIGH	2
#define DMA_PRI_HIGH		4
#define DMA_PRI_LOW			11
#define DMA_PRI_VERY_LOW	13
#define DMA_PRI_LOWEST		15


#define DMA_HS_NO			(0 << 4)	// 无握手信号，适用于 Memory to Memory 传输，PeripheralAddr 用作目的地址
#define DMA_HS_MRD			(1 << 4)	// memory read  handshake，适用于 Memory to Peripheral 传输
#define DMA_HS_MWR			(2 << 4)	// memory write handshake，适用于 Peripheral to Memory 传输
#define DMA_HS_EXT			(4 << 4)	// external handshake
#define DMA_HS_MSK			(7 << 4)

// memory read handshake signal
#define DMA_CH0_UART0TX		(0 | DMA_HS_MRD)
#define DMA_CH0_SPI0TX		(1 | DMA_HS_MRD)
#define DMA_CH0_QSPI0TX		(2 | DMA_HS_MRD)
#define DMA_CH0_ADC0SEQ0	(3 | DMA_HS_MRD)

#define DMA_CH1_USART0TX	(0 | DMA_HS_MRD)
#define DMA_CH1_UART1TX		(1 | DMA_HS_MRD)
#define DMA_CH1_MPUTX		(2 | DMA_HS_MRD)
#define DMA_CH1_ADC1SEQ0	(3 | DMA_HS_MRD)

// memory write handshake signal
#define DMA_CH0_USART0RX	(0 | DMA_HS_MWR)
#define DMA_CH0_UART1RX		(1 | DMA_HS_MWR)
#define DMA_CH0_MPURX		(2 | DMA_HS_MWR)
#define DMA_CH0_ADC0SEQ1	(3 | DMA_HS_MWR)

#define DMA_CH1_UART0RX		(0 | DMA_HS_MWR)
#define DMA_CH1_SPI0RX		(1 | DMA_HS_MWR)
#define DMA_CH1_QSPI0RX		(2 | DMA_HS_MWR)
#define DMA_CH1_ADC1SEQ1	(3 | DMA_HS_MWR)


// 外部握手信号
#define DMA_EXMRD_TIMR0		(0 | DMA_HS_MRD | DMA_HS_EXT)
#define DMA_EXMRD_TIMR1		(1 | DMA_HS_MRD | DMA_HS_EXT)
#define DMA_EXMRD_TIMR2		(2 | DMA_HS_MRD | DMA_HS_EXT)
#define DMA_EXMRD_TIMR3		(3 | DMA_HS_MRD | DMA_HS_EXT)
#define DMA_EXMRD_TRIG0		(4 | DMA_HS_MRD | DMA_HS_EXT)	// DMA_TRIG0引脚
#define DMA_EXMRD_TRIG1		(5 | DMA_HS_MRD | DMA_HS_EXT)	// DMA_TRIG1引脚

#define DMA_EXMWR_TIMR0		(0 | DMA_HS_MWR | DMA_HS_EXT)
#define DMA_EXMWR_TIMR1		(1 | DMA_HS_MWR | DMA_HS_EXT)
#define DMA_EXMWR_TIMR2		(2 | DMA_HS_MWR | DMA_HS_EXT)
#define DMA_EXMWR_TIMR3		(3 | DMA_HS_MWR | DMA_HS_EXT)
#define DMA_EXMWR_TRIG0		(4 | DMA_HS_MWR | DMA_HS_EXT)
#define DMA_EXMWR_TRIG1		(5 | DMA_HS_MWR | DMA_HS_EXT)



/* Interrupt Type */
#define DMA_IT_DONE		(1 << 1)	//Transfer Done
#define DMA_IT_HALF		(1 << 2)	//Transfer Half
#define DMA_IT_ERROR	(1 << 3)	//Transfer Error



void DMA_CH_Init(uint32_t chn, DMA_InitStructure * initStruct);	//DMA通道配置
void DMA_CH_Open(uint32_t chn);
void DMA_CH_Close(uint32_t chn);


static inline void DMA_CH_SetCount(uint32_t chn, uint32_t count)
{
	DMA->CH[chn].NDT = ( count 		<< DMA_NDT_LEN_Pos) |
					   ((count / 2) << DMA_NDT_HALF_Pos);
}


static inline uint32_t DMA_CH_GetRemaining(uint32_t chn)
{
	return (DMA->CH[chn].NDT & DMA_NDT_LEN_Msk);
}


static inline void DMA_CH_SetAddrAndCount(uint32_t chn, uint32_t addr, uint32_t count)
{
	DMA->CH[chn].MAR = addr;
	
	DMA_CH_SetCount(chn, count);
}

void DMA_CH_INTEn(uint32_t chn, uint32_t it);					//DMA中断使能
void DMA_CH_INTDis(uint32_t chn, uint32_t it);					//DMA中断禁止
void DMA_CH_INTClr(uint32_t chn, uint32_t it);					//DMA中断标志清除
uint32_t DMA_CH_INTStat(uint32_t chn, uint32_t it);				//DMA中断状态查询


#endif //__SWM221_DMA_H__
