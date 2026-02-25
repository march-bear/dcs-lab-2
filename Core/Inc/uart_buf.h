#pragma once

#include <stdint.h>
#include <stdbool.h>

#define UART_BUFFER_SIZE 8

typedef struct {
    char buffer[UART_BUFFER_SIZE];
    uint16_t write_idx;
    uint16_t read_idx;
    uint16_t size;
} uart_buf;

void uart_buf_push(uart_buf* buf, char data);
bool uart_buf_pop(uart_buf* buf, char* data);
void uart_buf_reset(uart_buf* buf);
bool uart_buf_empty(uart_buf* buf);
