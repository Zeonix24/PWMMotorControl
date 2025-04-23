#include "types.hpp"

const DUTY_CYCLE duty_cycles[] = {DC0, DC90};
uint32_t g_wrap_value = 0;
volatile uint8_t motor_state_index = 0;