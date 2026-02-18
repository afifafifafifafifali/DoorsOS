#ifndef SOUND_H
#define SOUND_H

#include <stdint.h>

// Initialize simple SB16/PC speaker
void sound_init(void);

// Play a tone (Hz) for a blocking duration (ms)
void sound_play(uint16_t frequency, uint16_t duration_ms);
 void speaker_off(void);

#endif