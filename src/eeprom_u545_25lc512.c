#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "picowalker-defs.h"
#include "peripherals.h"

#include "stm32u5xx_hal.h"
#include "stm32u5xx_hal_spi.h"

extern SPI_HandleTypeDef SPI_HANDLE;

enum {
    EEPROM_CMD_READ        = 0x03,
    EEPROM_CMD_WRITE       = 0x02,
    EEPROM_CMD_WREN        = 0x06,    // write enable
    EEPROM_CMD_WRDI        = 0x04,    // write disable
    EEPROM_CMD_RDSR        = 0x05,    // read status reg
    EEPROM_CMD_WRSR        = 0x01,    // write status reg
    EEPROM_CMD_PE          = 0x42,    // page erase
    EEPROM_CMD_SE          = 0xd8,    // sector erase
    EEPROM_CMD_CE          = 0xc7,    // chip erase
    EEPROM_CMD_RDID        = 0xab,    // release from deep power-down
    EEPROM_CMD_DPD         = 0xb9,    // send to deep power-down
};

#define EEPROM_PAGE_SIZE 128

#define EEPROM_STATUS_WIP_MASK      0x01 // Write in progress bit
#define EEPROM_STATUS_WIP_OFFSET    0x00

static uint8_t EEPROM_WRITE_BUFFER[EEPROM_PAGE_SIZE] = {0};

/**
 * Send a command to read a register
 *
 * @param cmd Command to send to eeprom
 * @param len Number of bytes to read
 * @param buf Buffer to read into
 */
static void eeprom_read_reg(uint8_t cmd, size_t len, uint8_t buf[len]) {
    HAL_GPIO_WritePin(EEPROM_CSB_PORT, EEPROM_CSB_PIN, GPIO_PIN_RESET);

    // Send command
    if( HAL_SPI_Transmit(&SPI_HANDLE, &cmd, 1, 5000) != HAL_OK ) {
        Error_Handler();
    }

    // Read data
    if( HAL_SPI_Receive(&SPI_HANDLE, buf, len, 5000) != HAL_OK ) {
        Error_Handler();
    }

    HAL_GPIO_WritePin(EEPROM_CSB_PORT, EEPROM_CSB_PIN, GPIO_PIN_SET);
}

/**
 * Send a command and data to the eeprom
 *
 * @param cmd Command to send to eeprom
 * @param len Number of bytes to send
 * @param buf Buffer of bytes to write to eeprom
 */
static void eeprom_write_cmd(uint8_t cmd, size_t len, uint8_t buf[len]) {
    HAL_GPIO_WritePin(EEPROM_CSB_PORT, EEPROM_CSB_PIN, GPIO_PIN_RESET);

    // Send command
    if( HAL_SPI_Transmit(&SPI_HANDLE, &cmd, 1, 500) != HAL_OK ) {
        Error_Handler();
    }

    // Send data
    if(len > 0 && buf != NULL) {
        if( HAL_SPI_Transmit(&SPI_HANDLE, &buf, len, 500) != HAL_OK ) {
            Error_Handler();
        }
    }

    HAL_GPIO_WritePin(EEPROM_CSB_PORT, EEPROM_CSB_PIN, GPIO_PIN_SET);
}

/**
 * Loop until the eeprom is ready for writes
 */
static void eeprom_wait_for_ready() {
    uint8_t buf[4] = {0};
    do {
        eeprom_read_reg(EEPROM_CMD_RDSR, 1, buf);
    } while(buf[0] & (EEPROM_STATUS_WIP_MASK << EEPROM_STATUS_WIP_OFFSET));
}

/*
 * ==========================================
 * Driver functions
 * ==========================================
 */

void pw_eeprom_init() {

    // Ensure CSB is high
    HAL_GPIO_WritePin(EEPROM_CSB_PORT, EEPROM_CSB_PIN, GPIO_PIN_SET);

    uint8_t buf[4] = {0};

    // Enable writing
    eeprom_write_cmd(EEPROM_CMD_WREN, 0, NULL);

    // Set status register, disable block protection and write protect enable
    buf[0] = 0x00;
    eeprom_write_cmd(EEPROM_CMD_WRSR, 1, buf);

}

int pw_eeprom_read(eeprom_addr_t addr, uint8_t *buf, size_t len) {
    eeprom_wait_for_ready();

    uint8_t params[4] = {0};

    // Disable writes
    eeprom_write_cmd(EEPROM_CMD_WRDI, 0, params);

    eeprom_wait_for_ready();

    // Send command and address, then read bytes in
    params[0] = EEPROM_CMD_READ;
    params[1] = addr>>8;
    params[2] = addr&0xff;

    HAL_GPIO_WritePin(EEPROM_CSB_PORT, EEPROM_CSB_PIN, GPIO_PIN_RESET);

    // Send command and address
    if( HAL_SPI_Transmit(&SPI_HANDLE, params, 3, 500) != HAL_OK ) {
        Error_Handler();
    }

    // Read data
    if( HAL_SPI_Receive(&SPI_HANDLE, buf, len, 500) != HAL_OK ) {
        Error_Handler();
    }

    HAL_GPIO_WritePin(EEPROM_CSB_PORT, EEPROM_CSB_PIN, GPIO_PIN_SET);

    // HAL doesn't give us this data, so assume we read it all
    return (int)len;
}

int pw_eeprom_write(eeprom_addr_t addr, uint8_t *buf, size_t len) {
    uint8_t params[4] = {0};

    size_t n_written = 0;
    eeprom_addr_t addr_end = addr + len;

    eeprom_wait_for_ready();

    // Can only write bytes on the same page, see datasheet section 2.2 Write Sequence
    size_t this_write = EEPROM_PAGE_SIZE-(addr&(EEPROM_PAGE_SIZE-1));
    while(n_written < len) {
        eeprom_wait_for_ready();

        // Enable writes, this may not be necessary for every write instruction
        eeprom_write_cmd(EEPROM_CMD_WREN, 0, params);

        // Write a maximum of 1 page at a time
        this_write = (len>EEPROM_PAGE_SIZE)?EEPROM_PAGE_SIZE:len;

        params[0] = EEPROM_CMD_WRITE;
        params[1] = addr>>8;
        params[2] = addr&0xff;

        HAL_GPIO_WritePin(EEPROM_CSB_PORT, EEPROM_CSB_PIN, GPIO_PIN_RESET);

        // Send command and address
        if( HAL_SPI_Transmit(&SPI_HANDLE, params, 3, 500) != HAL_OK ) {
            Error_Handler();
        }

        // Send data
        if( HAL_SPI_Transmit(&SPI_HANDLE, buf, this_write, 500) != HAL_OK ) {
            Error_Handler();
        }

        HAL_GPIO_WritePin(EEPROM_CSB_PORT, EEPROM_CSB_PIN, GPIO_PIN_SET);

        addr += this_write;
        buf += this_write;
        n_written += this_write;
    }
}

void pw_eeprom_set_area(eeprom_addr_t addr, uint8_t v, size_t len) {
    for(size_t i = 0; i < EEPROM_PAGE_SIZE; i++) EEPROM_WRITE_BUFFER[i] = v;

    // Align first write length to page size
    // Subsequent writes are of length page size or less
    size_t this_write = EEPROM_PAGE_SIZE - (addr&(EEPROM_PAGE_SIZE-1));
    while(len > 0) {
        pw_eeprom_write(addr, EEPROM_WRITE_BUFFER, this_write);
        len -= this_write;
        this_write = (len>EEPROM_PAGE_SIZE)?EEPROM_PAGE_SIZE:len;
    }

}
