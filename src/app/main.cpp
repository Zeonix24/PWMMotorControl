#include "pico/cyw43_arch.h"
#include <cstdio>

#include "driver/current_sensor.hpp"
#include "driver/led_driver.hpp"
#include "driver/motor_driver.hpp"
#include "driver/reset_button.hpp"
#include "lowpass_filter.tpp"

LowPass<2> lp(3.0f, 1000.0f, true);

int main()
{
    stdio_init_all();
    if (cyw43_arch_init())
    {
        printf("WiFi chip failed to initialize");
        return -1;
    }
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, true);
    bool trigger = false;
    setup_leds();
    setup_reset_button();
    setup_current_sensor();
    // cppcheck-suppress unreadVariable
    float zero_voltage = calibrate_current_sensor();
    setup_pwm_motor();

    set_motor_duty_cycle(duty_cycles[motor_state_index]);
    // sleep_ms(5000);
    watchdog_enable(3000, 1);
    while (1)
    {
        check_for_reset();

        float current =
            lp.filt(read_current(zero_voltage));  // the returned current is
                                                  // filtered by low-pass filter

        if (current > THRESHOLD_CURRENT)
        {
            trigger = true;
        }

        if (current < THRESHOLD_CURRENT && trigger == true)
        {
            motor_state_index = 0;
        } else
        {
            motor_state_index = 1;
        }
        set_motor_duty_cycle(duty_cycles[motor_state_index]);

        printf("Current index: %d, Duty cycle: %d%%, Current(A): %.6f\n",
               motor_state_index, duty_cycles[motor_state_index], current);

        sleep_us(200);
        watchdog_update();
    }
}