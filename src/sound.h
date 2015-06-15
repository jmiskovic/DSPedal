#ifndef SOUND_H
#define SOUND_H

#define SAMPLE_RATE 48000

void sound_init(void);
void sound_blocking_passthrough(void);
void sound_blocking_process(void);

#endif /* SOUND_H */
