# This code is based on a sample from Microsoft (see license below),
# with modifications made by MediaTek.
# Modified version of CMakeLists.txt from Microsoft Azure Sphere sample code:
# https://github.com/Azure/azure-sphere-samples/blob/master/Samples/HelloWorld/HelloWorld_RTApp_MT3620_BareMetal/CMakeLists.txt

#  Copyright (c) Microsoft Corporation. All rights reserved.
#  Licensed under the MIT License.

###################################################################################################################

# Select your developer board by removing the # tag from the beginning of the line

# The default board selected is the AVNET Starter Kit.

# If you are NOT using the AVNET board be sure to comment out the AVNET board

set(AVNET TRUE "AVNET Azure Sphere Starter Kit")                
# set(SEEED_STUDIO_RDB TRUE "Seeed Studio Azure Sphere MT3620 Development Kit (aka Reference Design Board or rdb)")
# set(SEEED_STUDIO_MINI TRUE "Seeed Studio Azure Sphere MT3620 Mini Dev Board")

###################################################################################################################

cmake_minimum_required(VERSION 3.10)

# Configurations
project(FreeRTOS_RTcore_GPIO C)
azsphere_configure_tools(TOOLS_REVISION "20.07")

azsphere_configure_api(TARGET_API_SET "6")

add_compile_definitions(OSAI_FREERTOS)
add_compile_definitions(OSAI_ENABLE_DMA)
add_link_options(-specs=nano.specs -specs=nosys.specs)

set(Source
    "main.c"
    "mt3620-intercore.c"
    "mt3620-uart-poll.c"
    "./OS_HAL/src/os_hal_gpio.c"
    "./OS_HAL/src/os_hal_uart.c"
    "./OS_HAL/src/os_hal_dma.c"
    "./OS_HAL/src/os_hal_i2c.c"
)
source_group("Source" FILES ${Source})


if(AVNET)

    set(Oem
        "lsm6dso_reg.c"
        "lsm6dso_driver.c" 
        "i2c.c"
    )
    source_group("Oem" FILES ${Oem})

endif(AVNET)

if(SEEED_STUDIO_RDB OR SEEED_STUDIO_MINI)

    set(Oem
    )
    source_group("Oem" FILES ${Oem})

endif(SEEED_STUDIO_RDB OR SEEED_STUDIO_MINI)

set(ALL_FILES
    ${Source}
    ${Oem}
)

# Executable
add_executable(${PROJECT_NAME} ${ALL_FILES})

# Include Folders
include_directories(${PROJECT_NAME} PUBLIC ./)
target_include_directories(${PROJECT_NAME} PUBLIC ./OS_HAL/inc ./)

# Libraries
set(OSAI_FREERTOS 1)
add_subdirectory(./MT3620_M4_Driver ./lib/MT3620_M4_Driver)
target_link_libraries(${PROJECT_NAME} MT3620_M4_Driver m)

# Linker, Image
set_target_properties(${PROJECT_NAME} PROPERTIES LINK_DEPENDS ${CMAKE_SOURCE_DIR}/linker.ld)

if(AVNET)
    azsphere_target_hardware_definition(${PROJECT_NAME} TARGET_DIRECTORY "../HardwareDefinitions/avnet_mt3620_sk" TARGET_DEFINITION "azure_sphere_learning_path.json")
    add_definitions( -DOEM_AVNET=TRUE )
endif(AVNET)

if (SEEED_STUDIO_RDB)
    azsphere_target_hardware_definition(${PROJECT_NAME} TARGET_DIRECTORY "../HardwareDefinitions/mt3620_rdb" TARGET_DEFINITION "azure_sphere_learning_path.json")
    add_definitions( -DOEM_SEEED_STUDIO=TRUE )
endif()

if (SEEED_STUDIO_MINI)
    azsphere_target_hardware_definition(${PROJECT_NAME} TARGET_DIRECTORY "../HardwareDefinitions/seeed_mt3620_mdb" TARGET_DEFINITION "azure_sphere_learning_path.json")
    add_definitions( -DOEM_SEEED_STUDIO_MINI=TRUE )    
endif(SEEED_STUDIO_MINI)

azsphere_target_add_image_package(${PROJECT_NAME})
