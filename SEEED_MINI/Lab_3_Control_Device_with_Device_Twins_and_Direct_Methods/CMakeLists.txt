#  Copyright (c) Microsoft Corporation. All rights reserved.
#  Licensed under the MIT License.

cmake_minimum_required (VERSION 3.8)
project (AzureSphereIoTCentral C)


set(Source
    "main.c"
    "../shared/globals.c"
    "../shared/azure_iot.c"
    "../shared/peripheral.c"
    "../shared/device_twins.c"
    "../shared/direct_methods.c"
    "../shared/timer.c"
    "../shared/terminate.c"
    "../shared/eventloop_timer_utilities.c"
    "../shared/parson.c"
    "../shared/oem/board.c"
)
source_group("Source" FILES ${Source})


set(ALL_FILES
    ${Source}
)

# Create executable
add_executable(${PROJECT_NAME} ${ALL_FILES})
target_include_directories(${PROJECT_NAME} PUBLIC ${AZURE_SPHERE_API_SET_DIR}/usr/include/azureiot)
target_compile_definitions(${PROJECT_NAME} PUBLIC AZURE_IOT_HUB_CONFIGURED)
target_link_libraries(${PROJECT_NAME} applibs pthread gcc_s c azureiot)


# Add MakeImage post-build command
include ("${AZURE_SPHERE_MAKE_IMAGE_FILE}")
