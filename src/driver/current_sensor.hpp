#pragma once

#include "hardware/gpio.h"
#include "hardware/adc.h"
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"

#include "common/constants.hpp"
#include "common/types.hpp"
#include "motor_driver.hpp"

#include <cstdio>

void setup_current_sensor();
float calibrate_current_sensor();
float read_current(float zero_voltage);
