#ifndef PATCHER_H
#define PATCHER_H

typedef struct Effect tEffect;

void patcher_init();
void activate_effect(tEffect * e); // add effect to end of current chain
void deactivate_effect(); // remove selected effect from current chain

#endif /* PATCHER_H */