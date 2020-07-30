#  Copyright (c) Microsoft Corporation. All rights reserved.
#  Licensed under the MIT License.

cmake_minimum_required (VERSION 3.8)
project (mt3620_lib C ASM)

azsphere_configure_tools(TOOLS_REVISION "20.04")
#azsphere_configure_api(TARGET_API_SET "5+Beta2004")

ADD_COMPILE_DEFINITIONS(OSAI_BARE_METAL)
SET(CMAKE_ASM_FLAGS "-mcpu=cortex-m4")

# Create executable
add_library (${PROJECT_NAME} STATIC 
OS_HAL/src/os_hal_adc.c
OS_HAL/src/os_hal_dma.c
OS_HAL/src/os_hal_eint.c
OS_HAL/src/os_hal_gpio.c
OS_HAL/src/os_hal_gpioif.c
OS_HAL/src/os_hal_gpt.c
OS_HAL/src/os_hal_i2c.c
OS_HAL/src/os_hal_i2s.c
OS_HAL/src/os_hal_pwm.c
OS_HAL/src/os_hal_spim.c
OS_HAL/src/os_hal_uart.c
OS_HAL/src/os_hal_wdt.c
)
# Include Folders
TARGET_INCLUDE_DIRECTORIES(${PROJECT_NAME} PUBLIC OS_HAL/inc)
TARGET_INCLUDE_DIRECTORIES(${PROJECT_NAME} PUBLIC
                           MT3620_M4_BSP/CMSIS/include
                           MT3620_M4_BSP/mt3620/inc
                           MT3620_M4_BSP/printf
                           MT3620_M4_Driver/MHAL/inc
                           MT3620_M4_Driver/HDL/inc)

ADD_SUBDIRECTORY(./MT3620_M4_Driver ./lib/MT3620_M4_Driver)
ADD_SUBDIRECTORY(./MT3620_M4_BSP ./lib/MT3620_M4_BSP)