#ifndef SEGMENT_DISPLAY_H
#define SEGMENT_DISPLAY_H

#include <stdint.h>

// Display digit pins
#define DIGIT_1 7
#define DIGIT_2 8
#define DIGIT_3 9
#define DIGIT_4 10

// Displays number onto chosen digin
void display_digit(uint8_t digit, uint8_t num);

// Initialize display pins
void display_init();

void int_to_digits(int num, uint8_t digits[4]);

#endif