#include "pico/cyw43_arch.h"
#include <cstdio>

#include "driver/current_sensor.hpp"
#include "driver/led_driver.hpp"
#include "driver/motor_driver.hpp"
#include "driver/reset_button.hpp"
#include "driver/non-volatile_storage.hpp"
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
    printf("Starting...\n");

    setup_leds();
    setup_calibration_button();
    setup_current_sensor();
    setup_pwm_motor();
    sleep_ms(200);

    check_calibration_mode();
    check_for_reset();
    float zero_voltage;
    if (NonVolatileStorage::is_calibrated())
    {
        zero_voltage = NonVolatileStorage::get_zero_voltage();
        printf("Using stored calibration: %f\n", zero_voltage);
    } else
    {
        // Perform initial calibration
        printf("No calibration found, performing initial calibration...\n");
        zero_voltage = calibrate_current_sensor();
        printf("Initial calibration complete: %f\n", zero_voltage);
    }

    set_motor_duty_cycle(duty_cycles[motor_state_index]);

    sleep_ms(5000);
    watchdog_enable(3000, 1);
    while (1)
    {
        // Check for calibration button press during runtime
        if (is_calibration_button_pressed())
        {
            printf("Calibration button pressed during operation\n");

            // Wait to see if this is a long-press for reset
            check_for_reset();

            // If we get here, it was a normal press - do calibration
            zero_voltage = calibrate_current_sensor();
            sleep_ms(1000);  // Debounce
        }

        float current =
            lp.filt(read_current(zero_voltage));  // the returned current is
                                                  // filtered by low-pass filter

        if (current < THRESHOLD_CURRENT)
        {
            motor_state_index = 0;
        } else
        {
            motor_state_index = 1;
        }
        set_motor_duty_cycle(duty_cycles[motor_state_index]);

        printf(
            "Current index: %d, Duty cycle: %d%%, Current(A): %.6f, Offset: "
            "%f\n",
            motor_state_index, duty_cycles[motor_state_index], current,
            zero_voltage);

        sleep_us(200);
        watchdog_update();
    }
}