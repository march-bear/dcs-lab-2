#include "led.h"
#include "gpio.h"

void light_up(led_color led) {
	switch (led) {
	case RED:
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_SET);
		break;
	case ORANGE:
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET);
		break;
	case GREEN:
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_SET);
		break;
	}
}

void light_down(led_color led) {
	switch (led) {
	case RED:
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_RESET);
		break;
	case ORANGE:
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_RESET);
		break;
	case GREEN:
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_RESET);
		break;
	}
}

void light_blink(led_color led) {
	light_up(led);
	HAL_Delay(BLINK_TIME);
	light_down(led);
}

