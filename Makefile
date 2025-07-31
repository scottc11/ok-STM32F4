
######################################
# source
######################################
# C sources

C_SOURCES += \
$(LIB_PATH)/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_tim.c \
$(LIB_PATH)/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_tim_ex.c \
$(LIB_PATH)/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc.c \
$(LIB_PATH)/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc_ex.c \
$(LIB_PATH)/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash.c \
$(LIB_PATH)/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash_ex.c \
$(LIB_PATH)/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash_ramfunc.c \
$(LIB_PATH)/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_gpio.c \
$(LIB_PATH)/STM32F4xx_HAL_Driver/Src/stm32f4xx_ll_gpio.c \
$(LIB_PATH)/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma_ex.c \
$(LIB_PATH)/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma.c \
$(LIB_PATH)/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr.c \
$(LIB_PATH)/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr_ex.c \
$(LIB_PATH)/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_cortex.c \
$(LIB_PATH)/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal.c \
$(LIB_PATH)/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_exti.c \
$(LIB_PATH)/STM32F4xx_HAL_Driver/Inc/stm32f4xx_ll_exti.c \
$(LIB_PATH)/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_adc.c \
$(LIB_PATH)/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_adc_ex.c \
$(LIB_PATH)/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dac.c \
$(LIB_PATH)/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dac_ex.c \
$(LIB_PATH)/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_i2c.c \
$(LIB_PATH)/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_i2c_ex.c \
$(LIB_PATH)/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_spi.c \
$(LIB_PATH)/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_uart.c \
$(LIB_PATH)/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pcd.c \
$(LIB_PATH)/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pcd_ex.c \
$(LIB_PATH)/STM32F4xx_HAL_Driver/Src/stm32f4xx_ll_usb.c \
$(LIB_PATH)/middleware/FreeRTOS/Source/croutine.c \
$(LIB_PATH)/middleware/FreeRTOS/Source/event_groups.c \
$(LIB_PATH)/middleware/FreeRTOS/Source/list.c \
$(LIB_PATH)/middleware/FreeRTOS/Source/queue.c \
$(LIB_PATH)/middleware/FreeRTOS/Source/stream_buffer.c \
$(LIB_PATH)/middleware/FreeRTOS/Source/tasks.c \
$(LIB_PATH)/middleware/FreeRTOS/Source/timers.c \
$(LIB_PATH)/middleware/FreeRTOS/Source/CMSIS_RTOS_V2/cmsis_os2.c \
$(LIB_PATH)/middleware/FreeRTOS/Source/portable/MemMang/heap_4.c \
$(LIB_PATH)/middleware/FreeRTOS/Source/portable/GCC/ARM_CM4F/port.c \
$(LIB_PATH)/middleware/USB_DEVICE/App/usb_device.c \
$(LIB_PATH)/middleware/USB_DEVICE/App/usbd_desc.c \
$(LIB_PATH)/middleware/USB_DEVICE/App/usbd_cdc_if.c \
$(LIB_PATH)/middleware/USB_DEVICE/Target/usbd_conf.c \
$(LIB_PATH)/middleware/STM32_USB_Device_Library/Core/Src/usbd_core.c \
$(LIB_PATH)/middleware/STM32_USB_Device_Library/Core/Src/usbd_ctlreq.c \
$(LIB_PATH)/middleware/STM32_USB_Device_Library/Core/Src/usbd_ioreq.c \
$(LIB_PATH)/middleware/STM32_USB_Device_Library/Class/CDC/Src/usbd_cdc.c \
$(LIB_PATH)/system/Src/freertos.c \
$(LIB_PATH)/system/Src/stm32f4xx_hal_timebase_tim.c \
$(LIB_PATH)/system/Src/stm32f4xx_hal_msp.c \
$(LIB_PATH)/system/Src/stm32f4xx_it.c \
$(LIB_PATH)/system/Src/system_stm32f4xx.c \
$(LIB_PATH)/system/Src/system_clock_config.c

# C_SOURCES += $(shell find $(LIB_PATH)/middleware/STM32_USB_Device_Library -name '*.c')
# C_SOURCES += $(shell find $(LIB_PATH)/middleware/USB_DEVICE -name '*.c')

CPP_SOURCES += $(shell find $(LIB_PATH)/HAL -name '*.cpp')
CPP_SOURCES += $(shell find $(LIB_PATH)/RTOS -name '*.cpp')
CPP_SOURCES += $(shell find $(LIB_PATH)/misc -name '*.cpp')
CPP_SOURCES += $(shell find $(LIB_PATH)/utils -name '*.cpp')


C_INCLUDES += \
-I$(LIB_PATH)/HAL/Inc \
-I$(LIB_PATH)/RTOS/Inc \
-I$(LIB_PATH)/cxxsupport \
-I$(LIB_PATH)/STM32F4xx_HAL_Driver/Inc \
-I$(LIB_PATH)/STM32F4xx_HAL_Driver/Inc/Legacy \
-I$(LIB_PATH)/CMSIS/Include \
-I$(LIB_PATH)/CMSIS/Device/ST/STM32F4xx/Include \
-I$(LIB_PATH)/CMSIS/DSP/Include \
-I$(LIB_PATH)/middleware/FreeRTOS/Source/include \
-I$(LIB_PATH)/middleware/FreeRTOS/Source/CMSIS_RTOS_V2 \
-I$(LIB_PATH)/middleware/FreeRTOS/Source/portable/GCC/ARM_CM4F \
-I$(LIB_PATH)/middleware/STM32_USB_Device_Library/Class/CDC/Inc \
-I$(LIB_PATH)/middleware/STM32_USB_Device_Library/Core/Inc \
-I$(LIB_PATH)/middleware/USB_DEVICE/App \
-I$(LIB_PATH)/middleware/USB_DEVICE/Target \
-I$(LIB_PATH)/misc \
-I$(LIB_PATH)/system/Inc \
-I$(LIB_PATH)/utils \

# C++ includes
CPP_INCLUDES = \

# AS includes
AS_INCLUDES = 

# ASM sources ("Assembly Language") - defines main() function
ASM_SOURCES =  \
$(LIB_PATH)/startup_stm32f446xx.s

#######################################
# LDFLAGS
#######################################
# link script
LDSCRIPT = $(LIB_PATH)/STM32F446RETx_FLASH.ld


#######################################
# binaries
#######################################
PREFIX = arm-none-eabi-
# The gcc compiler bin path can be either defined in make command via GCC_PATH variable (> make GCC_PATH=xxx)
# either it can be added to the PATH environment variable.
ifdef GCC_PATH
CC = $(GCC_PATH)/$(PREFIX)gcc
CXX = $(GCC_PATH)/$(PREFIX)g++
AS = $(GCC_PATH)/$(PREFIX)gcc -x assembler-with-cpp
CP = $(GCC_PATH)/$(PREFIX)objcopy
SZ = $(GCC_PATH)/$(PREFIX)size
AR = $(GCC_PATH)/$(PREFIX)ar
GDB = $(GCC_PATH)/$(PREFIX)gdb
else
CC = $(PREFIX)gcc
CXX = $(PREFIX)g++
AS = $(PREFIX)gcc -x assembler-with-cpp
CP = $(PREFIX)objcopy
SZ = $(PREFIX)size
AR = $(PREFIX)ar
GDB = $(PREFIX)gdb
endif
HEX = $(CP) -O ihex
BIN = $(CP) -O binary -S
 
#######################################
# CFLAGS
#######################################

# optimization
OPT = -Og

# Specify the name of the target CPU.
CPU = -mcpu=cortex-m4

# Specify the name of the target floating point hardware/format.
FPU = -mfpu=fpv4-sp-d16

# Specify if floating point hardware should be used.
FLOAT-ABI = -mfloat-abi=hard

# mcu
MCU = $(CPU) -mthumb $(FPU) $(FLOAT-ABI)

# macros for gcc
# AS defines
AS_DEFS = 

# C defines
C_DEFS =  \
-DUSE_HAL_DRIVER \
-DARM_MATH_CM4 \
-DSTM32F446xx



###########

# -Og                   
# -Wall	Recommended compiler warnings
# -fdata-sections
# -ffunction-sections
# -g    Generate debugging information
# -gdwarf-2
# -MMD
# -MP
# -c                       Compile and assemble, but do not link.
###########

# compile gcc flags
ASFLAGS = $(MCU) $(AS_DEFS) $(AS_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections

CFLAGS = $(MCU) $(C_DEFS) $(C_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections 

ifeq ($(DEBUG), 1)
CFLAGS += -g -gdwarf-2
endif

ifeq ($(SERIAL_DEBUG), 1)
CFLAGS += -DSERIAL_DEBUG=1
endif

# get firmware version from git
ifeq ($(origin FIRMWARE_VERSION), undefined)
    FIRMWARE_VERSION = $(shell git rev-parse --short HEAD)
endif

# pass the firmware version into program
CFLAGS += -DFIRMWARE_VERSION=\"$(FIRMWARE_VERSION)\"

# Generate dependency information
CFLAGS += -MMD -MP -MF"$(@:%.o=%.d)"

# C++ Flags
CPPFLAGS = $(CFLAGS) $(CPP_INCLUDES)
CPPFLAGS += \
-fno-exceptions \
-fno-rtti 

C_STANDARD = -std=gnu11
CPP_STANDARD += -std=gnu++14

# libraries
LIBS = -lc -lm -lnosys 
LIBDIR = 
LDFLAGS = $(MCU) -specs=nano.specs -T$(LDSCRIPT) $(LIBDIR) $(LIBS) -Wl,-Map=$(BUILD_DIR)/$(TARGET).map,--cref -Wl,--gc-sections

# CMSIS ARM math library
LDFLAGS += -L$(LIB_PATH)/CMSIS/Lib/GCC -larm_cortexM4lf_math

# enable printf float support (increases build size)
ifdef USB_DEBUG
LDFLAGS += -u _printf_float -u _scanf_float
endif

# default action: build all
all: $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/$(TARGET).hex $(BUILD_DIR)/$(TARGET).bin

#######################################
# helpers
#######################################
usedFlash = $$( $(SZ) $@ | sed -n 2p | awk '{print $$1}' )
usedFlashPercent = $$(( 100 * $(usedFlash) / $(FLASH_SIZE) ))
flashMessage = Flash Used: $(usedFlash)/$(FLASH_SIZE) ( $(usedFlashPercent) % )
usedRam = $$( $(SZ) $@ | sed -n 2p | awk '{ram=$$2+$$3} {print ram}' )
usedRamPercent = $$(( 100 * $(usedRam) / $(RAM_SIZE) ))
ramMessage = Ram Used: $(usedRam)/$(RAM_SIZE) ( $(usedRamPercent) % ) - (static only)

# Extract FreeRTOS heap size from config file - simplified approach
heapSize = $$( grep 'configTOTAL_HEAP_SIZE' $(LIB_PATH)/system/Inc/FreeRTOSConfig.h | grep -o '[0-9][0-9]*' | tail -1 )
heapPercent = $$(( 100 * $(heapSize) / $(RAM_SIZE) ))
heapMessage = FreeRTOS Heap: $(heapSize)/$(RAM_SIZE) ( $(heapPercent) % ) - (dynamic)
totalRamUsed = $$(( $(usedRam) + $(heapSize) ))
totalRamPercent = $$(( 100 * $(totalRamUsed) / $(RAM_SIZE) ))
totalRamMessage = Total RAM Est: $(totalRamUsed)/$(RAM_SIZE) ( $(totalRamPercent) % ) - (static + heap)

#######################################
# build the application
#######################################
# list of .c objects
OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
vpath %.c $(sort $(dir $(C_SOURCES)))
# list of .cpp objects
OBJECTS += $(addprefix $(BUILD_DIR)/,$(CPP_SOURCES:.cpp=.o))
vpath %.cpp $(sort $(dir $(CPP_SOURCES)))
# list of ASM program objects
OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(ASM_SOURCES:.s=.o)))
vpath %.s $(sort $(dir $(ASM_SOURCES)))

# Progress tracking
TOTAL_FILES := $(words $(OBJECTS))
CURRENT_FILE := 0

define progress_bar
    $(eval CURRENT_FILE=$(shell echo $$(($(CURRENT_FILE)+1))))
    @echo -ne "\033[1;32mBuilding \033[1;34m[$(CURRENT_FILE)/$(TOTAL_FILES)] \033[1;32m- $1\033[0m\r"
endef

$(BUILD_DIR)/%.o: %.c Makefile | $(BUILD_DIR)
	$(call progress_bar,$<)
	@$(CC) -c $(CFLAGS) -Wa,-a,-ad,-alms=$(BUILD_DIR)/$(notdir $(<:.c=.lst)) $< -o $@

$(BUILD_DIR)/%.o: %.cpp Makefile | $(BUILD_DIR)
	@mkdir -p $(@D)
	$(call progress_bar,$<)
	@$(CXX) $(CPPFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: %.s Makefile | $(BUILD_DIR)
	$(call progress_bar,$<)
	@$(AS) -c $(CFLAGS) $< -o $@

$(BUILD_DIR)/$(TARGET).elf: $(OBJECTS) Makefile
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@
	$(SZ) $@
	@echo ""
	@echo "$(flashMessage)"
	@echo "$(ramMessage)"
	@echo "$(heapMessage)"
	@echo "$(totalRamMessage)"
	@echo ""

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
# searches for all .d files in given directory and inserts them into the .c file
#######################################
-include $(wildcard $(BUILD_DIR)/*.d)


######################################
# OpenOCD stuff
# TODO: add config.mk file for settings like programmer, etc.
######################################
CHIPSET ?= stm32f4x
FLASH_ADDRESS ?= 0x08000000

OCD=openocd
OCD_DIR ?= /usr/local/share/openocd/scripts # this value works, but for some reason this folder only exists at path -> /opt/homebrew/Cellar/open-ocd/0.11.0/share/openocd/scripts
PGM_DEVICE ?= interface/stlink.cfg
OCDFLAGS = -f $(PGM_DEVICE) -f target/$(CHIPSET).cfg

program:
	$(OCD) -s $(OCD_DIR) $(OCDFLAGS) \
		-c "program ./$(BUILD_DIR)/$(TARGET).elf verify reset exit"


DFU_INTERFACE_NUMBER = 0
DFU_ALT_SETTING = 0
DFU_FUSE_ADDRESS = $(FLASH_ADDRESS)

usb-upload:
	dfu-util -a $(DFU_ALT_SETTING) -s $(DFU_FUSE_ADDRESS):leave -D $(BUILD_DIR)/$(TARGET).bin
