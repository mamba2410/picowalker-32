#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "picowalker-defs.h"
#include "peripherals.h"

#include "stm32u5xx_hal.h"
#include "stm32u5xx_hal_spi.h"

extern SPI_HandleTypeDef SPI_HANDLE;

#define ACCEL_READ_BIT  0x80
#define ACCEL_WRITE_MASK 0x7f
enum {
    ACCEL_REG_CHIPID        = 0x00,
    ACCEL_REG_ERR           = 0x02,
    ACCEL_REG_STATUS        = 0x03,
    ACCEL_REG_ACC_X_LSB     = 0x04,
    ACCEL_REG_ACC_X_MSB     = 0x05,
    ACCEL_REG_ACC_Y_LSB     = 0x06,
    ACCEL_REG_ACC_Y_MSB     = 0x07,
    ACCEL_REG_ACC_Z_LSB     = 0x08,
    ACCEL_REG_ACC_Z_MSB     = 0x09,
    ACCEL_REG_TEMP          = 0x11,
    ACCEL_REG_STEP_CNT_0    = 0x15,
    ACCEL_REG_STEP_CNT_1    = 0x16,
    ACCEL_REG_STEP_CNT_2    = 0x17,
    ACCEL_REG_STEP_STAT     = 0x18,
    ACCEL_REG_ACC_CONFIG0   = 0x19,
    ACCEL_REG_ACC_CONFIG1   = 0x1a,
    ACCEL_REG_ACC_CONFIG2   = 0x1b,
    ACCEL_REG_INT_CONFIG0   = 0x1f,
    ACCEL_REG_INT_CONFIG1   = 0x20,
    ACCEL_REG_INT1_MAP      = 0x21,
    ACCEL_REG_INT2_MAP      = 0x22,
    ACCEL_REG_INT12_MAP     = 0x23,
    ACCEL_REG_AUTOLOWPOW0   = 0x2b,
    ACCEL_REG_AUTOLOWPOW1   = 0x2c,
    ACCEL_REG_AUTOWAKEUP0   = 0x2d,
    ACCEL_REG_AUTOWAKEUP1   = 0x2e,
};

#define ACCEL_POWER_MASK    0x03
#define ACCEL_POWER_OFFSET  1
enum {
    ACCEL_POWER_SLEEP  = 0x00,
    ACCEL_POWER_LOW    = 0x01,
    ACCEL_POWER_NORMAL = 0x02,
};

#define ACCEL_STEP_STAT_MASK 0x03
#define ACCEL_STEP_STAT_OFFSET 0
typedef enum {
    ACCEL_STEP_STAT_STILL   = 0x00,
    ACCEL_STEP_STAT_WALKING = 0x01,
    ACCEL_STEP_STAT_RUNNING = 0x02,
} accel_step_stat_t;

#define INT_CONFIG1_STEP_INT_EN 0x01 // step taken interrupt enable
#define INT1_MAP_STEP 0x01 // map step taken interrupt to pin INT1

#define ACCEL_CHIPID 0x90
static uint32_t total_steps = 0;

/**
 * Read data from a register on the chip
 *
 * The register is sent, then we receive a dummy byte and
 * the actual data follows.
 *
 * @param len Number of bytes to read back
 * @param buf Buffer to receive data into
 */
static void accel_read_reg(uint8_t reg, size_t len, uint8_t buf[len]) {
    reg |= ACCEL_READ_BIT;
    HAL_GPIO_WritePin(ACCEL_CSB_PORT, ACCEL_CSB_PIN, GPIO_PIN_RESET);

    uint8_t txbuf[2] = {reg, 0};

    // Send command and read dummy in one go
    if( HAL_SPI_TransmitReceive(&SPI_HANDLE, txbuf, txbuf, 2, 500) != HAL_OK) {
        Error_Handler();
    }

    // Read actual data
    if( HAL_SPI_Receive(&SPI_HANDLE, buf, len, 500) != HAL_OK ) {
        Error_Handler();
    }

    HAL_GPIO_WritePin(ACCEL_CSB_PORT, ACCEL_CSB_PIN, GPIO_PIN_SET);
}

/**
 * Write buffer to register on chip.
 *
 * @param reg The register to write to
 * @param len Number of bytes to send
 * @param buf Buffer to send
 */
static void accel_write_reg(uint8_t reg, size_t len, uint8_t buf[len]) {
    reg &= ACCEL_WRITE_MASK;
    HAL_GPIO_WritePin(ACCEL_CSB_PORT, ACCEL_CSB_PIN, GPIO_PIN_RESET);

    // Send command
    if( HAL_SPI_Transmit(&SPI_HANDLE, &reg, 1, 500) != HAL_OK ) {
        Error_Handler();
    }

    // Send data
    if( HAL_SPI_Transmit(&SPI_HANDLE, buf, len, 500) != HAL_OK ) {
        Error_Handler();
    }

    HAL_GPIO_WritePin(ACCEL_CSB_PORT, ACCEL_CSB_PIN, GPIO_PIN_SET);
}

/**
 * Read steps as 24-bit LE int.
 * Check the power status first, if asleep, wake it up.
 *
 * @param len Max length of the buffer
 * @param buf Buffer to use when sending messages
 * @return Number of steps reported by chip
 */
static uint32_t accel_read_steps(size_t len, uint8_t buf[len]) {

    accel_read_reg(ACCEL_REG_STATUS, 1, buf);
    uint8_t power_mode = (buf[0]>>ACCEL_POWER_OFFSET) & ACCEL_POWER_MASK;
    if(power_mode == ACCEL_POWER_SLEEP) {
        // power offset in config reg is different to status reg
        buf[0] = ACCEL_POWER_NORMAL;
        accel_write_reg(ACCEL_REG_ACC_CONFIG0, 1, buf);
        HAL_Delay(2);
    }

    // read steps
    accel_read_reg(ACCEL_REG_STEP_CNT_0, 4, buf);

    // Reg is little endian
    uint32_t steps = (buf[2]<<16) | (buf[1]<<8) | (buf[0]<<0);
    accel_step_stat_t step_stat = (buf[3] >> ACCEL_STEP_STAT_OFFSET)&ACCEL_STEP_STAT_MASK;
}

/*
 * ==========================================
 * Driver functions
 * ==========================================
 */

void pw_accel_init() {
    // Ensure CS pin is high
    HAL_GPIO_WritePin(ACCEL_CSB_PORT, ACCEL_CSB_PIN, GPIO_PIN_SET);
    HAL_Delay(1);

    uint8_t buf[4] = {0};

    // Check if chip is on bus
    accel_read_reg(ACCEL_REG_CHIPID, 1, buf);
    if(buf[0] != ACCEL_CHIPID) {
        Error_Handler();
    }

    // Enable interrupts to detect step taken
    buf[0] = INT_CONFIG1_STEP_INT_EN;
    accel_write_reg(ACCEL_REG_INT_CONFIG1, 1, buf);

    // Map step interrupt to chip pin INT1
    buf[0] = INT1_MAP_STEP;
    accel_write_reg(ACCEL_REG_INT12_MAP, 1, buf);

    // Interrupt pins by default are active low, push-pull output

    // reset total steps reported by chip
    total_steps = 0;
}

uint32_t pw_accel_get_new_steps() {

    uint8_t buf[4];

    uint32_t read_steps = accel_read_steps(4, buf);
    int32_t new_steps = read_steps - total_steps;
    if(new_steps < 0) new_steps = 0;
    total_steps = read_steps;

    return (uint32_t)new_steps;
}
