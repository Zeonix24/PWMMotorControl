#include <cstdio>
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "pico/cyw43_arch.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"

constexpr uint8_t MOTOR_PIN = 21;
constexpr uint8_t BUTTON_PIN = 16;
constexpr uint16_t FREQ_HZ = 25000;

// LED Pin definitions
constexpr uint8_t LED_RED = 8;
constexpr uint8_t LED_YELLOW = 10;
constexpr uint8_t LED_GREEN = 13;

// Global variables
uint32_t g_wrap_value;
volatile uint8_t g_current_index = 0;
volatile uint64_t g_last_interrupt_time = 0;

enum DUTY_CYCLE
{
    DC0 = 0,
    DC70 = 70,
    DC80 = 80,
    DC90 = 90,
    DC100 = 100,
};

const DUTY_CYCLE duty_cycles[] = {DC0, DC100, DC90, DC80, DC70};
size_t num_elements = sizeof(duty_cycles) / sizeof(duty_cycles[0]);

void setup_leds() {
    // Initialize LED pins
    gpio_init(LED_RED);
    gpio_init(LED_YELLOW); 
    gpio_init(LED_GREEN);
    
    // Set as outputs
    gpio_set_dir(LED_RED, GPIO_OUT);
    gpio_set_dir(LED_YELLOW, GPIO_OUT);
    gpio_set_dir(LED_GREEN, GPIO_OUT);
    
    // Initialize all LEDs to off
    gpio_put(LED_RED, false);
    gpio_put(LED_YELLOW, false);
    gpio_put(LED_GREEN, false);
}

void update_leds(uint8_t duty_cycle) {
    // Turn off all LEDs first
    gpio_put(LED_RED, false);
    gpio_put(LED_YELLOW, false);
    gpio_put(LED_GREEN, false);
    
    // Update LEDs based on duty cycle
    if (duty_cycle >= 70) gpio_put(LED_RED, true);
    if (duty_cycle >= 80) gpio_put(LED_YELLOW, true);
    if (duty_cycle >= 90) {gpio_put(LED_GREEN, true); gpio_put(LED_YELLOW, false);}
    if (duty_cycle >= 100) gpio_put(LED_YELLOW, true);
}

void setup_pwm_motor() 
{
    gpio_set_function(MOTOR_PIN, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(MOTOR_PIN);
    pwm_config config = pwm_get_default_config();
    
    constexpr uint32_t clock = 125000000;
    constexpr uint32_t divider = 125;
    g_wrap_value = clock / (FREQ_HZ * divider);
    
    pwm_config_set_clkdiv(&config, divider);
    pwm_config_set_wrap(&config, g_wrap_value);
    pwm_init(slice_num, &config, true);
}

void set_motor_duty_cycle(uint8_t duty_cycle) {
    if (duty_cycle > 100) duty_cycle = 100;
    uint16_t level = (g_wrap_value * duty_cycle) / 100;
    pwm_set_gpio_level(MOTOR_PIN, level);
    // Update LEDs whenever duty cycle changes
    update_leds(duty_cycle);
    printf("Motor duty cycle set to: %u%%\n", duty_cycle);
}

void gpio_callback(uint gpio, uint32_t events) {
    uint64_t current_time = time_us_64();
    
    if (current_time - g_last_interrupt_time < 250000) {
        printf("Debounce ignored at %llu us\n", current_time);
        return;
    }
    
    g_last_interrupt_time = current_time;
    
    if (gpio_get(BUTTON_PIN) == 0) {
        g_current_index = (g_current_index + 1) % num_elements;
        printf("Button pressed! Index: %d\n", g_current_index);
        set_motor_duty_cycle(duty_cycles[g_current_index]);
    }
}

void setup_button() {
    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_PIN);
    
    sleep_ms(100);
    
    gpio_set_irq_enabled_with_callback(BUTTON_PIN, 
                                     GPIO_IRQ_EDGE_FALL,
                                     true, 
                                     &gpio_callback);
                                     
    printf("Button setup complete on GPIO %d\n", BUTTON_PIN);
}

int main()
{
    stdio_init_all();
    if (cyw43_arch_init())
    {
        printf("WiFi chip failed to initialize");
        return -1;
    }
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, true);

    // Setup all peripherals
    setup_leds();
    setup_pwm_motor();
    setup_button();
    
    // Set initial duty cycle
    set_motor_duty_cycle(duty_cycles[g_current_index]);
    printf("System initialized. Press button to cycle through duty cycles.\n");

    while (true)
    {       
        printf("Current index: %d, Duty cycle: %u%%\n", 
               g_current_index, 
               duty_cycles[g_current_index]);
        sleep_ms(2000);
    }
}