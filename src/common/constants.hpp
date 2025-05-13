#pragma once

#include <cstdint>

// GPIO PINS AND INPUT
constexpr uint8_t MOTOR_PIN = 14;
constexpr uint8_t LED_PIN = 15;
constexpr uint8_t CALIBRATION_BUTTON_PIN = 16;
constexpr uint8_t ACS712_PIN = 28;
constexpr uint8_t ADC2_INPUT = 2;

// CONSTANTS
constexpr float CONVERSION_FACTOR = 3.3f / (1 << 12);
constexpr float THRESHOLD_CURRENT = 0.01f;
constexpr uint16_t FREQ_HZ = 25000;
constexpr uint8_t DEBOUNCE_DELAY_MS = 50;
constexpr uint16_t LONG_PRESS_MS = 2000;
constexpr float SENS = 0.100f;  // ACS712 Sensitvity:
                                // 185mV/A @ 5A
                                // 100mV/A @ 20A
                                // 65mV/A @ 30A
