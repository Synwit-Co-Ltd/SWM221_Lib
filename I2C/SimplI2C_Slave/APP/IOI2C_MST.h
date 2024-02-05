#ifndef __IOI2C_MST_H__
#define __IOI2C_MST_H__

#include <stdint.h>

void IOI2C_MST_Init(uint32_t baud);

uint32_t IOI2C_MST_IsBusy(void);
void IOI2C_MST_Send(char slvaddr, char buff[], uint32_t size);
void IOI2C_MST_Read(char slvaddr, char buff[], uint32_t size);

#endif //__IOI2C_MST_H__
