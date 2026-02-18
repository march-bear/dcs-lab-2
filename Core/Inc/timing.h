#include <stdint.h>
#include <stdbool.h>

#define TIMERS_COUNT 4

void timer_reset(int n);
bool is_timer_expired(int n, uint32_t duration);
