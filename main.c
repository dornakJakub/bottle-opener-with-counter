#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include <stdio.h>

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

#define SENSOR 11

// 7-segment display numbers representation
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

void int_to_digits(int num, uint8_t digits[4]) {
    for (int i = 0; i < 4; i++) {
        digits[i] = num % 10;
        num /= 10;
    }
}

int main() {
    // Initialize display pins as outputs
    stdio_init_all();
    gpio_init(SENSOR);
    gpio_set_dir(SENSOR, GPIO_IN);

    for (int i = 0; i < 11; i++) {
        gpio_init(i);
        gpio_set_dir(i, GPIO_OUT);
    }

    int count = 0;
    uint8_t digits[4];
    int_to_digits(count, digits);

    while (true) {
        for (int i = 0; i < 4; i++) {
            display_digit(i, digits[i]);
            sleep_ms(2);
        }

        int sensor_value = gpio_get(SENSOR);

        printf("Sensor Value: %d\n", sensor_value);

        sleep_ms(200);
    }
}