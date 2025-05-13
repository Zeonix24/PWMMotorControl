#include "reset_button.hpp"
#include "non-volatile_storage.hpp"

// Debounce state tracking
struct
{
    bool is_pressed;
    absolute_time_t press_time;
    bool debouncing;
} button_state = {false, {0}, false};

// Setup the calibration button with internal pull-up
void setup_calibration_button()
{
    gpio_init(CALIBRATION_BUTTON_PIN);
    gpio_set_dir(CALIBRATION_BUTTON_PIN, GPIO_IN);
    gpio_pull_up(CALIBRATION_BUTTON_PIN);
}

bool is_calibration_button_pressed()
{
    // Read current button state (active low)
    bool current_state = gpio_get(CALIBRATION_BUTTON_PIN) == 0;
    absolute_time_t current_time = get_absolute_time();

    // First time button is pressed
    if (current_state && !button_state.is_pressed)
    {
        button_state.press_time = current_time;
        button_state.is_pressed = true;
        button_state.debouncing = true;
        return false;
    }

    // Debounce logic
    if (button_state.debouncing)
    {
        // Wait for debounce delay
        if (absolute_time_diff_us(button_state.press_time, current_time) >=
            (DEBOUNCE_DELAY_MS * 1000))
        {
            button_state.debouncing = false;

            // Recheck button state after debounce delay
            if (gpio_get(CALIBRATION_BUTTON_PIN) == 0)
            {
                return true;
            }
        }
        return false;
    }

    // Button released
    if (!current_state)
    {
        button_state.is_pressed = false;
        return false;
    }

    return false;
}

// Check for calibration mode and perform calibration if requested
void check_calibration_mode()
{
    absolute_time_t start_time = get_absolute_time();

    // Check if calibration button is pressed during startup
    while (is_calibration_button_pressed())
    {
        absolute_time_t current_time = get_absolute_time();

        // Check for long press
        if (absolute_time_diff_us(start_time, current_time) >=
            (LONG_PRESS_MS * 1000))
        {
            printf("Calibration mode activated!\n");

            // Perform calibration
            float zero_voltage = calibrate_current_sensor();

            // Provide visual feedback (blink LED)
            for (int i = 0; i < 3; i++)
            {
                cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, true);
                sleep_ms(200);
                cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, false);
                sleep_ms(200);
            }

            printf("Calibration complete. Zero voltage: %f\n", zero_voltage);

            // Wait to prevent multiple calibrations
            sleep_ms(1000);
            break;
        }

        // Small delay to prevent tight looping
        sleep_ms(10);
    }
}

// Add a function to handle hardware reset
void check_for_reset()
{
    // Check if calibration button is held for an extended period (5 seconds)
    absolute_time_t start_time = get_absolute_time();

    if (is_calibration_button_pressed())
    {
        while (is_calibration_button_pressed())
        {
            absolute_time_t current_time = get_absolute_time();

            // Check for very long press (5 seconds)
            if (absolute_time_diff_us(start_time, current_time) >=
                (5000 * 1000))
            {
                printf("Long press detected - performing hardware reset\n");

                // Flash LED rapidly as an indicator
                for (int i = 0; i < 10; i++)
                {
                    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, true);
                    sleep_ms(100);
                    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, false);
                    sleep_ms(100);
                }

                // Clear calibration data
                NonVolatileStorage::clear_calibration();

                // Reset the device
                watchdog_enable(1, 0);
                while (1);  // Wait for watchdog to trigger
            }

            // Small delay
            sleep_ms(100);
        }
    }
}