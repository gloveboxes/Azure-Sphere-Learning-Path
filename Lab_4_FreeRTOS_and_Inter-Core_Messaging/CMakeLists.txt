#  Copyright (c) Microsoft Corporation. All rights reserved.
#  Licensed under the MIT License.

CMAKE_MINIMUM_REQUIRED(VERSION 3.11)
PROJECT(Azure_Sphere_RTcore_FreeRTOS C)

# include
include_directories(${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/freertos/include ${CMAKE_SOURCE_DIR}/freertos/portable ${CMAKE_SOURCE_DIR}/printf)

# Create executable
ADD_EXECUTABLE(${PROJECT_NAME} main.c mt3620-intercore.c mt3620-uart-poll.c mt3620-gpio.c freertos/list.c freertos/tasks.c freertos/queue.c freertos/event_groups.c freertos/timers.c freertos/stream_buffer.c freertos/portable/heap_4.c freertos/portable/port.c printf/printf.c)
TARGET_LINK_LIBRARIES(${PROJECT_NAME})
SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES LINK_DEPENDS ${CMAKE_SOURCE_DIR}/linker.ld)

# Add MakeImage post-build command
INCLUDE("${AZURE_SPHERE_MAKE_IMAGE_FILE}")
