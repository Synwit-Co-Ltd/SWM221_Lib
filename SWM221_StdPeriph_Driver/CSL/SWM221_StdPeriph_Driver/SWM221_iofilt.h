#ifndef __SWM221_IOFILT_H__
#define __SWM221_IOFILT_H__


/* 选择对哪个信号进行滤波 */
#define IOFILT0_PB14	1
#define IOFILT0_PB4		2
#define IOFILT0_PB5		4
#define IOFILT0_PB6		8

#define IOFILT1_ACMP0	1	// 对 ACMP0_OUT 滤波，ACMP0 的状态（SYS->ACMPSR.CMP0OUT）、中断（SYS->ACMPSR.CMP0IF）、作为 PWM 刹车信号均被滤波
#define IOFILT1_ACMP1	2

#define IOFILT_WIDTH_250ns	1	// 滤波器时钟源为 HRC 且滤波时钟不分频时，每个滤波周期为 1/8MHz = 125ns
#define IOFILT_WIDTH_500ns	2
#define IOFILT_WIDTH_1us	3
#define IOFILT_WIDTH_2us	4
#define IOFILT_WIDTH_4us	5
#define IOFILT_WIDTH_8us	6
#define IOFILT_WIDTH_16us	7
#define IOFILT_WIDTH_32us	8
#define IOFILT_WIDTH_64us	9
#define IOFILT_WIDTH_128us	10
#define IOFILT_WIDTH_256us	11
#define IOFILT_WIDTH_512us	12
#define IOFILT_WIDTH_1024us	13
#define IOFILT_WIDTH_2048us	14
#define IOFILT_WIDTH_4096us	15



void IOFILT_Init(uint32_t IOFILTn, uint32_t signal, uint32_t width);


#endif // __SWM221_IOFILT_H__
