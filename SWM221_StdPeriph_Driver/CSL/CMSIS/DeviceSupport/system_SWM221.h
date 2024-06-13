#ifndef __SYSTEM_SWM221_H__
#define __SYSTEM_SWM221_H__

#ifdef __cplusplus
 extern "C" {
#endif


#define SYS_CLK_8MHz		3	 	//内部高频8MHz RC振荡器
#define SYS_CLK_XTAL		2		//外部晶体振荡器（4-24MHz）
#define SYS_CLK_PLL			1		//锁相环输出
#define SYS_CLK_32KHz		0		//内部低频32KHz RC振荡器


#define SYS_CLK_DIV_1		0
#define SYS_CLK_DIV_2		1
#define SYS_CLK_DIV_4		2
#define SYS_CLK_DIV_8		3


extern uint32_t SystemCoreClock;		// System Clock Frequency (Core Clock)
extern uint32_t CyclesPerUs;			// Cycles per micro second


extern void SystemInit(void);

extern void SystemCoreClockUpdate (void);


extern void switchToHRC(void);
extern void switchToDIV(uint32_t src, uint32_t div);

extern void switchOnHRC(void);
extern void switchOnXTAL(void);
extern void switchOnPLL(uint32_t src, uint32_t indiv, uint32_t fbdiv);
extern void switchOn32KHz(void);


#ifdef __cplusplus
}
#endif

#endif //__SYSTEM_SWM221_H__
