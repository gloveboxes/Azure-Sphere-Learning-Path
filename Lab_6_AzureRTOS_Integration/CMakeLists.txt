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

azsphere_configure_tools(TOOLS_REVISION "20.04")
azsphere_configure_api(TARGET_API_SET "5+Beta2004")

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
)

target_include_directories (${PROJECT_NAME} PUBLIC "./tx")
TARGET_INCLUDE_DIRECTORIES(${PROJECT_NAME} PUBLIC
                           ./MT3620_lib/MT3620_M4_BSP/CMSIS/include
                           ./MT3620_lib/MT3620_M4_BSP/mt3620/inc
                           ./MT3620_lib/MT3620_M4_BSP/printf
                           ./MT3620_lib/MT3620_M4_Driver/MHAL/inc
                           ./MT3620_lib/MT3620_M4_Driver/HDL/inc
                           ./MT3620_lib/OS_HAL/inc)

target_link_libraries (${PROJECT_NAME} "${PROJECT_SOURCE_DIR}/out/tx/ARM-Debug/libtx.a")
target_link_libraries (${PROJECT_NAME} "${PROJECT_SOURCE_DIR}/out/mt3620_lib/ARM-Debug/libmt3620_lib.a")
target_link_libraries (${PROJECT_NAME} "${PROJECT_SOURCE_DIR}/out/mt3620_lib/ARM-Debug/lib/MT3620_M4_BSP/libMT3620_M4_BSP.a")
target_link_libraries (${PROJECT_NAME} "${PROJECT_SOURCE_DIR}/out/mt3620_lib/ARM-Debug/lib/MT3620_M4_Driver/libMT3620_M4_Driver.a")
target_link_libraries(${PROJECT_NAME} m)


add_subdirectory("${PROJECT_SOURCE_DIR}/tx" "${PROJECT_SOURCE_DIR}/out/tx/ARM-Debug/")
add_subdirectory("${PROJECT_SOURCE_DIR}/MT3620_lib" "${PROJECT_SOURCE_DIR}/out/mt3620_lib/ARM-Debug/")

set_target_properties (${PROJECT_NAME} PROPERTIES LINK_DEPENDS ${PROJECT_SOURCE_DIR}/linker.ld)

# Add MakeImage post-build command
# include ("${AZURE_SPHERE_MAKE_IMAGE_FILE}")

if(AVNET)
    azsphere_target_hardware_definition(${PROJECT_NAME} TARGET_DIRECTORY "../Hardware/avnet_mt3620_sk" TARGET_DEFINITION "azure_sphere_learning_path.json")
    add_definitions( -DOEM_AVNET=TRUE )
endif(AVNET)

if (SEEED_STUDIO_RDB)
    azsphere_target_hardware_definition(${PROJECT_NAME} TARGET_DIRECTORY "../Hardware/mt3620_rdb" TARGET_DEFINITION "azure_sphere_learning_path.json")
    add_definitions( -DOEM_SEEED_STUDIO=TRUE )
endif()

if (SEEED_STUDIO_MINI)
    azsphere_target_hardware_definition(${PROJECT_NAME} TARGET_DIRECTORY "../Hardware/seeed_mt3620_mdb" TARGET_DEFINITION "azure_sphere_learning_path.json")
    add_definitions( -DOEM_SEEED_STUDIO_MINI=TRUE )    
endif(SEEED_STUDIO_MINI)

azsphere_target_add_image_package(${PROJECT_NAME})

