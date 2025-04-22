#pragma once

#include <cstdint>

enum DUTY_CYCLE
{
    DC0 = 0,
    DC90 = 90
};

extern const DUTY_CYCLE duty_cycles[];

extern uint32_t g_wrap_value;
extern volatile uint8_t motor_state_index;