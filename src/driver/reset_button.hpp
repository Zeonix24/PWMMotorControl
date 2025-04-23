#pragma once

#include "common/constants.hpp"
#include "hardware/watchdog.h"
#include "hardware/gpio.h"
#include "pico/stdlib.h"

void setup_reset_button();
void check_for_reset();
