#ifndef __IOSPI_H__
#define __IOSPI_H__

void IOSPI_Init(void);
void IOSPI_CS_Low(void);
void IOSPI_CS_High(void);
uint32_t IOSPI_ReadWrite(uint32_t data, uint32_t len);

#endif // __IOSPI_H__
