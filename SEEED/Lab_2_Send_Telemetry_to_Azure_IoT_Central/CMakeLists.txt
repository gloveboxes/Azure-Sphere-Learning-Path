#  Copyright (c) Microsoft Corporation. All rights reserved.
#  Licensed under the MIT License.

cmake_minimum_required (VERSION 3.10)
project (AzureSphereIoTCentral C)

azsphere_configure_tools(TOOLS_REVISION "20.04")
azsphere_configure_api(TARGET_API_SET "5+Beta2004")


add_subdirectory("../libs" out)

set(Source
    "main.c"
    "exit_codes.h"
    "../oem/board.c"
)
source_group("Source" FILES ${Source})


set(ALL_FILES
    ${Source}
)

# Create executable
add_executable(${PROJECT_NAME} ${ALL_FILES})
target_compile_definitions(${PROJECT_NAME} PUBLIC AZURE_IOT_HUB_CONFIGURED)
target_link_libraries(${PROJECT_NAME} applibs pthread gcc_s c azsphere_libs)

azsphere_target_hardware_definition(${PROJECT_NAME} TARGET_DIRECTORY "../oem/Hardware//mt3620_rdb" TARGET_DEFINITION "azure_sphere_learning_path.json")

azsphere_target_add_image_package(${PROJECT_NAME})