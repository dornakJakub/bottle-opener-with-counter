#include <stdint.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "segment_display.h"

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

// Initialize display pins as outputs
void display_init() {
    for (int i = 0; i < 12; i++) {
        gpio_init(i);
        gpio_set_dir(i, GPIO_OUT);
    }
}

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

// Converts an integer to an array of digits which can be displayed using_display_digit
void int_to_digits(int num, uint8_t digits[4]) {
    for (int i = 0; i < 4; i++) {
        digits[3 - i] = num % 10;
        num /= 10;
    }
}