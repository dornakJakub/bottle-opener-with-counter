#include "pico/stdlib.h"
#include <stdbool.h>
#include <stdint.h>
#include "segment_display.h"
#include "flash.h"

#define GPIO_ON 1
#define GPIO_OFF 0

// Pin mapping
#define SENSOR 14
#define RESET 15

// Define constant values
#define SENSOR_DELAY_MS 1000
#define RESET_THRESHOLD 5000
#define DISPLAY_TIME 8000

void init() {
    display_init();

    gpio_init(SENSOR);
    gpio_set_dir(SENSOR, GPIO_IN);

    gpio_init(RESET);
    gpio_set_dir(RESET, GPIO_IN);
}

int main() {
    init();

    // Initialize variables
    int count;
    if (read_int_from_flash(&count) == 0) {
        count = 0;
    }
    uint32_t sensor_press_time = 0, reset_press_time = 0, current_time;
    uint8_t digits[4];
    bool sensor_pressed, reset_pressed, sensor_is_disabled = false, reset_was_pressed = false;
    int_to_digits(count, digits);

    while (true) {
        current_time = to_ms_since_boot(get_absolute_time());

        // Reset the counter if reset button is pressed for 5 seconds
        reset_pressed = gpio_get(RESET);

        if (reset_pressed) {
            if (!reset_was_pressed) {
                reset_press_time = to_ms_since_boot(get_absolute_time());
                reset_was_pressed = true;
            }
            else {
                if (current_time - reset_press_time >= RESET_THRESHOLD) {
                    count = 0;
                    int_to_digits(count, digits);
                    reset_was_pressed = false;
                }
            }
        }
        else {
            reset_was_pressed = false;
        }

        // Read the sensor value
        sensor_pressed = gpio_get(SENSOR);

        // Check if the sensor is pressed and the switch is enabled
        if (sensor_pressed) {
            sensor_press_time = to_ms_since_boot(get_absolute_time());
            if (!sensor_is_disabled) {
                // Increment the count
                int_to_digits(++count, digits);
                write_int_to_flash((count));

                // Disable the switch
                sensor_is_disabled = true;
            }
        }

        // Check if the switch is disabled and if the delay has passed
        if (sensor_is_disabled && (current_time - sensor_press_time >= SENSOR_DELAY_MS)) {
            // Re-enable the switch
            sensor_is_disabled = false;
        }

        // If sensor or reset detects signal, display count on display
        if (current_time - reset_press_time <= DISPLAY_TIME || current_time - sensor_press_time <= DISPLAY_TIME) {
            for (int i = 0; i < 4; i++) {
                display_digit(i, digits[i]);
                sleep_ms(2);
            }
        }
        else {
            gpio_put(DIGIT_1, GPIO_OFF);
            gpio_put(DIGIT_2, GPIO_OFF);
            gpio_put(DIGIT_3, GPIO_OFF);
            gpio_put(DIGIT_4, GPIO_OFF);
        }

        sleep_ms(10);
    }
}