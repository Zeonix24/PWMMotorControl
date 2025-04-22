#include "pico/cyw43_arch.h"
#include <cstdio>

#include "driver/current_sensor.hpp"
#include "driver/led_driver.hpp"
#include "driver/motor_driver.hpp"
#include "driver/reset_button.hpp"

int main()
{
    stdio_init_all();
    if (cyw43_arch_init())
    {
        printf("WiFi chip failed to initialize");
        return -1;
    }
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, true);

    setup_leds();
    setup_current_sensor();
    float zero_voltage = calibrate_current_sensor();
    setup_pwm_motor();

    set_motor_duty_cycle(duty_cycles[motor_state_index]);
    sleep_ms(20);  // Give the system time to properly initialize everything
    watchdog_enable(3000, 1);
    while (1)
    {
        printf("Current index: %d, Duty cycle: %d%%\n", motor_state_index,
               duty_cycles[motor_state_index]);
        read_current(zero_voltage);

        watchdog_update();
        sleep_ms(500);
    }
}