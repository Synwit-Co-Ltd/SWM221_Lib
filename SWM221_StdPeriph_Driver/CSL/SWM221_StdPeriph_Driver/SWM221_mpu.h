#ifndef __SWM221_MPU_H__
#define __SWM221_MPU_H__

typedef struct {
	uint8_t  RDHoldTime;	//LCD_RD低电平保持时间,取值1--32
	uint8_t  WRHoldTime;	//LCD_WR低电平保持时间,取值1--16
	uint8_t  CSFall_WRFall;	//LCD_CS下降沿到LCD_WR下降沿延时，取值1--4
	uint8_t  WRRise_CSRise;	//LCD_WR上升沿到LCD_CS上升沿延时，取值1--4
	uint8_t  RDCSRise_Fall;	//读操作时，LCD_CS上升沿到下降沿延时，取值1--32
	uint8_t  WRCSRise_Fall;	//写操作时，LCD_CS上升沿到下降沿延时，取值1--16
} MPU_InitStructure;


void MPU_Init(MPU_TypeDef * MPUx, MPU_InitStructure * initStruct);

void MPU_WR_REG(MPU_TypeDef * MPUx, uint8_t reg);
void MPU_WR_DATA(MPU_TypeDef * MPUx, uint8_t val);
void MPU_WriteReg(MPU_TypeDef * MPUx, uint8_t reg, uint8_t val);
uint8_t MPU_ReadReg(MPU_TypeDef * MPUx, uint8_t reg);


#endif // __SWM221_MPU_H__
