#include "hardware/adc.h"
#include "hardware/gpio.h"
#include "hardware/pwm.h"
#include "pico/cyw43_arch.h"
#include "pico/stdlib.h"
#include <cstdio>

// // Global variables
constexpr uint8_t MOTOR_PIN = 14;
constexpr uint16_t FREQ_HZ = 25000;

constexpr uint8_t GP28 = 28;
constexpr uint8_t ADC2 = 2;
constexpr float THRESHOLD_CURRENT = 0.01f;
constexpr float conversion_factor = 3.3f / (1 << 12);

// LED Pin definitions
constexpr uint8_t LED_GREEN = 15;

uint32_t g_wrap_value;
volatile uint8_t motor_state_index = 0;

enum DUTY_CYCLE
{
    DC0 = 0,
    DC90 = 90,
};

const DUTY_CYCLE duty_cycles[] = {DC0, DC90};
size_t num_elements = sizeof(duty_cycles) / sizeof(duty_cycles[0]);

void setup_leds()
{
    // Initialize LED pins
    gpio_init(LED_GREEN);

    // Set as outputs
    gpio_set_dir(LED_GREEN, GPIO_OUT);

    // Initialize all LEDs to ON
    gpio_put(LED_GREEN, true);
}

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

void setup_current_sensor()
{
    adc_init();
    adc_gpio_init(GP28);
    adc_select_input(ADC2);
}

float calibrate_current_sensor()
{
    uint8_t samples = 180;
    uint32_t total = 0;
    for (uint8_t i = 0; i < samples; i++)
    {
        uint16_t raw_adc = adc_read();
        total += raw_adc;
        sleep_ms(3);
    }
    float avg_adc = static_cast<float>(total) / samples;
    return (avg_adc * conversion_factor);
}
void read_current(float zero_voltage)
{
    uint8_t samples = 180;
    uint32_t totalACS = 0;
    for (uint8_t i = 0; i < samples; i++)
    {
        uint16_t raw_adc = adc_read();
        totalACS += raw_adc;
        sleep_ms(3);
    }
    float avg_ACS = static_cast<float>(totalACS) / samples;

    float current_I = ((zero_voltage - (avg_ACS * conversion_factor)) / 0.185);
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

int main()
{
    stdio_init_all();
    if (cyw43_arch_init())
    {
        printf("WiFi chip failed to initialize");
        return -1;
    }
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, true);

    setup_current_sensor();
    float zero_voltage = calibrate_current_sensor();
    setup_pwm_motor();
    set_motor_duty_cycle(duty_cycles[motor_state_index]);
    setup_leds();
    sleep_ms(20);  // Give the system time to properly initialize everything
    while (1)
    {
        printf("Current index: %d, Duty cycle: %d%%\n", motor_state_index,
               duty_cycles[motor_state_index]);
        read_current(zero_voltage);
        sleep_ms(500);
    }
}