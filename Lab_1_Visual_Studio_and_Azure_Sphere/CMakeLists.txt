#  Copyright (c) Microsoft Corporation. All rights reserved.
#  Licensed under the MIT License.

cmake_minimum_required (VERSION 3.8)
project (AzureSphereIoTCentral C)

add_subdirectory("../libs/MT3620_Grove_Shield/MT3620_Grove_Shield_Library" out)

set(Source
    "main.c"
    "../shared/globals.c"
    "../shared/azure_iot.c"
    "../shared/peripheral.c"
    "../shared/device_twins.c"
    "../shared/direct_methods.c"
    "../shared/timer.c"
    "../shared/terminate.c"
    "../shared/epoll_timerfd_utilities.c"
    "../shared/parson.c"
)
source_group("Source" FILES ${Source})

set(Avnet
    "../shared/avnet/lps22hh_reg.c"
    "../shared/avnet/lsm6dso_reg.c"
)
source_group("Avnet" FILES ${Avnet})

set(ALL_FILES
    ${Source}
    ${Avnet}
)

# Create executable
ADD_EXECUTABLE(${PROJECT_NAME} ${ALL_FILES})
TARGET_INCLUDE_DIRECTORIES(${PROJECT_NAME} PUBLIC ${AZURE_SPHERE_API_SET_DIR}/usr/include/azureiot)
TARGET_COMPILE_DEFINITIONS(${PROJECT_NAME} PUBLIC AZURE_IOT_HUB_CONFIGURED)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} applibs pthread gcc_s c MT3620_Grove_Shield_Library azureiot)


# Add MakeImage post-build command
include ("${AZURE_SPHERE_MAKE_IMAGE_FILE}")
