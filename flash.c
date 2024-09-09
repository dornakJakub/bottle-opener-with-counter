#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "hardware/sync.h"
#include "hardware/flash.h"
#include "pico/stdlib.h"
#include "flash.h"

int read_int_from_flash(int *out_value) {
    const flash_data_t *flash_data = (const flash_data_t *)(XIP_BASE + FLASH_TARGET_OFFSET);

    // Check if the magic number matches
    if (flash_data->magic == MAGIC_NUMBER) {
        *out_value = flash_data->value;
        return 1;  // Data exists and is valid
    }

    return 0;  // Data does not exist
}

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