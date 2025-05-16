#pragma once

#include <cstdint>

// GPIO PINS AND INPUT
constexpr uint8_t MOTOR_PIN = 14;
constexpr uint8_t LED_PIN = 15;
constexpr uint8_t RESET_BUTTON_PIN = 16;
constexpr uint8_t ACS712_PIN = 28;
constexpr uint8_t ADC2_INPUT = 2;

// CONSTANTS
constexpr float CONVERSION_FACTOR = 3.3f / (1 << 12);
constexpr float THRESHOLD_CURRENT = 0.07f;
constexpr uint16_t FREQ_HZ = 25000;
