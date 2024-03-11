#ifndef __SWM221_IOFILT_H__
#define __SWM221_IOFILT_H__


/* ѡ����ĸ��źŽ����˲� */
#define IOFILT0_PB14	1
#define IOFILT0_PB4		2
#define IOFILT0_PB5		4
#define IOFILT0_PB6		8

#define IOFILT1_ACMP0	1	// �� ACMP0_OUT �˲���ACMP0 ��״̬��SYS->ACMPSR.CMP0OUT�����жϣ�SYS->ACMPSR.CMP0IF������Ϊ PWM ɲ���źž����˲�
#define IOFILT1_ACMP1	2


#define IOFILT_WIDTH_2		1
#define IOFILT_WIDTH_4		2
#define IOFILT_WIDTH_8		3
#define IOFILT_WIDTH_16		4
#define IOFILT_WIDTH_32		5
#define IOFILT_WIDTH_64		6
#define IOFILT_WIDTH_128	7
#define IOFILT_WIDTH_256	8
#define IOFILT_WIDTH_512	9
#define IOFILT_WIDTH_1024	10
#define IOFILT_WIDTH_2048	11
#define IOFILT_WIDTH_4096	12
#define IOFILT_WIDTH_8192	13
#define IOFILT_WIDTH_16384	14
#define IOFILT_WIDTH_32768	15



void IOFILT_Init(uint32_t IOFILTn, uint32_t signal, uint32_t width);
void IOFILT_Open(uint32_t IOFILTn);
void IOFILT_Close(uint32_t IOFILTn);


#endif // __SWM221_IOFILT_H__
