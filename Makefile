##########################################################################################################################
# File automatically-generated by tool: [projectgenerator] version: [3.16.0] date: [Mon Apr 25 17:07:03 CST 2022] 
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
TARGET = STM32F407_2022_framework


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
Core/Src/gpio.c \
Core/Src/freertos.c \
Core/Src/can.c \
Core/Src/crc.c \
Core/Src/dma.c \
Core/Src/rng.c \
Core/Src/spi.c \
Core/Src/tim.c \
Core/Src/usart.c \
Core/Src/stm32f4xx_it.c \
Core/Src/stm32f4xx_hal_msp.c \
Core/Src/stm32f4xx_hal_timebase_tim.c \
Middlewares/Third_Party/ARM_CMSIS/CMSIS/DSP/Source/BasicMathFunctions/BasicMathFunctions.c \
Middlewares/Third_Party/ARM_CMSIS/CMSIS/DSP/Source/QuaternionMathFunctions/QuaternionMathFunctions.c \
Middlewares/Third_Party/ARM_CMSIS/CMSIS/DSP/Source/BayesFunctions/BayesFunctions.c \
Middlewares/Third_Party/ARM_CMSIS/CMSIS/DSP/Source/CommonTables/CommonTables.c \
Middlewares/Third_Party/ARM_CMSIS/CMSIS/DSP/Source/ComplexMathFunctions/ComplexMathFunctions.c \
Middlewares/Third_Party/ARM_CMSIS/CMSIS/DSP/Source/ControllerFunctions/ControllerFunctions.c \
Middlewares/Third_Party/ARM_CMSIS/CMSIS/DSP/Source/DistanceFunctions/DistanceFunctions.c \
Middlewares/Third_Party/ARM_CMSIS/CMSIS/DSP/Source/FastMathFunctions/FastMathFunctions.c \
Middlewares/Third_Party/ARM_CMSIS/CMSIS/DSP/Source/FilteringFunctions/FilteringFunctions.c \
Middlewares/Third_Party/ARM_CMSIS/CMSIS/DSP/Source/MatrixFunctions/MatrixFunctions.c \
Middlewares/Third_Party/ARM_CMSIS/CMSIS/DSP/Source/StatisticsFunctions/StatisticsFunctions.c \
Middlewares/Third_Party/ARM_CMSIS/CMSIS/DSP/Source/SupportFunctions/SupportFunctions.c \
Middlewares/Third_Party/ARM_CMSIS/CMSIS/DSP/Source/SVMFunctions/SVMFunctions.c \
Middlewares/Third_Party/ARM_CMSIS/CMSIS/DSP/Source/TransformFunctions/TransformFunctions.c \
Middlewares/Third_Party/ARM_CMSIS/CMSIS/DSP/Source/InterpolationFunctions/InterpolationFunctions.c \
Middlewares/Third_Party/ARM_CMSIS/CMSIS/DSP/Source/BasicMathFunctions/BasicMathFunctionsF16.c \
Middlewares/Third_Party/ARM_CMSIS/CMSIS/DSP/Source/ComplexMathFunctions/ComplexMathFunctionsF16.c \
Middlewares/Third_Party/ARM_CMSIS/CMSIS/DSP/Source/FilteringFunctions/FilteringFunctionsF16.c \
Middlewares/Third_Party/ARM_CMSIS/CMSIS/DSP/Source/CommonTables/CommonTablesF16.c \
Middlewares/Third_Party/ARM_CMSIS/CMSIS/DSP/Source/TransformFunctions/TransformFunctionsF16.c \
Middlewares/Third_Party/ARM_CMSIS/CMSIS/DSP/Source/MatrixFunctions/MatrixFunctionsF16.c \
Middlewares/Third_Party/ARM_CMSIS/CMSIS/DSP/Source/InterpolationFunctions/InterpolationFunctionsF16.c \
Middlewares/Third_Party/ARM_CMSIS/CMSIS/DSP/Source/StatisticsFunctions/StatisticsFunctionsF16.c \
Middlewares/Third_Party/ARM_CMSIS/CMSIS/DSP/Source/SupportFunctions/SupportFunctionsF16.c \
Middlewares/Third_Party/ARM_CMSIS/CMSIS/DSP/Source/FastMathFunctions/FastMathFunctionsF16.c \
Middlewares/Third_Party/ARM_CMSIS/CMSIS/DSP/Source/DistanceFunctions/DistanceFunctionsF16.c \
Middlewares/Third_Party/ARM_CMSIS/CMSIS/DSP/Source/BayesFunctions/BayesFunctionsF16.c \
Middlewares/Third_Party/ARM_CMSIS/CMSIS/DSP/Source/SVMFunctions/SVMFunctionsF16.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc_ex.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash_ex.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash_ramfunc.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_gpio.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma_ex.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr_ex.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_cortex.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_exti.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_can.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_crc.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rng.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_spi.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_tim.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_tim_ex.c \
Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_uart.c \
Core/Src/system_stm32f4xx.c \
Middlewares/Third_Party/FreeRTOS/Source/croutine.c \
Middlewares/Third_Party/FreeRTOS/Source/event_groups.c \
Middlewares/Third_Party/FreeRTOS/Source/list.c \
Middlewares/Third_Party/FreeRTOS/Source/queue.c \
Middlewares/Third_Party/FreeRTOS/Source/stream_buffer.c \
Middlewares/Third_Party/FreeRTOS/Source/tasks.c \
Middlewares/Third_Party/FreeRTOS/Source/timers.c \
Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2/cmsis_os2.c \
Middlewares/Third_Party/FreeRTOS/Source/portable/MemMang/heap_4.c \
Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F/port.c

# User C sources
C_SOURCES += \
Middlewares/Third_Party/SEGGER/RTT/SEGGER_RTT_printf.c \
Middlewares/Third_Party/SEGGER/RTT/SEGGER_RTT.c

# User C++ sources
CPP_SOURCES = \
Core/warpper.cpp \
Core/BSP/bsp.cpp \
Core/BSP/interface/bsp_can.cpp \
Core/BSP/interface/bsp_gpio.cpp \
Core/BSP/interface/bsp_pwm.cpp \
Core/BSP/interface/bsp_spi.cpp \
Core/BSP/interface/bsp_uart.cpp \
Core/BSP/lib/bsp_delay.cpp \
Core/BSP/lib/bsp_log.cpp \
Core/BSP/lib/bsp_random.cpp \
Core/BSP/lib/bsp_reset.cpp \
Core/BSP/lib/bsp_time.cpp \
Core/BSP/lib/bsp_math.cpp \
Core/HAL/hal.cpp \
Core/HAL/lib/common.cpp \
Core/HAL/lib/soft_crc.cpp \
Core/HAL/lib/datatypes.cpp \
Core/HAL/lib/MadgwickAHRS.cpp \
Core/HAL/lib/MahonyAHRS.cpp \
Core/HAL/lib/pid.cpp \
Core/HAL/monitor/monitor.cpp \
Core/HAL/driver/buzzer/buzzer.cpp \
Core/HAL/driver/com/can_pc.cpp \
Core/HAL/driver/imu/imu_data.cpp \
Core/HAL/driver/imu/bmi088.cpp \
Core/HAL/driver/imu/bmi088_tools.cpp \
Core/HAL/driver/remote/DT7_DR16.cpp \
Core/HAL/driver/super_cap/super_cap_wuli.cpp \
Core/HAL/driver/ws2812b/ws2812b.cpp \
Core/HAL/driver/eio/spi_eio.cpp \
Core/HAL/driver/motor/can_motor.cpp \
Core/APP/cmd/gimbal_board_cmd.cpp \
Core/APP/cmd/chassis_board_cmd.cpp \
Core/APP/gimbal/gimbal.cpp \
Core/APP/shoot/shoot.cpp \
Core/APP/chassis/chassis.cpp \
Core/APP/app.cpp \

# ASM sources
ASM_SOURCES =  \
startup_stm32f407xx.s


#User ASM sources
ASM_SOURCES += \
Middlewares/Third_Party/SEGGER/RTT/SEGGER_RTT_ASM_ARMv7M.s
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


# Add g++ compiler
ifdef GCC_PATH
CCP = $(GCC_PATH)/$(PREFIX)g++
else
CCP = $(PREFIX)g++
endif

#######################################
# CFLAGS
#######################################
# cpu
CPU = -mcpu=cortex-m4

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
-DSTM32F407xx


# AS includes
AS_INCLUDES =  \
-ICore\Inc

# C includes
C_INCLUDES =  \
-ICore/Inc \
-IDrivers/STM32F4xx_HAL_Driver/Inc \
-IDrivers/STM32F4xx_HAL_Driver/Inc/Legacy \
-IMiddlewares/Third_Party/FreeRTOS/Source/include \
-IMiddlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 \
-IMiddlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F \
-IDrivers/CMSIS/Device/ST/STM32F4xx/Include \
-IDrivers/CMSIS/Include \
-IMiddlewares/Third_Party/ARM_CMSIS/CMSIS/DSP/PrivateInclude/ \
-IMiddlewares/Third_Party/ARM_CMSIS/CMSIS/DSP/Include/ \
-IMiddlewares/Third_Party/ARM_CMSIS/CMSIS/DSP/Include

# User C includes
C_INCLUDES += \
-ICore \
-ICore/HAL/driver/imu \
-ICore/HAL/driver/buzzer \
-ICore/HAL/driver/com \
-ICore/HAL/driver/motor \
-ICore/HAL/driver/remote \
-ICore/HAL/driver/referee \
-ICore/HAL/driver/ws2812b \
-ICore/HAL/driver/radiotube \
-ICore/HAL/driver/super_cap \
-ICore/HAL/lib \
-ICore/HAL/monitor \
-ICore/HAL \
-ICore/APP \
-ICore/APP/cmd \
-ICore/APP/shoot \
-ICore/APP/gimbal \
-ICore/APP/chassis \
-ICore/BSP \
-ICore/BSP/lib \
-ICore/BSP/interface \
-IMiddlewares/Third_Party/SEGGER/RTT/ \
-IMiddlewares/Third_Party/SEGGER/Config \
-IMiddlewares/Third_Party/ARM_CMSIS/CMSIS/DSP/PrivateInclude/ \
-IMiddlewares/Third_Party/ARM_CMSIS/CMSIS/DSP/Include/ \
-IMiddlewares/Third_Party/ARM_CMSIS/CMSIS/DSP/Include

# compile gcc flags
ASFLAGS = $(MCU) $(AS_DEFS) $(AS_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections

CFLAGS = $(MCU) $(C_DEFS) $(C_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections

ifeq ($(DEBUG), 1)
CFLAGS += -g -gdwarf-2
endif


# Generate dependency information
CFLAGS += -MMD -MP -MF"$(@:%.o=%.d)"


#######################################
# LDFLAGS
#######################################
# link script
LDSCRIPT = STM32F407IGHx_FLASH.ld

# libraries
LIBS = -lc -lm -lnosys 
LIBDIR = 
LDFLAGS = $(MCU) -lstdc++ -specs=nano.specs -T$(LDSCRIPT) $(LIBDIR) $(LIBS) -Wl,-Map=$(BUILD_DIR)/$(TARGET).map,--cref -Wl,--gc-sections

# default action: build all
all: $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/$(TARGET).hex $(BUILD_DIR)/$(TARGET).bin


#######################################
# build the application
#######################################
# list of objects
OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
vpath %.c $(sort $(dir $(C_SOURCES)))
OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(CPP_SOURCES:.cpp=.o)))
vpath %.cpp $(sort $(dir $(CPP_SOURCES)))
# list of ASM program objects
OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(ASM_SOURCES:.s=.o)))
vpath %.s $(sort $(dir $(ASM_SOURCES)))

$(BUILD_DIR)/%.o: %.c Makefile | $(BUILD_DIR) 
	$(CC) -c $(CFLAGS) -Wa,-a,-ad,-alms=$(BUILD_DIR)/$(notdir $(<:.c=.lst)) $< -o $@

$(BUILD_DIR)/%.o: %.cpp Makefile | $(BUILD_DIR)
	$(CCP) -c $(CFLAGS) -std=c++17 -Wa,-a,-ad,-alms=$(BUILD_DIR)/$(notdir $(<:.cpp=.lst)) $< -o $@

$(BUILD_DIR)/%.o: %.s Makefile | $(BUILD_DIR)
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
