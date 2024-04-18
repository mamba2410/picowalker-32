#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "picowalker-defs.h"
#include "peripherals.h"

#include "stm32u5xx_hal.h"
#include "stm32u5xx_hal_spi.h"

extern SPI_HandleTypeDef SPI_HANDLE;

enum {
    FLASH_CMD_NORD   = 0x03, // normal read
    FLASH_CMD_WREN   = 0x06, // write enable
    FLASH_CMD_WRDI   = 0x04, // write disable
    FLASH_CMD_RDSR   = 0x05, // read status reg
    FLASH_CMD_WRSR   = 0x01, // write status reg
    FLASH_CMD_DP     = 0xb9, // deep power down
    FLASH_CMD_RDID   = 0xab, // read id and release power down, 3 dummy bytes then 1 byte id
    FLASH_CMD_RDJDID = 0x9f, // read JEDEC id
    FLASH_CMD_RDMDID = 0x90, // read manufacturer and device id, 2 dummy bytes then 3 bytes?
    FLASH_CMD_RDUID  = 0x4b, // read unique id
    FLASH_CMD_RSTEN  = 0x66, // softawre reset enable
    FLASH_CMD_RST    = 0x99, // software reset
    FLASH_CMD_NOP    = 0x00, // no-op
    FLASH_CMD_FRQO   = 0x6b, // fast read quad output, 1 dummy byte, no read limit
    FLASH_CMD_PP     = 0x02, // page program, max 256 bytes, needs erasing first
    FLASH_CMD_PPQ    = 0x32, // page program quad
    FLASH_CMD_SER    = 0xd7, // sector erase (4k)
    FLASH_CMD_BER32  = 0x52, // block erase (32k)
    FLASH_CMD_BER64  = 0xd8, // block erase (64k)
    FLASH_CMD_CER    = 0xc7, // chip erase
}

#define FLASH_STATUS_WIP_MASK   0x01 // write in progress
#define FLASH_STATUS_WIP_OFFSET 0x00
#define FLASH_STATUS_QE_MASK    0x01 // quad enable, must be set for qspi operations
#define FLASH_STATUS_QE_OFFSET  0x06

#define FLASH_PAGE_SIZE 256    // maximum bytes available to program at once
#define FLASH_SECTOR_SIZE 4096 // minimum number of bytes can erase

static void flash_send_cmd(uint8_t cmd, size_t len, uint8_t buf[len]) {

    HAL_GPIO_WritePin(FLASH_CSB_PORT, FLASH_CSB_PIN, GPIO_PIN_RESET);

    uint8_t params[4];
    params[0] = cmd;
    params[1] = address>>16; // BE
    params[2] = address>>8;
    params[3] = address&0xff; 

    // Send instruction and address
    if( HAL_SPI_Transmit(&SPI_HANDLE, params, 4, HAL_SPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK ) {
        Error_Handler();
    }

    HAL_GPIO_WritePin(FLASH_CSB_PORT, FLASH_CSB_PIN, GPIO_PIN_SET);
}

static void flash_send_cmd_with_addr(uint8_t cmd, uint32_t addr, size_t len, uint8_t buf[len]) {

    HAL_GPIO_WritePin(FLASH_CSB_PORT, FLASH_CSB_PIN, GPIO_PIN_RESET);

    uint8_t params[4];
    params[0] = cmd;
    params[1] = address>>16; // BE
    params[2] = address>>8;
    params[3] = address&0xff; 

    // Send instruction and address
    if( HAL_SPI_Transmit(&SPI_HANDLE, params, 4, HAL_SPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK ) {
        Error_Handler();
    }

    if(len > 0 && buf != 0) {
        // Send data
        if( HAL_SPI_Transmit(&SPI_HANDLE, buf, len, HAL_SPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK ) {
            Error_Handler();
        }

    }

    HAL_GPIO_WritePin(FLASH_CSB_PORT, FLASH_CSB_PIN, GPIO_PIN_SET);
}

static void flash_read_status_reg(size_t len, uint8_t buf[len]) {
    uint8_t cmd = FLASH_CMD_RDSR;

    HAL_GPIO_WritePin(FLASH_CSB_PORT, FLASH_CSB_PIN, GPIO_PIN_RESET);


    // Send instruction
    if( HAL_SPI_Transmit(&SPI_HANDLE, &cmd, 1, HAL_SPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK ) {
        Error_Handler();
    }

    // Read status reg data out
    if( HAL_SPI_Receive(&SPI_HANDLE, buf, len, HAL_SPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK ) {
        Error_Handler();
    }

    HAL_GPIO_WritePin(FLASH_CSB_PORT, FLASH_CSB_PIN, GPIO_PIN_SET);
}

/*
 * ==========================================
 * Driver functions
 * ==========================================
 */

void pw_flash_init() {
    HAL_GPIO_WritePin(FLASH_CSB_PORT, FLASH_CSB_PIN, GPIO_PIN_SET);

    // maybe reset enable then software reset?
}

int pw_flash_read(uint32_t address, uint8_t *buf, size_t len) {

    uint8_t buf[4] = {0};
    HAL_GPIO_WritePin(FLASH_CSB_PORT, FLASH_CSB_PIN, GPIO_PIN_RESET);

    buf[0] = FLASH_CMD_NORD;
    buf[1] = address>>16; // BE
    buf[2] = address>>8;
    buf[3] = address&0xff; 

    // Send instruction and address
    if( HAL_SPI_Transmit(&SPI_HANDLE, buf, 4, HAL_SPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK ) {
        Error_Handler();
    }

    // Read actual data
    if( HAL_SPI_Receive(&SPI_HANDLE, buf, len, HAL_SPI_TIMEOUT_DEFAULT_VALUE) != HAL_OK ) {
        Error_Handler();
    }

    HAL_GPIO_WritePin(FLASH_CSB_PORT, FLASH_CSB_PIN, GPIO_PIN_SET);
}

int pw_flash_write(uint32_t address, uint8_t *buf, size_t len) {
    // Note: Flash must be erased before programmed.
    // Note: Smallest erase block is 4k
    /*
     * Write sequence
     * - wait for WIP
     * - set write enable
     * - erase sector/block (set to all 1s)
     * - wait for WIP
     * - set write enable
     * - send write data (up to 256 bytes)
     * - repeat
     */

    //


}
