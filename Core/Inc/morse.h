#pragma once

#include <stdint.h>

#define DOT_DURATION 200 // ms
#define DASH_DURATION 600 // ms
#define PAUSE_DURATION 200 // ms
#define LETTER_PAUSE_DURATION 600 // ms

typedef enum {
	MORSE_DASH,
	MORSE_DOT
} morse;

typedef struct {
    char letter;     	// Буква латиницы (A-Z)
    morse code[4]; 		// Код Морзе
    uint8_t length;  	// Длина кода
} MorseCode;

extern const int morse_alphabet_len;
extern const MorseCode morse_alphabet[];
bool morse_get_sym(char letter, int i, morse* buf);
