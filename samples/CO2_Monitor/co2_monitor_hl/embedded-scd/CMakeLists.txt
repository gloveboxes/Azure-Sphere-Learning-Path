CMAKE_MINIMUM_REQUIRED(VERSION 3.8)
PROJECT(scd30_lib C)

include_directories(
    "./scd30" 
    "./embedded-common"
    "./scd-common"
    )

################################################################################
# Source groups
################################################################################
set(Source
    "./scd30/scd30.c"
    "./embedded-common/sensirion_common.c"
    "./embedded-common/hw_i2c/sensirion_hw_i2c_implementation.c"
    "./scd30/scd30.h"
)
source_group("Source" FILES ${Source})

set(ALL_FILES
    ${Source}
)

################################################################################
# Target
################################################################################
add_library(${PROJECT_NAME} STATIC ${ALL_FILES})


set(ROOT_NAMESPACE scd30_lib)
target_include_directories(${PROJECT_NAME} PUBLIC ${AZURE_SPHERE_API_SET_DIR}/usr/include/azureiot)
set_target_properties(${PROJECT_NAME} PROPERTIES
    VS_GLOBAL_KEYWORD "AzureSphere"
)

SET(GCC_COVERAGE_COMPILE_FLAGS "-Wall -fstrict-aliasing -Wstrict-aliasing=1 -Wsign-conversion -fPIC")

set_source_files_properties( ./embedded-common/sensirion_common.c PROPERTIES COMPILE_FLAGS -Wno-conversion)
add_definitions(${GCC_COVERAGE_COMPILE_FLAGS})

target_link_libraries (${PROJECT_NAME} applibs pthread gcc_s c azureiot)


if(AVNET)
    azsphere_target_hardware_definition(${PROJECT_NAME} TARGET_DIRECTORY "./Hardware/avnet_mt3620_sk" TARGET_DEFINITION "azure_sphere_learning_path.json")
endif(AVNET)

if (SEEED_STUDIO_RDB)
    azsphere_target_hardware_definition(${PROJECT_NAME} TARGET_DIRECTORY "./Hardware/mt3620_rdb" TARGET_DEFINITION "azure_sphere_learning_path.json")
endif()

if (SEEED_STUDIO_MINI)
    azsphere_target_hardware_definition(${PROJECT_NAME} TARGET_DIRECTORY "./Hardware/seeed_mt3620_mdb" TARGET_DEFINITION "azure_sphere_learning_path.json")
endif(SEEED_STUDIO_MINI)