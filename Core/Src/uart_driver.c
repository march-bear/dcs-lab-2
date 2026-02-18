#include "uart_driver.h"
#include "stm32f4xx_hal_uart.h"

bool uart6_transmit_ongoing;
bool uart6_receive_finished;
uint8_t uart6_buf;
bool interrupt = true;

void uart6_switch_interrupt_mode() {
	if (interrupt) {
		HAL_UART_Abort(&huart6);
		uart6_transmit_ongoing = 0;
		uart6_receive_finished = 1;
	}

	interrupt = !interrupt;
}

HAL_StatusTypeDef receive_char_into_buffer(uart_buf* buffer) {
	if (interrupt) {
		return uart6_try_push_received_char_it(buffer);
	} else {
		return uart6_try_push_received_char(buffer);
	}
}

HAL_StatusTypeDef uart6_try_push_received_char(uart_buf* buffer) {
	uint8_t data;
	HAL_StatusTypeDef res = HAL_UART_Receive(&huart6, &data, 1, 10);

	if (res == HAL_OK) {
		uart_buf_push(buffer, (char) data);
	}

	return res;
}

HAL_StatusTypeDef transmit_char_from_buffer(uart_buf* buffer) {
	if (interrupt) {
		return uart6_try_transmit_pushed_char_it(buffer);
	} else {
		return uart6_try_transmit_pushed_char(buffer);
	}
}

HAL_StatusTypeDef uart6_try_transmit_pushed_char(uart_buf* buffer) {
	char data;
	if (uart_buf_pop(buffer, &data)) {
		return HAL_UART_Transmit(&huart6, (uint8_t *) &data, 1, 10);
	}

	return HAL_ERROR;
}

HAL_StatusTypeDef uart6_start_receive_char_it() {
	uart6_receive_finished = 0;
	return HAL_UART_Receive_IT(&huart6, &uart6_buf, 1);
}

bool uart6_try_push_received_char_it(uart_buf* buffer) {
	if (uart6_receive_finished) {
		uart_buf_push(buffer, (char) uart6_buf);
		uart6_start_receive_char_it();
		return true;
	}

	return false;
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
	if (huart->Instance == USART6) {
		uart6_receive_finished = 1;
	}
}

HAL_StatusTypeDef uart6_try_transmit_pushed_char_it(uart_buf* buffer) {
	char data;
	if (uart_buf_pop(buffer, &data) && uart6_transmit_ongoing == 0) {
		uart6_transmit_ongoing = 1;
		return HAL_UART_Transmit_IT(&huart6, (uint8_t *) &data, 1);
	}

	return HAL_ERROR;
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
	if (huart->Instance == USART6) {
		uart6_transmit_ongoing = 0;
	}
}
