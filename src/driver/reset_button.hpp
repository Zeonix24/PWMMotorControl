#pragma once

#include "pico/cyw43_arch.h"
#include "hardware/watchdog.h"
#include "hardware/gpio.h"
#include "current_sensor.hpp"
#include "common/constants.hpp"

void setup_calibration_button();
bool is_calibration_button_pressed();
void check_calibration_mode();
void check_for_reset();
