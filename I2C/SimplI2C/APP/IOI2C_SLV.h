#ifndef __IOI2C_SLV_H__
#define __IOI2C_SLV_H__

#include <stdint.h>

void IOI2C_SLV_Init(void);

uint32_t IOI2C_SLV_RxNotEmpty(void);
char IOI2C_SLV_FetchByte(void);
void IOI2C_SLV_FetchBytes(char buff[], uint32_t len);

#endif //__IOI2C_SLV_H__
