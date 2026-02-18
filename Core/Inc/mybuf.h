#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "morse.h"

#define MY_BUFFER_SIZE 8

typedef struct {
    morse buffer[MY_BUFFER_SIZE];
    uint16_t write_idx;
    uint16_t read_idx;
} my_buf;

void my_buf_init(my_buf* buf);
bool my_buf_write(my_buf* buf, morse data);
bool my_buf_next(my_buf* buf, morse* data);
void my_buf_reset(my_buf* buf);
bool my_buf_curr(my_buf* buf, morse* data);
bool my_buf_empty(my_buf* buf);
bool my_buf_full(my_buf* buf);
int my_buf_len(my_buf* buf);
bool my_buf_is_eq(my_buf* buf, const morse* other, uint8_t len);
char decode_char(my_buf * buf);
