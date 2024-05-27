#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "picowalker-defs.h"
#include "peripherals.h"

#include "stm32u5xx_hal_tim.h"

extern TIM_HandleTypeDef US_TIMER_HANDLE;
static uint16_t last_press = 0;

void HAL_GPIO_EXTI_Falling_Callback(uint16_t pin) {
    if( pin == BUT1_PIN || pin == BUT2_PIN || pin == BUT3_PIN ) {

        if((HAL_GetTick() - last_press) < DEBOUNCE_TIME_MS)
            return;

        last_press = HAL_GetTick();
        char c = (pin == BUT1_PIN ? 'L' : (pin == BUT2_PIN ? 'M' : 'R'));
        pw_log(3, "Button %c pressed\r\n", c);

    } else {
        pw_log(3, "EXTI handled, not from buttons\r\n");
    }
}

/*
 * ==========================================
 * Driver functions
 * ==========================================
 */

void pw_button_init() {
    // nothing to do
}

