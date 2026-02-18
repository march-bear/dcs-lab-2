#include "mybuf.h"

void my_buf_init(my_buf* buf) {
    buf->write_idx = 0;
    buf->read_idx = 0;
}

bool my_buf_write(my_buf* buf, morse data) {
    if (buf->write_idx == MY_BUFFER_SIZE) {
    	return false;
    }

    buf->buffer[buf->write_idx] = data;
    buf->write_idx++;

    return true;
}

bool my_buf_next(my_buf* buf, morse* data) {
    if (buf->read_idx == buf->write_idx) {
    	return false;
    }

    *data = buf->buffer[buf->read_idx];
    buf->read_idx++;

    return true;
}

bool my_buf_curr(my_buf* buf, morse* data) {
    if (buf->read_idx == buf->write_idx) {
    	return false;
    }

    *data = buf->buffer[buf->read_idx];

    return true;
}

void my_buf_reset(my_buf* buf) {
	buf->write_idx = 0;
	buf->read_idx = 0;
}

bool my_buf_empty(my_buf* buf) {
	return buf->write_idx == 0;
}

bool my_buf_full(my_buf* buf) {
	return buf->write_idx == MY_BUFFER_SIZE;
}

int my_buf_len(my_buf* buf) {
	return buf->write_idx - buf->read_idx;
}

bool my_buf_is_eq(my_buf* buf, const morse* other, uint8_t len) {
	if (my_buf_len(buf) == len) {
		for (int j = 0; j < len; ++j) {
			if (other[j] != buf->buffer[j]) {
				return false;
			}
		}

		return true;
	}

	return false;
}

char decode_char(my_buf * buf) {
	for (int i = 0; i < morse_alphabet_len; ++i) {
		if (my_buf_is_eq(buf, morse_alphabet[i].code, morse_alphabet[i].length)) {
			return morse_alphabet[i].letter;
		}
	}

	return '\0';
}
