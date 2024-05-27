#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "picowalker-defs.h"
#include "peripherals.h"

#include "stm32u5xx_hal.h"
#include "stm32u5xx_hal_irda.h"
#include "stm32u5xx_hal_tim.h"

extern UART_HandleTypeDef IR_UART_HANDLE;
extern TIM_HandleTypeDef US_TIMER_HANDLE;

#define RX_INITIAL_TIMEOUT_US 50000 // 50ms
#define RX_BYTE_TIMEOUT_US 3742 // 3.742ms

/**
 * Spinlock until the flag specified goes high.
 *
 * HAL provides this, but only internally, so we make our own.
 * Note: `HAL_GetTick()` only has millisecond-accuracy, so we can't use it.
 *
 * Idea: Create a 1-shot (or countdown) timer, load it on function start and have it run out
 * after `timeout` microseconds.
 * If the flag goes high before timer runs out, we stop timer and return happy.
 * If the timer runs out, we just exit loop and say we timed out.
 * 16-bit timer is enough for this, provided we run at 1MHz.
 * We might not even need to start it at the start of function, as long as it ticks
 * at 1MHz and we sample it before it hits the start timestamp again then we're fine
 *
 * @param huart Handle to the uart structure to test
 * @param flag Flag to check for
 * @param timeout Time, in microseconds, to wait before timeout
 * @return `true` if we timed out, false if the flag went high
 */
static bool uart_wait_on_flag_until_timeout(UART_HandleTypeDef *huart, uint32_t flag, uint16_t timeout) {

    uint16_t start=0, diff=0, now=0;

    //start = __HAL_TIM_GET_COUNTER(&TIM1);
    //start = TIM_GetCounter(&US_TIMER_HANDLE);
    start = US_TIMER_HANDLE.Instance->CNT;

    while(__HAL_UART_GET_FLAG(huart, flag) != 0) {
        //now = __HAL_TIM_GET_COUNTER(&TIM1);
        //now = TIM_GetCounter(&US_TIMER_HANDLE);
        now = US_TIMER_HANDLE.Instance->CNT;
        diff = now - start; // Ok to roll over, as long as the interval is less than 16-bit max uint
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
    //HAL_TIM_Cmd(&US_TIMER_HANDLE, ENABLE);
    HAL_TIM_Base_Start(&US_TIMER_HANDLE);
}

int pw_ir_read(uint8_t *buf, size_t len) {

    /*
     * Copying logic from pico version:
     * Wait 50ms after function call, if we have nothing then timeout
     * If next byte takes longer than 3742us then stop reading and return buffer
     *
     * Note: needs timestamp of time in us. If needed, can start a freerunning timer
     * on function entrance and stop it on exit
     * See HAL_GetTick()
     */

    size_t cursor = 0;
    uint8_t *read_head = buf;
    bool timeout = HAL_OK;

    timeout = uart_wait_on_flag_until_timeout(&IR_UART_HANDLE, UART_FLAG_RXNE, RX_INITIAL_TIMEOUT_US);
    if(!timeout) return 0;

    do {
        // Wait for RXNE
        timeout = uart_wait_on_flag_until_timeout(&IR_UART_HANDLE, UART_FLAG_RXNE, RX_BYTE_TIMEOUT_US);

        // If we didn't timeout
        if(timeout) {
            (void)HAL_UART_Receive(&IR_UART_HANDLE, read_head, 1, 4);
            //*read_head = IR_UART_HANDLE.Instance->RDR;
            read_head++;
            cursor++;
        }

    } while( !timeout );

    return (int)cursor;
}

int pw_ir_write(uint8_t *buf, size_t len) {
    HAL_StatusTypeDef c;
    if((c = HAL_UART_Transmit(&IR_UART_HANDLE, buf, len, 5000)) != HAL_OK) {
        pw_log(1, "IR transmit failed: %d\r\n", c);
        //Error_Handler();
        return 0;
    }

    return len;
}
