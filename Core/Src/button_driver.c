#include "gpio.h"

#include "button_driver.h"

uint32_t last_deb_time = 0;
uint32_t last_move_time = 0;

GPIO_PinState button_state = GPIO_PIN_SET; // inverted
GPIO_PinState last_reading = GPIO_PIN_SET; // inverted

GPIO_TypeDef* GPIOx = GPIOC;
uint16_t GPIO_Pin = GPIO_PIN_15;

bool is_pressed() {
	return !button_state;
}

bool is_long_press() {
	return is_pressed() && HAL_GetTick() - last_move_time > LONG_PRESS_DELAY;
}

bool is_long_unpress() {
	return !is_pressed() && HAL_GetTick() - last_move_time > LONG_UNPRESS_DELAY;
}

int update_button_state() {
	GPIO_PinState reading = HAL_GPIO_ReadPin(GPIOx, GPIO_Pin);

	if (reading != last_reading) {
		last_deb_time = HAL_GetTick();
		last_reading = reading;
	}

	if ((HAL_GetTick() - last_deb_time) > DEBOUNCE_DELAY) {
		if (reading != button_state) {
			button_state = reading;
			last_move_time = HAL_GetTick();

			if (is_pressed()) {
				return 1; // front
			} else {
				return -1; // edge
			}
		}
	}

	return 0; // nothing
}
