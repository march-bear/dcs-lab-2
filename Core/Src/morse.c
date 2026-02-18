#include <stdlib.h>
#include <stdbool.h>

#include "morse.h"

const MorseCode morse_alphabet[] = {
    {'A',   {MORSE_DOT,  MORSE_DASH},          					2},     // .-
    {'B',   {MORSE_DASH, MORSE_DOT,  MORSE_DOT, MORSE_DOT}, 	4}, 	// -...
    {'C',   {MORSE_DASH, MORSE_DOT,  MORSE_DASH, MORSE_DOT}, 	4}, 	// -.-.
    {'D',   {MORSE_DASH, MORSE_DOT,  MORSE_DOT},            	3}, 	// -..
    {'E',   {MORSE_DOT},                                    	1}, 	// .
    {'F',   {MORSE_DOT,  MORSE_DOT, MORSE_DASH, MORSE_DOT}, 	4}, 	// ..-.
    {'G',   {MORSE_DASH, MORSE_DASH, MORSE_DOT},            	3}, 	// --.
    {'H',   {MORSE_DOT,  MORSE_DOT,  MORSE_DOT, MORSE_DOT}, 	4}, 	// ....
    {'I',   {MORSE_DOT,  MORSE_DOT},                         	2}, 	// ..
    {'J',   {MORSE_DOT,  MORSE_DASH, MORSE_DASH, MORSE_DASH},	4}, 	// .---
    {'K',   {MORSE_DASH, MORSE_DOT,  MORSE_DASH},            	3}, 	// -.-
    {'L',   {MORSE_DOT,  MORSE_DASH, MORSE_DOT,  MORSE_DOT}, 	4}, 	// .-..
    {'M',   {MORSE_DASH, MORSE_DASH},                        	2}, 	// --
    {'N',   {MORSE_DASH, MORSE_DOT},                         	2}, 	// -.
    {'O',   {MORSE_DASH, MORSE_DASH, MORSE_DASH},            	3}, 	// ---
    {'P',   {MORSE_DOT,  MORSE_DASH, MORSE_DASH, MORSE_DOT}, 	4}, 	// .--.
    {'Q',   {MORSE_DASH, MORSE_DASH, MORSE_DOT,  MORSE_DASH},	4}, 	// --.-
    {'R',   {MORSE_DOT,  MORSE_DASH, MORSE_DOT},             	3}, 	// .-.
    {'S',   {MORSE_DOT,  MORSE_DOT,  MORSE_DOT},             	3}, 	// ...
    {'T',   {MORSE_DASH},                                    	1}, 	// -
    {'U',   {MORSE_DOT,  MORSE_DOT,  MORSE_DASH},            	3}, 	// ..-
    {'V',   {MORSE_DOT,  MORSE_DOT,  MORSE_DOT,  MORSE_DASH},	4}, 	// ...-
    {'W',   {MORSE_DOT,  MORSE_DASH, MORSE_DASH},            	3}, 	// .--
    {'X',   {MORSE_DASH, MORSE_DOT,  MORSE_DOT,  MORSE_DASH},	4}, 	// -..-
    {'Y',   {MORSE_DASH, MORSE_DOT,  MORSE_DASH, MORSE_DASH},	4}, 	// -.--
    {'Z',   {MORSE_DASH, MORSE_DASH, MORSE_DOT,  MORSE_DOT}, 	4}  	// --..
};

const int morse_alphabet_len = 26;

bool morse_get_sym(char letter, int i, morse* buf) {
	if (letter > 'Z' || letter < 'A') {
		return false;
	}
	MorseCode code = morse_alphabet[letter - 'A'];
	if (i >= code.length) {
		return false;
	}
    *buf = code.code[i];

	return true;
}
