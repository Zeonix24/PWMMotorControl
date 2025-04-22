#pragma once

#include <cstdio>
#include "hardware/pwm.h"
#include "hardware/gpio.h"

#include "common/constants.hpp"
#include "common/types.hpp"

void setup_pwm_motor();
void set_motor_duty_cycle(uint8_t duty_cycle);
