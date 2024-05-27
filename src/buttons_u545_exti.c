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

        // Software debounce
        if((HAL_GetTick() - last_press) < DEBOUNCE_TIME_MS)
            return;
        last_press = HAL_GetTick();

        uint8_t b = 0;
        if(pin == BUT1_PIN) b |= BUTTON_L;
        if(pin == BUT2_PIN) b |= BUTTON_M;
        if(pin == BUT3_PIN) b |= BUTTON_R;
        pw_button_callback(b);

    } else {
        pw_log(2, "EXTI handled, not from buttons\r\n");
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

