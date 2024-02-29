#ifndef __SYSTEM_SWM221_H__
#define __SYSTEM_SWM221_H__

#ifdef __cplusplus
 extern "C" {
#endif


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
