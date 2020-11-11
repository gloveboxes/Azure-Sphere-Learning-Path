#  Copyright (c) Microsoft Corporation. All rights reserved.
#  Licensed under the MIT License.

###################################################################################################################

# Select your developer board by removing the # tag from the beginning of the line

# The default board selected is the AVNET Starter Kit.

# If you are NOT using the AVNET board be sure to comment out the AVNET board

# set(AVNET TRUE "AVNET Azure Sphere Starter Kit")         
set(SEEED_STUDIO_MINI TRUE "Seeed Studio Azure Sphere MT3620 Mini Dev Board")


###################################################################################################################

cmake_minimum_required (VERSION 3.10)
project (AzureSphereIoTCentral C)

azsphere_configure_tools(TOOLS_REVISION "20.10")
azsphere_configure_api(TARGET_API_SET "7")


add_subdirectory("../../../LearningPathLibrary" out)
<<<<<<< HEAD:environment-monitor/CMakeLists.txt
add_subdirectory("embedded" sensirion)
=======
add_subdirectory("./embedded-scd" scd30)
>>>>>>> 63887c89cc4368faca2c850a3b585d55e86a465a:co2_monitor_hl/CMakeLists.txt

set(Source
    "main.c"
)
source_group("Source" FILES ${Source})

if(AVNET)

    add_definitions( -DOEM_AVNET=TRUE )

endif(AVNET)

if(SEEED_STUDIO_RDB OR SEEED_STUDIO_MINI)

    add_definitions( -DOEM_SEEED_STUDIO=TRUE )

endif(SEEED_STUDIO_RDB OR SEEED_STUDIO_MINI)

set(ALL_FILES
    ${Source}
)

# Create executable
add_executable(${PROJECT_NAME} ${ALL_FILES})
target_compile_definitions(${PROJECT_NAME} PUBLIC AZURE_IOT_HUB_CONFIGURED)
target_link_libraries(${PROJECT_NAME} applibs pthread gcc_s c azsphere_libs Sensirion_lib)

target_include_directories(${PROJECT_NAME} PUBLIC
                           ../../../LearningPathLibrary
                           ../../../IntercoreContract
                          )


if(AVNET)
    azsphere_target_hardware_definition(${PROJECT_NAME} TARGET_DIRECTORY "./Hardware/avnet_mt3620_sk" TARGET_DEFINITION "azure_sphere_learning_path.json")
endif(AVNET)

if (SEEED_STUDIO_RDB)
    azsphere_target_hardware_definition(${PROJECT_NAME} TARGET_DIRECTORY "./Hardware/mt3620_rdb" TARGET_DEFINITION "azure_sphere_learning_path.json")
endif()

if (SEEED_STUDIO_MINI)
    azsphere_target_hardware_definition(${PROJECT_NAME} TARGET_DIRECTORY "./Hardware/seeed_mt3620_mdb" TARGET_DEFINITION "azure_sphere_learning_path.json")
endif(SEEED_STUDIO_MINI)

azsphere_target_add_image_package(${PROJECT_NAME})