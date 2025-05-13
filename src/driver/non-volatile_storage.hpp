#pragma once

#include "pico/stdlib.h"
#include "hardware/flash.h"
#include "hardware/sync.h"
#include <cstdio>

// Add this section to ensure flash functions run from RAM
#ifndef __not_in_flash
#define __not_in_flash __attribute__((noinline, section(".time_critical")))
#endif

class NonVolatileStorage
{
  public:
    // Add attributes to ensure these functions are placed in RAM
    static bool __no_inline_not_in_flash_func(save_zero_voltage)(float zero_voltage);
    static float __no_inline_not_in_flash_func(get_zero_voltage)();
    static bool __no_inline_not_in_flash_func(is_calibrated)();
    static void __no_inline_not_in_flash_func(clear_calibration)();
};