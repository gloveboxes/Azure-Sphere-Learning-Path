#  Copyright (c) Microsoft Corporation. All rights reserved.
#  Licensed under the MIT License.

cmake_minimum_required (VERSION 3.8)
project (AzureSphereIoTCentral C)

add_subdirectory("../MT3620_Grove_Shield/MT3620_Grove_Shield_Library" out)

# Create executable
ADD_EXECUTABLE(${PROJECT_NAME} main.c globals.c inter_core.c azure_iot.c epoll_timerfd_utilities.c parson.c)
TARGET_INCLUDE_DIRECTORIES(${PROJECT_NAME} PUBLIC ${AZURE_SPHERE_API_SET_DIR}/usr/include/azureiot)
TARGET_COMPILE_DEFINITIONS(${PROJECT_NAME} PUBLIC AZURE_IOT_HUB_CONFIGURED)
TARGET_LINK_LIBRARIES(${PROJECT_NAME} applibs pthread gcc_s c MT3620_Grove_Shield_Library azureiot)


# Add MakeImage post-build command
include ("${AZURE_SPHERE_MAKE_IMAGE_FILE}")
