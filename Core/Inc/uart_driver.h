#pragma once

#include <stdbool.h>
#include "usart.h"
#include "uart_buf.h"

extern uart_buf buffer_rx;
extern uart_buf buffer_tx;

HAL_StatusTypeDef start_receive_char();
bool get_interrupt();
void uart6_switch_interrupt_mode();

HAL_StatusTypeDef receive_char_into_buffer();
void transmit_char_from_buffer();
