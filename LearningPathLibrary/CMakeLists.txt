CMAKE_MINIMUM_REQUIRED(VERSION 3.8)
PROJECT(azsphere_libs C)

################################################################################
# Source groups
################################################################################
set(Source
    "azure_iot.c"
    "device_twins.c"
    "direct_methods.c"
    "eventloop_timer_utilities.c"
    "globals.c"
    "inter_core.c"
    "parson.c"
    "peripheral_gpio.c"
    "terminate.c"
    "timer.c"
    "user_config.c"
    "utilities.c"
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
target_include_directories(${PROJECT_NAME} PUBLIC ${AZURE_SPHERE_API_SET_DIR}/usr/include/azureiot ../IntercoreContract)
set_target_properties(${PROJECT_NAME} PROPERTIES
    VS_GLOBAL_KEYWORD "AzureSphere"
)

target_link_libraries (${PROJECT_NAME} applibs pthread gcc_s c azureiot)
