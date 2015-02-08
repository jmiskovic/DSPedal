#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "bitmaps.h"

#define   LCD_PIXEL_HEIGHT    48u
#define   LCD_PIXEL_WIDTH     84u

void graphics_clear(void);
void graphics_init(void);
void graphics_refresh(void);

void graphics_draw_rect(uint16_t left, uint16_t top, uint16_t width, uint16_t height);
void graphics_erase_rect(uint16_t left, uint16_t top, uint16_t width, uint16_t height);

typedef uint8_t (*graphics_op) (uint8_t first, uint8_t second);
uint8_t and_op(uint8_t first, uint8_t second);
uint8_t or_op(uint8_t first, uint8_t second);
uint8_t xor_op(uint8_t first, uint8_t second);
void graphics_draw(bitmap const sprite, graphics_op op, uint16_t x, uint16_t y);
void graphics_write(const char * str, bitmap const font, graphics_op op, uint16_t x, uint16_t y);

#endif /* GRAPHICS_H */