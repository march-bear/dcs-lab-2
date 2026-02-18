#pragma once

#include <stdbool.h>

#define LONG_UNPRESS_DELAY 2000
#define LONG_PRESS_DELAY 200
#define DEBOUNCE_DELAY 50

bool is_pressed();
bool is_long_press();
bool is_long_unpress();
int update_button_state();
