##########################################################################################################################
# File automatically-generated by tool: [projectgenerator] version: [4.2.0-B44] date: [Sat Apr 13 13:42:58 BST 2024]
##########################################################################################################################

# ------------------------------------------------
# Generic Makefile (based on gcc)
#
# ChangeLog :
#	2017-02-10 - Several enhancements + project update mode
#   2015-07-22 - first version
# ------------------------------------------------

######################################
# target
######################################
TARGET = picowalker-32-nucleo


######################################
# building variables
######################################
# debug build?
DEBUG = 1
# optimization
OPT = -Og


#######################################
# paths
#######################################
# Build path
BUILD_DIR = build

######################################
# source
######################################
# C sources
C_SOURCES =  \
Core/Src/main.c \
Core/Src/stm32u5xx_it.c \
Core/Src/stm32u5xx_hal_msp.c \
Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_adc.c \
Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_adc_ex.c \
Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_dma.c \
Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_dma_ex.c \
Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal.c \
Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_i2c.c \
Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_i2c_ex.c \
Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_rcc.c \
Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_rcc_ex.c \
Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_cortex.c \
Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_flash.c \
Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_flash_ex.c \
Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_gpio.c \
Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_exti.c \
Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_pwr.c \
Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_pwr_ex.c \
Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_gtzc.c \
Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_icache.c \
Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_comp.c \
Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_dac.c \
Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_dac_ex.c \
Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_ospi.c \
Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_ll_dlyb.c \
Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_spi.c \
Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_spi_ex.c \
Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_tim.c \
Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_tim_ex.c \
Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_irda.c \
Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_uart.c \
Drivers/STM32U5xx_HAL_Driver/Src/stm32u5xx_hal_uart_ex.c \
Core/Src/system_stm32u5xx.c \
src/pw_main.c \
src/screen_u545_dwo.c


# ASM sources
ASM_SOURCES =  \
startup_stm32u545xx.s

# ASM sources
ASMM_SOURCES =


#######################################
# binaries
#######################################
PREFIX = arm-none-eabi-
# The gcc compiler bin path can be either defined in make command via GCC_PATH variable (> make GCC_PATH=xxx)
# either it can be added to the PATH environment variable.
ifdef GCC_PATH
CC = $(GCC_PATH)/$(PREFIX)gcc
AS = $(GCC_PATH)/$(PREFIX)gcc -x assembler-with-cpp
CP = $(GCC_PATH)/$(PREFIX)objcopy
SZ = $(GCC_PATH)/$(PREFIX)size
else
CC = $(PREFIX)gcc
AS = $(PREFIX)gcc -x assembler-with-cpp
CP = $(PREFIX)objcopy
SZ = $(PREFIX)size
endif
HEX = $(CP) -O ihex
BIN = $(CP) -O binary -S

#######################################
# CFLAGS
#######################################
# cpu
CPU = -mcpu=cortex-m33

# fpu
FPU = -mfpu=fpv4-sp-d16

# float-abi
FLOAT-ABI = -mfloat-abi=hard

# mcu
MCU = $(CPU) -mthumb $(FPU) $(FLOAT-ABI)

# macros for gcc
# AS defines
AS_DEFS =

# C defines
C_DEFS =  \
-DUSE_HAL_DRIVER \
-DSTM32U545xx


# AS includes
AS_INCLUDES =

# C includes
C_INCLUDES =  \
-ICore/Inc \
-IDrivers/STM32U5xx_HAL_Driver/Inc \
-IDrivers/STM32U5xx_HAL_Driver/Inc/Legacy \
-IDrivers/CMSIS/Device/ST/STM32U5xx/Include \
-IDrivers/CMSIS/Include \
-Isrc


# compile gcc flags
ASFLAGS = $(MCU) $(AS_DEFS) $(AS_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections

CFLAGS += $(MCU) $(C_DEFS) $(C_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections

ifeq ($(DEBUG), 1)
CFLAGS += -g -gdwarf-2
endif


# Generate dependency information
CFLAGS += -MMD -MP -MF"$(@:%.o=%.d)"


#######################################
# LDFLAGS
#######################################
# link script
LDSCRIPT = STM32U545xx_FLASH.ld

# libraries
LIBS = -lc -lm -lnosys
LIBDIR =
LDFLAGS = $(MCU) -specs=nano.specs -T$(LDSCRIPT) $(LIBDIR) $(LIBS) -Wl,-Map=$(BUILD_DIR)/$(TARGET).map,--cref -Wl,--gc-sections

# default action: build all
all: $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/$(TARGET).hex $(BUILD_DIR)/$(TARGET).bin


#######################################
# build the application
#######################################
# list of objects
OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
vpath %.c $(sort $(dir $(C_SOURCES)))
# list of ASM program objects
OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(ASM_SOURCES:.s=.o)))
vpath %.s $(sort $(dir $(ASM_SOURCES)))
OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(ASMM_SOURCES:.S=.o)))
vpath %.S $(sort $(dir $(ASMM_SOURCES)))

$(BUILD_DIR)/%.o: %.c Makefile | $(BUILD_DIR)
	$(CC) -c $(CFLAGS) -Wa,-a,-ad,-alms=$(BUILD_DIR)/$(notdir $(<:.c=.lst)) $< -o $@

$(BUILD_DIR)/%.o: %.s Makefile | $(BUILD_DIR)
	$(AS) -c $(CFLAGS) $< -o $@
$(BUILD_DIR)/%.o: %.S Makefile | $(BUILD_DIR)
	$(AS) -c $(CFLAGS) $< -o $@

$(BUILD_DIR)/$(TARGET).elf: $(OBJECTS) Makefile
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@
	$(SZ) $@

$(BUILD_DIR)/%.hex: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(HEX) $< $@

$(BUILD_DIR)/%.bin: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(BIN) $< $@

$(BUILD_DIR):
	mkdir $@

#######################################
# clean up
#######################################
clean:
	-rm -fR $(BUILD_DIR)

#######################################
# dependencies
#######################################
-include $(wildcard $(BUILD_DIR)/*.d)

# *** EOF ***
