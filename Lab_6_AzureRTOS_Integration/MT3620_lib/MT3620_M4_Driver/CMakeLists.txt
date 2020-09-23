CMAKE_MINIMUM_REQUIRED(VERSION 3.11)

PROJECT(MT3620_M4_Driver C)

ADD_LIBRARY(MT3620_M4_Driver
            ../MT3620_M4_BSP/printf/printf.c
            ./MHAL/src/mhal_osai.c
            ./MHAL/src/mhal_adc.c ./HDL/src/hdl_adc.c
            ./MHAL/src/mhal_dma.c ./HDL/src/hdl_dma.c
            ./MHAL/src/mhal_eint.c ./HDL/src/hdl_eint.c
            ./MHAL/src/mhal_gpio.c ./HDL/src/hdl_gpio.c
            ./MHAL/src/mhal_gpioif.c ./HDL/src/hdl_gpioif.c
            ./MHAL/src/mhal_gpt.c ./HDL/src/hdl_gpt.c
            ./MHAL/src/mhal_i2c.c ./HDL/src/hdl_i2c.c
            ./MHAL/src/mhal_i2s.c ./HDL/src/hdl_i2s.c
            ./MHAL/src/mhal_pwm.c ./HDL/src/hdl_pwm.c
            ./MHAL/src/mhal_spim.c ./HDL/src/hdl_spim.c
            ./MHAL/src/mhal_uart.c ./HDL/src/hdl_uart.c
            ./MHAL/src/mhal_wdt.c ./HDL/src/hdl_wdt.c
            ./MHAL/src/mhal_mbox.c ./HDL/src/hdl_mbox.c)

TARGET_INCLUDE_DIRECTORIES(MT3620_M4_Driver PUBLIC
                           ../MT3620_M4_BSP/printf
                           ./MHAL/inc
                           ./HDL/inc)

ADD_SUBDIRECTORY(../MT3620_M4_BSP ./lib/MT3620_M4_BSP)
TARGET_LINK_LIBRARIES(MT3620_M4_Driver PUBLIC MT3620_M4_BSP)


if(DEFINED OSAI_FREERTOS)
ADD_SUBDIRECTORY(../MT3620_M4_BSP/FreeRTOS ./MT3620_M4_BSP/FreeRTOS)
TARGET_LINK_LIBRARIES(MT3620_M4_Driver PUBLIC MT3620_M4_FreeRTOS)
endif()
TARGET_INCLUDE_DIRECTORIES(MT3620_M4_Driver PUBLIC
                           ../OS_HAL/inc)
