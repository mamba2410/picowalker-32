#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "picowalker-defs.h"
#include "peripherals.h"

#include "stm32u5xx_hal.h"
#include "stm32u5xx_hal_irda.h"

extern IRDA_HandleTypeDef IRDA_HANDLE;

#define RX_INITIAL_TIMEOUT_US 50000 // 50ms
#define RX_BYTE_TIMEOUT_US 3742 // 3.742ms

/**
 * Spinlock until the flag specified goes high.
 * 
 * Current implementation won't work, see uart version
 * for more details
 * 
 * 
 * @param hirda Handle to the irda structure to test
 * @param flag Flag to check for
 * @param timeout Time, in microseconds, to wait before timeout
 * @return `true` if we timed out, false if the flag went high
 */
static bool irda_wait_on_flag_until_timeout(IRDA_HandleTypeDef *hirda, uint32_t flag, uint32_t timeout) {

    uint32_t start = HAL_GetTick();
    int32_t diff = 0;
    while(__HAL_IRDA_GET_FLAG(hirda, flag) != 0) {
        diff = HAL_GetTick() - start;
        if(diff >= timeout) return true;
    }

    return false;
}

/*
 * ==========================================
 * Driver functions
 * ==========================================
 */

void pw_ir_init() {
    // nothing to do?
}

int pw_ir_read(uint8_t *buf, size_t len) {

    size_t cursor = 0;
    uint8_t *read_head = buf;
    bool timeout = false;

    timeout = irda_wait_on_flag_until_timeout(&IRDA_HANDLE, IRDA_FLAG_RXNE, RX_INITIAL_TIMEOUT_US);
    if(timeout) return 0;

    do {
        // Wait for RXNE
        timeout = irda_wait_on_flag_until_timeout(&IRDA_HANDLE, IRDA_FLAG_RXNE, RX_BYTE_TIMEOUT_US);

        // If we didn't timeout
        if(timeout == HAL_OK) {
            (void)HAL_IRDA_Receive(&IRDA_HANDLE, read_head, 1, RX_BYTE_TIMEOUT_US);
            read_head++;
            cursor++;
        }

    } while( !timeout );

    return (int)cursor;
    
}

int pw_ir_write(uint8_t *buf, size_t len) {

    if(HAL_IRDA_Transmit(&IRDA_HANDLE, buf, len, HAL_IRDA_DEFAULT_TIMEOUT) != HAL_OK) {
        Error_Handler();
    }

    return len;
}