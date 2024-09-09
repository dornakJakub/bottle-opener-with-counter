#ifndef FLASH_H
#define FLASH_H

#include <stdint.h>

#define FLASH_TARGET_OFFSET (256 * 1024)
#define MAGIC_NUMBER 0x12345678

// Reads flash_data_t on FLASH_TARGET_OFFSET and if the magic number is matching stores it's int value in out_value variable
int read_int_from_flash(int *out_value);

// Creates flash_data_t value and writes it at FLASH_TARGET_OFFSET
void write_int_to_flash(int value);

typedef struct {
    uint32_t magic;
    int value;
} flash_data_t;

#endif