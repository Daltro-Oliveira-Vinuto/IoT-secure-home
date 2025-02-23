// libraries used in the project
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"

// Define GPIO pins for various components
const uint PIR_SENSOR_PIN = 8;  // GPIO pin for PIR motion sensor
const uint LED_PIN_RED = 13;      // GPIO pin for red LED
const uint LED_PIN_GREEN = 11;    // GPIO pin for green LED
const uint8_t LED_PIN_BLUE = 12;     // GPIO pin for blue LED
const uint FLAME_SENSOR_PIN = 18; // GPIO pin for flame sensor
const uint RIGHT_BUTTON = 6;  // GPIO pin for right button
const uint LEFT_BUTTON = 5;   // GPIO pin for left button
const uint LEFT_BUZZER_PIN = 10; // GPIO pin for the left buzzer

// Define constants for readability
#define ON 1
#define OFF 0

// Flag to track whether the system is active
volatile bool bitdoglab_is_on = false;

// Structure to define buzzer parameters
typedef struct {
    float frequency;      // Frequency of the buzzer sound
    float duty_cycle;     // Duty cycle percentage (0-100)
    uint gpio;            // GPIO pin connected to the buzzer
    float clock_divisor;  // PWM clock divisor value
    float wrap;           // PWM wrap value
} Buzzer;

// Function to configure and activate the buzzer
void set_buzzer(Buzzer *buzzer) {
    float pwm_level;

    // Set the GPIO function to PWM output
    gpio_set_function(buzzer->gpio, GPIO_FUNC_PWM);  
    uint pwm_slice = pwm_gpio_to_slice_num(buzzer->gpio); // Get PWM slice number

    // Configure PWM settings
    buzzer->clock_divisor = 125.0;  
    buzzer->wrap = (125000000 / (buzzer->clock_divisor * buzzer->frequency)) - 1;  

    pwm_set_clkdiv(pwm_slice, buzzer->clock_divisor); // Set PWM clock divisor
    pwm_set_wrap(pwm_slice, buzzer->wrap);           // Set PWM wrap value
    pwm_set_enabled(pwm_slice, true);                // Enable PWM output

    // Set PWM duty cycle
    pwm_level = (buzzer->duty_cycle / 100.0) * buzzer->wrap;
    pwm_set_gpio_level(buzzer->gpio, pwm_level);
}

// Function to initialize GPIO pins
void start_gpios() {
    stdio_init_all(); // Initialize serial communication

    // Initialize PIR motion sensor
    gpio_init(PIR_SENSOR_PIN);
    gpio_set_dir(PIR_SENSOR_PIN, GPIO_IN);
    gpio_pull_down(PIR_SENSOR_PIN); // Ensure stable readings

    // Initialize flame sensor
    gpio_init(FLAME_SENSOR_PIN);
    gpio_set_dir(FLAME_SENSOR_PIN, GPIO_IN);
    gpio_pull_up(FLAME_SENSOR_PIN); // Enable pull-up for stable readings

    // Initialize LEDs as output
    gpio_init(LED_PIN_RED);
    gpio_set_dir(LED_PIN_RED, GPIO_OUT);
    gpio_init(LED_PIN_GREEN);
    gpio_set_dir(LED_PIN_GREEN, GPIO_OUT);
    gpio_init(LED_PIN_BLUE);
    gpio_set_dir(LED_PIN_BLUE, GPIO_OUT);

    // Initialize buttons with pull-ups
    gpio_init(RIGHT_BUTTON);
    gpio_init(LEFT_BUTTON);
    gpio_set_dir(RIGHT_BUTTON, GPIO_IN);
    gpio_pull_up(RIGHT_BUTTON);
    gpio_pull_up(LEFT_BUTTON);
}

// Main program loop
int main() {
    stdio_init_all(); // Initialize standard I/O
    start_gpios();    // Initialize GPIOs

    printf("PIR Motion Sensor Test\n");

    // Initialize buzzer configuration
    Buzzer buzzer0;
    buzzer0.duty_cycle = 80.0; // initialize with a hight duty cycle of 80%
    buzzer0.gpio = LEFT_BUZZER_PIN; // initialize this buzzer with the pin 10

    while (ON) { // Infinite loop
        if (bitdoglab_is_on) { // Check if system is active
            // Check motion sensor
            if (gpio_get(PIR_SENSOR_PIN)) { // Motion detected
                printf("Motion Detected!\n");
                gpio_put(LED_PIN_RED, 1); // Turn red LED ON
                buzzer0.frequency = 2000;  // Set buzzer frequency
                set_buzzer(&buzzer0);
                sleep_ms(2000);
                buzzer0.frequency = 0; // Turn off buzzer
                set_buzzer(&buzzer0);
            } else {
                gpio_put(LED_PIN_RED, 0); // Turn red LED OFF
            }

            // Check flame sensor
            if (gpio_get(FLAME_SENSOR_PIN) == 0) { // Flame detected (low signal)
                printf("Flame Detected!\n");
                gpio_put(LED_PIN_GREEN, 1); // Turn green LED ON
                buzzer0.frequency = 10000; // Set buzzer frequency
                set_buzzer(&buzzer0);
                sleep_ms(2000);
                buzzer0.frequency = 0; // turn off buzzer
                set_buzzer(&buzzer0);
            } else {
                gpio_put(LED_PIN_GREEN, 0); // Turn green LED OFF
            }
            sleep_ms(500); // Small delay before next sensor check
        } else { // System is off
            gpio_put(LED_PIN_RED, OFF);
            gpio_put(LED_PIN_GREEN, OFF);
            buzzer0.frequency = 0; // Turn off buzzer
            set_buzzer(&buzzer0);
        }

        // Check right button press to turn system ON
        if (!gpio_get(RIGHT_BUTTON)) {
            bitdoglab_is_on = true;
            gpio_put(LED_PIN_BLUE, ON); // Turn blue LED ON
            sleep_ms(100); // Debounce delay
            sleep_ms(1000); // time in which the blue led is turn on
            gpio_put(LED_PIN_BLUE, OFF); // Turn off LED BLUE
        }
        
        // Check left button press to turn system OFF
        if (!gpio_get(LEFT_BUTTON)) {
            bitdoglab_is_on = false;
            gpio_put(LED_PIN_BLUE, OFF); // Turn off blue LED 
            gpio_put(LED_PIN_GREEN, OFF); // Turn off green LED
            gpio_put(LED_PIN_RED, OFF); // Turn off red LED
            sleep_ms(100); // Debounce delay
            buzzer0.frequency = 0; // Turn off buzzer
            set_buzzer(&buzzer0);
        }
    }

    return 0; // This line will never be reached in an infinite loop
}
