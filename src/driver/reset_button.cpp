#include "reset_button.hpp"

void setup_reset_button()
{
    gpio_init(RESET_BUTTON_PIN);
    gpio_set_dir(RESET_BUTTON_PIN, GPIO_IN);
    gpio_pull_up(RESET_BUTTON_PIN);
}

void check_for_reset()
{
    sleep_ms(50);
    if (!gpio_get(RESET_BUTTON_PIN))
    {
        watchdog_reboot(0, 0, 0);
    }
}