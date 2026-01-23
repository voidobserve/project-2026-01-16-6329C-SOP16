#ifndef __LED_STRIP_VOICE_H__
#define __LED_STRIP_VOICE_H__

#include "includes.h"

u8 get_sound_triggered_by_motor(void);
void clear_sound_triggered_by_motor(void);
void set_sound_triggered_by_motor(void);
u8 get_sound_triggered_by_colorful_light(void);
void clear_sound_triggered_by_colorful_light(void);
void set_sound_triggered_by_colorful_light(void);

void sound_handle(void);

#endif
