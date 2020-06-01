CMAKE_MINIMUM_REQUIRED(VERSION 3.11)

PROJECT(MT3620_M4_FreeRTOS C)

ADD_LIBRARY(MT3620_M4_FreeRTOS
            ./croutine.c
            ./event_groups.c
            ./list.c
            ./queue.c
            ./stream_buffer.c
            ./tasks.c
            ./timers.c
            ./portable/heap_4.c
            ./portable/port.c)

TARGET_INCLUDE_DIRECTORIES(MT3620_M4_FreeRTOS PUBLIC
                           ./portable
                           ./include)

