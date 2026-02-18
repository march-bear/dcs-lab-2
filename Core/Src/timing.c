#include "timing.h"
#include "stm32f4xx_hal.h"

uint32_t timers[TIMERS_COUNT];

void timer_reset(int n) {
	if (0 <= n && n < TIMERS_COUNT) {
		timers[n] = HAL_GetTick();
	}
}

bool is_timer_expired(int n, uint32_t duration) {
	if (0 <= n && n < TIMERS_COUNT) {
		return HAL_GetTick() - timers[n] > duration;
	}

	return false;
}
