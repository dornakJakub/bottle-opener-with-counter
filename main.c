#include "pico/stdlib.h"
#include "hardware/gpio.h"

#define GPIO_ON 1
#define GPIO_OFF 0

// Pin mapping of segments
#define SEG_A 0
#define SEG_B 1
#define SEG_C 2
#define SEG_D 3
#define SEG_E 4
#define SEG_F 5
#define SEG_G 6

// Pin mapping of digits
#define DIGIT_1 7
#define DIGIT_2 8
#define DIGIT_3 9
#define DIGIT_4 10

// Pin mapping of sensor
#define SENSOR 14

// Delay between reading sensor values
#define DELAY_MS 1000

// 7-segment display numbers representation for common anode display
uint8_t numbers[10] = {
    0b11000000, // 0
    0b11111001, // 1
    0b10100100, // 2
    0b10110000, // 3
    0b10011001, // 4
    0b10010010, // 5
    0b10000010, // 6
    0b11111000, // 7
    0b10000000, // 8
    0b10010000  // 9
};

// Displays number num on digit digit
void display_digit(uint8_t digit, uint8_t num) {
    gpio_put(DIGIT_1, digit == 0);
    gpio_put(DIGIT_2, digit == 1);
    gpio_put(DIGIT_3, digit == 2);
    gpio_put(DIGIT_4, digit == 3);

    for (int i = 0; i < 7; i++) {
        gpio_put(i, (numbers[num] >> i) & 1);
    }
}

// Converts an integer to an array of digits which can be displayed
void int_to_digits(int num, uint8_t digits[4]) {
    for (int i = 0; i < 4; i++) {
        digits[3 - i] = num % 10;
        num /= 10;
    }
}

int main() {
    // Initialize sensor pin as input
    gpio_init(SENSOR);
    gpio_set_dir(SENSOR, GPIO_IN);

    // Initialize display pins as outputs
    for (int i = 0; i < 11; i++) {
        gpio_init(i);
        gpio_set_dir(i, GPIO_OUT);
    }

    // Initialize variables
    int count = 0;
    uint32_t last_press_time = 0;
    uint8_t digits[4];
    bool sensor_value, is_disabled = false;
    int_to_digits(count, digits);

    while (true) {
        // Display the current number
        for (int i = 0; i < 4; i++) {
            display_digit(i, digits[i]);
            sleep_ms(2);
        }

        // Read the sensor value
        sensor_value = gpio_get(SENSOR);

        // Check if the sensor is pressed and the switch is enabled
        if (sensor_value && !is_disabled) {
            // Increment the count    
            int_to_digits(++count, digits);

            // Save the time of the last press
            last_press_time = to_ms_since_boot(get_absolute_time());

            // Disable the switch
            is_disabled = true;
        }

        // Check if the switch is disabled and if the delay has passed
        if (is_disabled && (to_ms_since_boot(get_absolute_time()) - last_press_time >= DELAY_MS)) {
            // Re-enable the switch
            is_disabled = false;
        }
    }
}