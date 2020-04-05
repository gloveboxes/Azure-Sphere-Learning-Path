#  Copyright (c) Microsoft Corporation. All rights reserved.
#  Licensed under the MIT License.

cmake_minimum_required (VERSION 3.8)
project (AzureSphereIoTCentral C)

add_subdirectory("../libs" out)

set(Source
    "main.c"
    "exit_codes.h"
    "../oem/board.c"
)
source_group("Source" FILES ${Source})

set(Avnet
    "../oem/lps22hh_reg.c"
    "../oem/lsm6dso_reg.c"
    "../oem/imu_temp_pressure.c"
    "../oem/light_sensor.c"
)
source_group("Avnet" FILES ${Avnet})

set(ALL_FILES
    ${Source}
    ${Avnet}
)

# Create executable
add_executable(${PROJECT_NAME} ${ALL_FILES})
target_compile_definitions(${PROJECT_NAME} PUBLIC AZURE_IOT_HUB_CONFIGURED)
target_link_libraries(${PROJECT_NAME} applibs pthread gcc_s c azsphere_libs)

target_compile_options(${PROJECT_NAME} PRIVATE -Wno-unknown-pragmas)
set_source_files_properties(../oem/lsm6dso_reg.c PROPERTIES COMPILE_FLAGS -Wno-conversion)
set_source_files_properties(../oem/lps22hh_reg.c PROPERTIES COMPILE_FLAGS -Wno-conversion)

set_source_files_properties(../oem/imu_temp_pressure.c PROPERTIES COMPILE_FLAGS -Wno-float-conversion)
set_source_files_properties(../oem/imu_temp_pressure.c PROPERTIES COMPILE_FLAGS -Wno-incompatible-pointer-types)
set_source_files_properties(../oem/imu_temp_pressure.c PROPERTIES COMPILE_FLAGS -Wno-conversion)


# Add MakeImage post-build command
include ("${AZURE_SPHERE_MAKE_IMAGE_FILE}")
