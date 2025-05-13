#include "current_sensor.hpp"
#include "non-volatile_storage.hpp"

void setup_current_sensor()
{
    adc_init();
    adc_gpio_init(ACS712_PIN);
}

float calibrate_current_sensor()
{
    printf("Calibrating...\n");
    adc_select_input(ADC2_INPUT);
    uint16_t samples = 500;
    uint64_t total = 0;
    for (uint16_t i = 0; i < samples; i++)
    {
        total += adc_read();
    }
    float avg_adc = static_cast<float>(total) / samples;
    float zero_voltage_offset = (avg_adc * CONVERSION_FACTOR);
    printf("zero_voltage_offset: %f\n", zero_voltage_offset);

    // Save the calibration value to non-volatile memory and check success
    bool save_success =
        NonVolatileStorage::save_zero_voltage(zero_voltage_offset);
    if (!save_success)
    {
        printf("Warning: Failed to save calibration to non-volatile memory\n");
    } else
    {
        printf("Calibration value saved successfully\n");
    }

    return zero_voltage_offset;
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

    float current_I = (((avg_ACS * CONVERSION_FACTOR) - zero_voltage) / SENS);

    return (current_I);
}
