#include "morse_buf.h"

bool morse_buf_write(morse_buf* buf, morse data) {
    if (buf->write_idx == MORSE_BUFFER_SIZE) {
    	return false;
    }

    buf->buffer[buf->write_idx] = data;
    buf->write_idx++;

    return true;
}

bool morse_buf_next(morse_buf* buf, morse* data) {
    if (buf->read_idx == buf->write_idx) {
    	return false;
    }

    *data = buf->buffer[buf->read_idx];
    buf->read_idx++;

    return true;
}

bool morse_buf_curr(morse_buf* buf, morse* data) {
    if (buf->read_idx == buf->write_idx) {
    	return false;
    }

    *data = buf->buffer[buf->read_idx];

    return true;
}

void morse_buf_reset(morse_buf* buf) {
	buf->write_idx = 0;
	buf->read_idx = 0;
}

bool morse_buf_empty(morse_buf* buf) {
	return buf->write_idx == 0;
}

int morse_buf_len(morse_buf* buf) {
	return buf->write_idx - buf->read_idx;
}

bool morse_buf_full(morse_buf* buf) {
	return buf->write_idx == MORSE_BUFFER_SIZE;
}

bool morse_buf_is_eq(morse_buf* buf, const morse* other, uint8_t len) {
	if (morse_buf_len(buf) == len) {
		for (int j = 0; j < len; ++j) {
			if (other[j] != buf->buffer[j]) {
				return false;
			}
		}

		return true;
	}

	return false;
}

char decode_char(morse_buf * buf) {
	for (int i = 0; i < morse_alphabet_len; ++i) {
		if (morse_buf_is_eq(buf, morse_alphabet[i].code, morse_alphabet[i].length)) {
			return morse_alphabet[i].letter;
		}
	}

	return '\0';
}

