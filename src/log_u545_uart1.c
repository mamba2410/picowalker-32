#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdarg.h>
#include <stdio.h>

#include "picowalker-defs.h"
#include "peripherals.h"
#include "stm32u5xx_hal.h"
#include "stm32u5xx_hal_uart.h"

extern UART_HandleTypeDef DEBUG_UART_HANDLE;

const char* LOG_LEVEL_PREFIX[] = {
    "[FATAL] ",
    "[ERROR] ",
    "[WARN ] ",
    "[INFO ] ",
    "[DEBUG] ",
};


/*
 * required for HAL printf function
 */
int _write(int file, char *ptr, int len) {
    HAL_UART_Transmit(&DEBUG_UART_HANDLE, (uint8_t*)ptr, len, 50);
    return len;
}

/*
 * ==========================================
 * Driver functions
 * ==========================================
 */

void pw_log_init() {

}

void pw_log(pw_log_level_t level, const char* format, ...) {
    va_list va;
    va_start(va, format);

    HAL_UART_Transmit(&DEBUG_UART_HANDLE, (uint8_t*)LOG_LEVEL_PREFIX[level], 8, 50);
    vprintf(format, va);

    va_end(va);
}

