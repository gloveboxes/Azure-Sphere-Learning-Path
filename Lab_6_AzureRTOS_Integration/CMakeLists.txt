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

cmake_minimum_required (VERSION 3.10)
#set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")
project (demo_threadx C)

#azsphere_configure_tools(TOOLS_REVISION "20.07")
#azsphere_configure_api(TARGET_API_SET "6")

ADD_COMPILE_DEFINITIONS(OSAI_BARE_METAL)
ADD_LINK_OPTIONS(-specs=nano.specs -specs=nosys.specs)
# Create executable
add_executable (${PROJECT_NAME} 
                            ./demo_threadx/demo_azure_rtos.c 
                            ./demo_threadx/rtcoremain.c
                            ./demo_threadx/mt3620-intercore.c 
                            ./demo_threadx/mt3620-uart-poll.c 
                            ./demo_threadx/lsm6dso_reg.c 
                            ./demo_threadx/lsm6dso_driver.c 
                            ./demo_threadx/i2c.c
                            ./MT3620_lib/OS_HAL/src/os_hal_i2c.c
                            ./MT3620_lib/OS_HAL/src/os_hal_gpio.c
                            ./MT3620_lib/OS_HAL/src/os_hal_uart.c
                            "./MT3620_lib/OS_HAL/src/os_hal_dma.c"
)

include_directories(${PROJECT_NAME} PUBLIC
                    ./)

target_include_directories(${PROJECT_NAME} PUBLIC
                           ./MT3620_lib/OS_HAL/inc
                           ../IntercoreContract
                           ./)


add_subdirectory(./MT3620_lib/MT3620_M4_Driver ./lib/MT3620_M4_Driver)

set(THREADX_ARCH "cortex_m4")
set(THREADX_TOOLCHAIN "gnu")
add_subdirectory(tx)

target_link_libraries(${PROJECT_NAME} MT3620_M4_Driver MT3620_M4_BSP)
target_link_libraries(${PROJECT_NAME} tx)
target_link_libraries(${PROJECT_NAME} m)


set_target_properties (${PROJECT_NAME} PROPERTIES LINK_DEPENDS ${PROJECT_SOURCE_DIR}/linker.ld)

# Add MakeImage post-build command
# include ("${AZURE_SPHERE_MAKE_IMAGE_FILE}")

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

