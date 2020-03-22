CMAKE_MINIMUM_REQUIRED(VERSION 3.8)
PROJECT(azure_sphere_learning_path_library C)

################################################################################
# Source groups
################################################################################
set(Source
    "../shared/globals.c"
    "../shared/azure_iot.c"
    "../shared/peripheral.c"
    "../shared/device_twins.c"
    "../shared/direct_methods.c"
    "../shared/timer.c"
    "../shared/terminate.c"
    "../shared/eventloop_timer_utilities.c"
    "../shared/parson.c"
    "../shared/inter_core.c"
)
source_group("Source" FILES ${Source})

set(ALL_FILES
    ${Source}
)

################################################################################
# Target
################################################################################
add_library(${PROJECT_NAME} STATIC ${ALL_FILES})


set(ROOT_NAMESPACE azure_sphere_learning_path_library)
target_include_directories(${PROJECT_NAME} PUBLIC ${AZURE_SPHERE_API_SET_DIR}/usr/include/azureiot)
set_target_properties(${PROJECT_NAME} PROPERTIES
    VS_GLOBAL_KEYWORD "AzureSphere"
)

target_link_libraries (${PROJECT_NAME} applibs pthread gcc_s c azureiot)
