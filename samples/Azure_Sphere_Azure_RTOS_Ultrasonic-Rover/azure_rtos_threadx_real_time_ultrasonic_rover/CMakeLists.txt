#  Copyright (c) Microsoft Corporation. All rights reserved.
#  Licensed under the MIT License.

cmake_minimum_required (VERSION 3.8)
#set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")
project (demo_threadx C)

azsphere_configure_tools(TOOLS_REVISION "20.07")
#azsphere_configure_api(TARGET_API_SET "6")

ADD_COMPILE_DEFINITIONS(OSAI_BARE_METAL)
ADD_LINK_OPTIONS(-specs=nano.specs -specs=nosys.specs)

# Create executable
add_executable (${PROJECT_NAME} 
	"./demo_threadx/demo_threadx.c"
	"./demo_threadx/rtcoremain.c"
	"./demo_threadx/mt3620-intercore.c"
    "./demo_threadx/mt3620-uart-poll.c"
	"./MT3620_lib/OS_HAL/src/os_hal_gpio.c"
	"./MT3620_lib/OS_HAL/src/os_hal_uart.c"
    "./MT3620_lib/OS_HAL/src/os_hal_dma.c"
	"./Ultrasonic/ultrasonic.c"
	 )

include_directories(${PROJECT_NAME} PUBLIC
                    ./)

target_include_directories(${PROJECT_NAME} PUBLIC
                           ./MT3620_lib/OS_HAL/inc
                           ./)

add_subdirectory(./MT3620_lib/MT3620_M4_Driver ./lib/MT3620_M4_Driver)
add_subdirectory(tx)

target_link_libraries(${PROJECT_NAME} MT3620_M4_Driver MT3620_M4_BSP)
target_link_libraries(${PROJECT_NAME} tx)

set_target_properties (${PROJECT_NAME} PROPERTIES LINK_DEPENDS ${PROJECT_SOURCE_DIR}/linker.ld)

# Add MakeImage post-build command
# include ("${AZURE_SPHERE_MAKE_IMAGE_FILE}")

azsphere_target_hardware_definition(${PROJECT_NAME} TARGET_DIRECTORY "../HardwareDefinitions/mt3620_rdb" TARGET_DEFINITION "azure_sphere_learning_path.json")

azsphere_target_add_image_package(${PROJECT_NAME})

