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

// 7-segment display numbers representation
uint8_t numbers[10] = {
    0b00111111, // 0
    0b00000110, // 1
    0b01011011, // 2
    0b01001111, // 3
    0b01100110, // 4
    0b01101101, // 5
    0b01111101, // 6
    0b00000111, // 7
    0b01111111, // 8
    0b01101111  // 9
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

int main() {
    // Initialize display pins as outputs
    for (int i = 0; i < 11; i++) {
        gpio_init(i);
        gpio_set_dir(i, GPIO_OUT);
    }

    while (true) {
        for (int i = 0; i < 10; i++) {
            for (int j = 0; j < 4; j++) {
                display_digit(j, i);
                sleep_ms(1000);
            }
        }
    }
}