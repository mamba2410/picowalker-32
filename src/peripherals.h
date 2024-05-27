#ifndef PW_PERIPHERALS_H
#define PW_PERIPHERALS_H

#include "stm32u5xx_hal.h"

/*
 * Ports and pins
 */

#define IR_TX_PIN   GPIO_PIN_0
#define IR_TX_PORT  GPIOA
#define IR_RX_PIN   GPIO_PIN_1
#define IR_RX_PORT  GPIOA

#define SCREEN_OSPI_NCS_PIN     GPIO_PIN_2
#define SCREEN_OSPI_NCS_PORT    GPIOA
#define SCREEN_OSPI_CLK_PIN     GPIO_PIN_3
#define SCREEN_OSPI_CLK_PORT    GPIOA
#define SCREEN_OSPI_IO0_PIN     GPIO_PIN_1
#define SCREEN_OSPI_IO0_PORT    GPIOB
#define SCREEN_OSPI_IO1_PIN     GPIO_PIN_0
#define SCREEN_OSPI_IO1_PORT    GPIOB
#define SCREEN_OSPI_IO2_PIN     GPIO_PIN_7
#define SCREEN_OSPI_IO2_PORT    GPIOA
#define SCREEN_OSPI_IO3_PIN     GPIO_PIN_6
#define SCREEN_OSPI_IO3_PORT    GPIOA
#define SCREEN_RST_PIN          GPIO_PIN_0
#define SCREEN_RST_PORT         GPIOC

#define SPKR_PIN    GPIO_PIN_4
#define SPKR_PORT   GPIOA

#define DBG_TX_PIN  GPIO_PIN_9
#define DBG_TX_PORT GPIOA
#define DBG_RX_PIN  GPIO_PIN_10
#define DBG_RX_PORT GPIOA

#define USB_DM_PIN  GPIO_PIN_11
#define USB_DM_PORT GPIOA
#define USB_DP_PIN  GPIO_PIN_12
#define USB_DP_PORT GPIOA

#define SWDIO_PIN   GPIO_PIN_13
#define SWDIO_PORT  GPIOA
#define SWCLK_PIN   GPIO_PIN_14
#define SWCLK_PORT  GPIOA

#define COMP_PIN    GPIO_PIN_4
#define COMP_PORT   GPIOB

#define BAT_LO_PIN  GPIO_PIN_8
#define BAT_LO_PORT GPIOB

#define SPI_SCK_PIN     GPIO_PIN_13
#define SPI_SCK_PORT    GPIOB
#define SPI_MOSI_PIN    GPIO_PIN_14
#define SPI_MOSI_PORT   GPIOB
#define SPI_MISO_PIN    GPIO_PIN_15
#define SPI_MISO_PORT   GPIOB

#define EEPROM_CSB_PIN  GPIO_PIN_8
#define EEPROM_CSB_PORT GPIOA
#define FLASH_CSB_PIN   GPIO_PIN_15
#define FLASH_CSB_PORT  GPIOA
#define ACCEL_CSB_PIN   GPIO_PIN_5
#define ACCEL_CSB_PORT  GPIOB

#define I2C_SCL_PIN     GPIO_PIN_6
#define I2C_SCL_PORT    GPIOB
#define I2C_SDA_PIN     GPIO_PIN_7
#define I2C_SDA_PORT    GPIOB

#define BUT1_PIN        GPIO_PIN_12
#define BUT1_PORT       GPIOC
#define BUT2_PIN        GPIO_PIN_11
#define BUT2_PORT       GPIOC
#define BUT3_PIN        GPIO_PIN_10
#define BUT3_PORT       GPIOC
#define ACCEL_INT_PIN   GPIO_PIN_1
#define ACCEL_INT_PORT  GPIOH

/*
 * Handles from Core/Src/main.c
 */
#define SCREEN_OSPI_HANDLE  hospi1
#define SPEAKER_DAC_HANDLE  hdac1
#define COMP_HANDLE         hcomp1
#define SPI_HANDLE          hspi2
//#define IRDA_HANDLE         hirda4 // in irda mode
#define IR_UART_HANDLE      huart4 // in uart mode
#define I2C_HANDLE          hi2c1
#define DEBUG_UART_HANDLE   huart1
#define US_TIMER_HANDLE     htim1 // used for ir stuff

/*
 * =================================
 * Extras for nucleo board
 * =================================
 */

#define LED_PIN     GPIO_PIN_5
#define LED_PORT    GPIOA

#define PWR_WKUP_PIN    GPIO_PIN_2
#define PWR_WKUP_PORT   GPIOB

#define BOARD_BUTTON_PIN    GPIO_PIN_13
#define BOARD_BUTTON_PORT   GPIOC

#endif /* PW_PERIPHERALS_H */
