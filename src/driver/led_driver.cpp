#include "led_driver.hpp"

void setup_leds()
{
    // Initialize LED pins
    gpio_init(LED_PIN);

    // Set as outputs
    gpio_set_dir(LED_PIN, GPIO_OUT);

    // Initialize all LEDs to ON
    gpio_put(LED_PIN, true);
}
