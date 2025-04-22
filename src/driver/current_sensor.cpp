#include "current_sensor.hpp"

void setup_current_sensor()
{
    printf("Did I get her?\n");
    adc_init();
    adc_gpio_init(ACS712_PIN);
    adc_select_input(ADC_INPUT);
}

float calibrate_current_sensor()
{
    uint16_t samples = 360;
    uint64_t total = 0;
    for (uint16_t i = 0; i < samples; i++)
    {
        total += adc_read();
    }
    float avg_adc = static_cast<float>(total) / samples;
    return (avg_adc * CONVERSION_FACTOR);
}
void read_current(float zero_voltage)
{
    uint16_t samples = 360;
    uint64_t totalACS = 0;
    for (uint16_t i = 0; i < samples; i++)
    {
        totalACS += adc_read();
    }
    float avg_ACS = static_cast<float>(totalACS) / samples;

    float current_I = ((zero_voltage - (avg_ACS * CONVERSION_FACTOR)) / 0.185);
    if (current_I < THRESHOLD_CURRENT)
    {
        motor_state_index = 0;
    } else
    {
        motor_state_index = 1;
    }
    set_motor_duty_cycle(duty_cycles[motor_state_index]);
    printf("Current: %.6f\n", current_I);
}