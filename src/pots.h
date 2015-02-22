#ifndef POTS_H
#define POTS_H

#define POTS_COUNT 5 /* 3 pots and 2 expression pedals */

void pots_init();
uint16_t pots_read(uint8_t ch);

#endif /* POTS_H */
