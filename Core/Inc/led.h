#pragma once

#define BLINK_TIME 5

typedef enum {
	GREEN,
	RED,
	ORANGE
} led_color;

void light_up(led_color led);
void light_down(led_color led);
void light_blink(led_color led);
