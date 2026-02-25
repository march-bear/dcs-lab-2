#include "uart_buf.h"
#include "stm32f4xx_hal.h"

static uint32_t lock() {
	uint32_t pmask = __get_PRIMASK();
	__disable_irq();

	return pmask;
}

static void unlock(uint32_t pmask) {
	__set_PRIMASK(pmask);
}

void uart_buf_push(uart_buf* buf, char data) {
	uint32_t pmask = lock();

	buf->buffer[buf->write_idx] = data;
	buf->write_idx = (buf->write_idx + 1) % UART_BUFFER_SIZE;

	if (buf->size < UART_BUFFER_SIZE) {
		buf->size++;
	} else {
		buf->read_idx = (buf->read_idx + 1) % UART_BUFFER_SIZE;
	}

	unlock(pmask);
}

bool uart_buf_pop(uart_buf* buf, char* data) {
	uint32_t pmask = lock();

	if (buf->size == 0) {
		unlock(pmask);
		return false;
	}

	*data = buf->buffer[buf->read_idx];
	buf->read_idx = (buf->read_idx + 1) % UART_BUFFER_SIZE;

	buf->size--;

	unlock(pmask);
	return true;
}

bool uart_buf_empty(uart_buf* buf) {
	return buf->size == 0;
}

void uart_buf_reset(uart_buf* buf) {
	buf->write_idx = 0;
	buf->read_idx = 0;
	buf->size = 0;
}
