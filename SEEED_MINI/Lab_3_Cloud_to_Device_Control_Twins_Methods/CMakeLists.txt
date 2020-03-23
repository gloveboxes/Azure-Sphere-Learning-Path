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


set(ALL_FILES
    ${Source}
)

# Create executable
add_executable(${PROJECT_NAME} ${ALL_FILES})
target_compile_definitions(${PROJECT_NAME} PUBLIC AZURE_IOT_HUB_CONFIGURED)
target_link_libraries(${PROJECT_NAME} applibs pthread gcc_s c azsphere_libs)


# Add MakeImage post-build command
include ("${AZURE_SPHERE_MAKE_IMAGE_FILE}")
