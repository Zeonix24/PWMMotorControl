#include "motor_driver.hpp"

void setup_pwm_motor()
{
    gpio_set_function(MOTOR_PIN, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(MOTOR_PIN);
    pwm_config config = pwm_get_default_config();

    constexpr uint32_t clock = 125000000;  // 125 MHz
    constexpr uint32_t divider = 125;
    g_wrap_value = clock / (FREQ_HZ * divider);

    pwm_config_set_clkdiv(&config, divider);
    pwm_config_set_wrap(&config, g_wrap_value);
    pwm_init(slice_num, &config, true);
}

void set_motor_duty_cycle(uint8_t duty_cycle)
{
    uint16_t level = (g_wrap_value * duty_cycle) / 100;
    pwm_set_gpio_level(MOTOR_PIN, level);
    printf("Motor duty cycle set to: %u%%\n", duty_cycle);
}