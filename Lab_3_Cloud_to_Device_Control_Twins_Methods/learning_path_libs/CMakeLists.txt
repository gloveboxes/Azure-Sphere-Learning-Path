CMAKE_MINIMUM_REQUIRED(VERSION 3.8)
PROJECT(azsphere_libs C)

################################################################################
# Source groups
################################################################################
set(Source
    "globals.c"
    "azure_iot.c"
    "peripheral_gpio.c"
    "device_twins.c"
    "direct_methods.c"
    "timer.c"
    "terminate.c"
    "eventloop_timer_utilities.c"
    "parson.c"
    "inter_core.c"
)
source_group("Source" FILES ${Source})

set(ALL_FILES
    ${Source}
)

################################################################################
# Target
################################################################################
add_library(${PROJECT_NAME} STATIC ${ALL_FILES})


set(ROOT_NAMESPACE azsphere_libs)
target_include_directories(${PROJECT_NAME} PUBLIC ${AZURE_SPHERE_API_SET_DIR}/usr/include/azureiot)
set_target_properties(${PROJECT_NAME} PROPERTIES
    VS_GLOBAL_KEYWORD "AzureSphere"
)

target_link_libraries (${PROJECT_NAME} applibs pthread gcc_s c azureiot)
