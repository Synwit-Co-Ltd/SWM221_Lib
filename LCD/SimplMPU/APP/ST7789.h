#ifndef __ST7789_H__
#define __ST7789_H__

void ST7789_Init(void);
void ST7789_Clear(uint16_t rgb);
void ST7789_DrawPoint(uint16_t x, uint16_t y, uint16_t rgb);


void ST7789_DMAClear(uint16_t color);
uint32_t ST7789_DMADone(void);

#endif //__ST7789_H__
