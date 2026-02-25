#include <string.h>

#include "uart_driver.h"
#include "stm32f4xx_hal_uart.h"

uart_buf buffer_rx;
uart_buf buffer_tx;

bool uart6_transmit_ongoing = false;
bool uart6_receive_finished = true;
uint8_t uart6_buf = 0;
bool interrupt = true;
bool uart6_buf_read = true;

char msg_int[] = "\r\nMODE: INTERRUPT\r\n";
char msg_bl[] = "\r\nMODE: BLOCKING\r\n";

bool get_interrupt() {
	return interrupt;
}

HAL_StatusTypeDef uart6_try_transmit_pushed_char_it() {
	static char data;
	if (uart6_transmit_ongoing == false && uart_buf_pop(&buffer_tx, &data)) {
		uart6_transmit_ongoing = true;
		return HAL_UART_Transmit_IT(&huart6, (uint8_t *) &data, 1);
	}

	return HAL_ERROR;
}

HAL_StatusTypeDef uart6_start_receive_char_it() {
	if (uart6_buf_read && uart6_receive_finished) {
		uart6_receive_finished = false;
		uart6_buf_read = false;
		return HAL_UART_Receive_IT(&huart6, &uart6_buf, 1);
	}
	return HAL_ERROR;
}

bool uart6_try_push_received_char_it() {
	if (!uart6_buf_read && uart6_receive_finished && uart6_buf != '\0') {
		uart_buf_push(&buffer_rx, (char) uart6_buf);
		if (uart6_buf != '+') {
			uart_buf_push(&buffer_tx, (char) uart6_buf);
		}

		uart6_buf_read = true;
		return true;
	}

	return false;
}

HAL_StatusTypeDef start_receive_char() {
	if (interrupt) {
		return uart6_start_receive_char_it();
	}

	return HAL_OK;
}

void uart6_switch_interrupt_mode() {
	if (interrupt) {
		HAL_UART_AbortReceive(&huart6);
		uart6_receive_finished = true;
		while (!uart_buf_empty(&buffer_tx) || uart6_transmit_ongoing) {
			uart6_try_transmit_pushed_char_it();
		}
	}

	interrupt = !interrupt;

	if (interrupt) {
		uart6_transmit_ongoing = true;
		HAL_UART_Transmit_IT(&huart6, (uint8_t *) msg_int, strlen(msg_int));
	} else {
		HAL_UART_Transmit(&huart6, (uint8_t *) msg_bl, strlen(msg_bl), 1000);
	}
}

HAL_StatusTypeDef uart6_try_push_received_char() {
	uint8_t data;
	HAL_StatusTypeDef res = HAL_UART_Receive(&huart6, &data, 1, 10);

	if (res == HAL_OK && data != '\0') {
		uart_buf_push(&buffer_rx, (char) data);
		if (data != '+') {
			uart_buf_push(&buffer_tx, (char) data);
		}
	}

	return res;
}

HAL_StatusTypeDef receive_char_into_buffer() {
	if (interrupt) {
		return uart6_try_push_received_char_it();
	} else {
		return uart6_try_push_received_char();
	}
}


HAL_StatusTypeDef uart6_try_transmit_pushed_char() {
	char data;
	if (uart_buf_pop(&buffer_tx, &data)) {
		return HAL_UART_Transmit(&huart6, (uint8_t *) &data, 1, 10);
	}

	return HAL_ERROR;
}

void transmit_char_from_buffer() {
	while (!uart_buf_empty(&buffer_tx)) {
		if (interrupt) {
			uart6_try_transmit_pushed_char_it();
		} else {
			uart6_try_transmit_pushed_char();
		}
	}
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
	if (huart->Instance == USART6) {
		uart6_receive_finished = 1;
	}
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
	if (huart->Instance == USART6) {
		uart6_transmit_ongoing = 0;
	}
}
