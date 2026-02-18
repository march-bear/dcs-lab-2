#pragma once

#include <stdbool.h>
#include "usart.h"
#include "uart_buf.h"

void uart6_switch_interrupt_mode();

HAL_StatusTypeDef receive_char_into_buffer(uart_buf* buffer);
HAL_StatusTypeDef transmit_char_from_buffer(uart_buf* buffer);

HAL_StatusTypeDef uart6_try_push_received_char(uart_buf* buffer);
HAL_StatusTypeDef uart6_try_transmit_pushed_char(uart_buf* buffer);

HAL_StatusTypeDef uart6_start_receive_char_it();
bool uart6_try_push_received_char_it(uart_buf* buffer);
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);

HAL_StatusTypeDef uart6_try_transmit_pushed_char_it(uart_buf* buffer);
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart);
