#ifndef __NT35510_H__
#define __NT35510_H__

void NT35510_Init(void);
void NT35510_Clear(uint16_t rgb);
void NT35510_DrawPoint(uint16_t x, uint16_t y, uint16_t rgb);


void NT35510_DMAClear(uint16_t color);
uint32_t NT35510_Done(void);

#endif //__NT35510_H__
