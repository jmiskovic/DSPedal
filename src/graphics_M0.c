#include "stdint.h"
#include "string.h"
#include "graphics.h"
#include "PCD8544.h"


uint8_t graphics_canvas[LCD_PIXEL_HEIGHT * LCD_PIXEL_WIDTH / 8] = {0};

void graphics_init(void) {
    graphics_clear();
    glcd_PCD8544_init();
}

void graphics_clear(void) {
    memset(graphics_canvas, 0x0, LCD_PIXEL_HEIGHT * LCD_PIXEL_WIDTH / 8);
}

void graphics_refresh(void) {
    glcd_write(graphics_canvas);
}

void graphics_erase_rect(uint16_t left, uint16_t top, uint16_t width, uint16_t height) {
    // y is in pixels, x is in bytes
    uint8_t mask, byte_offset;
    uint8_t * draw_location_pointer;
    uint16_t x, y;

    byte_offset = top % 8;
    for (y = top / 8; y <= (top + height) / 8; y++) {
        for (x = left; x < left + width; x++) {
            draw_location_pointer = graphics_canvas + x + y * LCD_PIXEL_WIDTH;
            if (draw_location_pointer >= graphics_canvas + LCD_PIXEL_HEIGHT * LCD_PIXEL_WIDTH / 8)
                continue;  // out of canvas range
            mask = 0;
            if (y == (top + height) / 8)
            { // last byte, preserve right part of byte
                mask |= (1 << (8 - ((byte_offset + height) % 8))) - 1;
            }
            if (y == top / 8)
            { // first byte, preserve left part of byte
                mask |= ~((1u << (8 - byte_offset)) - 1u);
            }
            *draw_location_pointer &= mask;
        }
    }
}

void graphics_draw_rect(uint16_t left, uint16_t top, uint16_t width, uint16_t height) {
    uint8_t mask, byte_offset;
    uint8_t * draw_location_pointer;
    uint16_t x, y;  // y is in pixels, x is in bytes

    byte_offset = top % 8;
    for (y = top / 8; y <= (top + height) / 8; y++) {
        for (x = left; x < left + width; x++) {
            draw_location_pointer = graphics_canvas + x + y * LCD_PIXEL_WIDTH;
            if (draw_location_pointer >= graphics_canvas + LCD_PIXEL_HEIGHT * LCD_PIXEL_WIDTH / 8)
                continue; // out of canvas range
            mask = 0xff;
            if (y == (top + height) / 8)
            { // last byte, preserve right part of byte
                mask ^= (1 << (8 - ((byte_offset + height) % 8))) - 1;
            }
            if (y == top / 8)
            { // first byte, preserve left part of byte
                mask ^= ~((1u << (8 - byte_offset)) - 1u);
            }
            *draw_location_pointer |= mask;
        }
    }
}

uint8_t __attribute__((unused)) and_op(uint8_t first, uint8_t second) {
    return first & second;
}

uint8_t __attribute__((unused)) or_op(uint8_t first, uint8_t second) {
    return first | second;
}

uint8_t __attribute__((unused)) xor_op(uint8_t first, uint8_t second) {
    return first ^ second;
}

void graphics_draw(bitmap const sprite, graphics_op op, uint16_t x, uint16_t y)
{ // draws over existing graphics
    uint8_t i,j;
    uint16_t ygrid = y - (y % 8);
    uint8_t byte_height; // how many bytes does the bitmap span
    uint8_t * draw_location_pointer;
    uint8_t stamp, mask;

    byte_height = sprite.height / 8 + (sprite.height % 8 > 0);

    for (i = 0; i < sprite.width; i++)
        for (j = 0; j < byte_height; j++)
        {
            if(ygrid >= LCD_PIXEL_HEIGHT || (x + i >= LCD_PIXEL_WIDTH))
                continue; // crosses screen border

            draw_location_pointer = graphics_canvas + x + i + (ygrid / 8 + j) * LCD_PIXEL_WIDTH;

            if (draw_location_pointer >= graphics_canvas + LCD_PIXEL_HEIGHT * LCD_PIXEL_WIDTH / 8)
                continue;

            if (ygrid != y)
            { // symbol placement spans over two bytes
                stamp = (*(sprite.data_pointer + i + sprite.width * j)) >> (y - ygrid);
                if (op == and_op) { // need to create mask to preserve non-bitmap pixels
                    mask = ~(0xff >> (y - ygrid));
                    stamp |= mask;
                }
                *draw_location_pointer = op(*draw_location_pointer, stamp);
                if(ygrid + 8 >= LCD_PIXEL_HEIGHT)
                    continue;   // crosses screen border
                stamp = (*(sprite.data_pointer  + i + sprite.width * j)) << (8 + ygrid - y);
                if (op == and_op) // need to create mask to preserve non-bitmap pixels
                {
                    if (j < byte_height - 1) {
                        mask = ~(0xff << (8 + ygrid - y));
                        stamp |= mask;
                    }
                    else {
                        mask = ~(0xff << (8 + ygrid - y + 8 - sprite.height % 8));
                        stamp |= mask;
                    }
                }
                (*(draw_location_pointer + LCD_PIXEL_WIDTH)) = op((*(draw_location_pointer + LCD_PIXEL_WIDTH)), stamp);
            }
            else if (draw_location_pointer < graphics_canvas + LCD_PIXEL_HEIGHT * LCD_PIXEL_WIDTH / 8)
            { // drawing position is byte-aligned
                stamp = *(sprite.data_pointer + i + sprite.width * j);
                if (op == and_op && j == byte_height - 1)
                    stamp |= (1 << (8 - (sprite.height % 8))) - 1;
                *draw_location_pointer = op(*draw_location_pointer, stamp);
            }
        }
}

/*  !"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\]^_`abcdefghijklmnopqrstuvwxyz{|}~ */
void graphics_write(const char * str, bitmap const font, graphics_op op, uint16_t x, uint16_t y) {
    bitmap character = {.width = font.width / 95, .height = font.height};
    while(*str) {
        if (*str < ' ' || *str > '~')
            character.data_pointer = font.data_pointer; /* out of range, insert space */
        else
            character.data_pointer = font.data_pointer + character.width * (uint16_t) (*str - ' ');
        graphics_draw(character, op, x, y);
        x += character.width;
        str++;
    }
}
