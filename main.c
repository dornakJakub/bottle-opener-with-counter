#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/flash.h"
#include "hardware/sync.h"
#include <stdio.h>

#define GPIO_ON 1
#define GPIO_OFF 0

// Pin mapping
#define DIGIT_1 7
#define DIGIT_2 8
#define DIGIT_3 9
#define DIGIT_4 10
#define SENSOR 14
#define RESET 15

// Define constant values
#define SENSOR_DELAY_MS 1000
#define FLASH_TARGET_OFFSET (256 * 1024)
#define MAGIC_NUMBER 0x12345678

typedef struct {
    uint32_t magic;
    int value;
} flash_data_t;


// Function to read the count from flash memory
int read_int_from_flash(int *out_value) {
    const flash_data_t *flash_data = (const flash_data_t *)(XIP_BASE + FLASH_TARGET_OFFSET);

    // Check if the magic number matches
    if (flash_data->magic == MAGIC_NUMBER) {
        *out_value = flash_data->value;
        return 1;  // Data exists and is valid
    }

    return 0;  // Data does not exist
}

// Function to write the count to flash memory
void write_int_to_flash(int value) {
    uint32_t ints = save_and_disable_interrupts();

    flash_data_t data_to_store;
    data_to_store.magic = MAGIC_NUMBER;
    data_to_store.value = value;

    uint8_t buffer[FLASH_PAGE_SIZE] = {0};
    memcpy(buffer, &data_to_store, sizeof(flash_data_t));

    flash_range_erase(FLASH_TARGET_OFFSET, FLASH_SECTOR_SIZE);

    flash_range_program(FLASH_TARGET_OFFSET, buffer, FLASH_PAGE_SIZE);

    restore_interrupts(ints);
}


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

void pin_init() {
    // Initialize sensor pin as input
    gpio_init(SENSOR);
    gpio_set_dir(SENSOR, GPIO_IN);

    // Initialize display pins as outputs
    for (int i = 0; i < 11; i++) {
        gpio_init(i);
        gpio_set_dir(i, GPIO_OUT);
    }

    // Initialize reset button
    gpio_init(RESET);
    gpio_set_dir(RESET, GPIO_IN);
}

int main() {
    pin_init();

    // Initialize variables
    int count;
    if (read_int_from_flash(&count) == 0) {
        count = 0;
    }
    uint32_t sensor_press_time = 0, reset_press_time = 0;
    uint8_t digits[4];
    bool sensor_value, reset_pressed, sensor_is_disabled = false, reset_was_pressed = false;
    int_to_digits(count, digits);int_to_digits(++count, digits);

    while (true) {
        // Display the current number
        for (int i = 0; i < 4; i++) {
            display_digit(i, digits[i]);
            sleep_ms(2);
        }


        // Reset the counter if reset button is pressed for 5 seconds
        reset_pressed = gpio_get(RESET);

        if (reset_pressed && !reset_was_pressed) {
            reset_press_time = to_ms_since_boot(get_absolute_time());
            reset_was_pressed = true;
        }
        else if (reset_pressed && reset_was_pressed) {
            uint32_t current_time = to_ms_since_boot(get_absolute_time());
            if (current_time - reset_press_time >= 5000) {
                count = 0;
                int_to_digits(count, digits);
                reset_was_pressed = false;
            }
        }

        else if (!reset_pressed) {
            reset_was_pressed = false;
        }

        // Read the sensor value
        sensor_value = gpio_get(SENSOR);

        // Check if the sensor is pressed and the switch is enabled
        if (sensor_value && !sensor_is_disabled) {
            // Increment the count    
            int_to_digits(++count, digits);
            write_int_to_flash((count));

            // Save the time of the last press
            sensor_press_time = to_ms_since_boot(get_absolute_time());

            // Disable the switch
            sensor_is_disabled = true;
        }

        // Check if the switch is disabled and if the delay has passed
        if (sensor_is_disabled && (to_ms_since_boot(get_absolute_time()) - sensor_press_time >= SENSOR_DELAY_MS)) {
            // Re-enable the switch
            sensor_is_disabled = false;
        }

        sleep_ms(10);
    }
}