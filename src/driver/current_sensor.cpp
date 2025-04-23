#include "current_sensor.hpp"

void setup_current_sensor()
{
    adc_init();
    adc_gpio_init(PICO3V3_PIN);
    adc_gpio_init(ACS712_PIN);
}

float calibrate_current_sensor()
{
    adc_select_input(ADC1_INPUT);
    uint16_t samples = 500;
    uint64_t total = 0;
    for (uint16_t i = 0; i < samples; i++)
    {
        total += adc_read();
    }
    float avg_adc = static_cast<float>(total) / samples;
    float avg_voltage = (avg_adc * CONVERSION_FACTOR) / 2.0f;
    printf("avg_voltage: %f\n", avg_voltage);
    return avg_voltage;
}
float read_current(float zero_voltage)
{
    adc_select_input(ADC2_INPUT);
    uint16_t samples = 500;
    uint64_t totalACS = 0;
    for (uint16_t i = 0; i < samples; i++)
    {
        totalACS += adc_read();
    }
    float avg_ACS = static_cast<float>(totalACS) / samples;

    float current_I = ((zero_voltage - (avg_ACS * CONVERSION_FACTOR)) / 0.185);

    return current_I;
}
