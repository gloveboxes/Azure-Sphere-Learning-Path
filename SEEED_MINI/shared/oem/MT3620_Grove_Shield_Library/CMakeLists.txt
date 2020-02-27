CMAKE_MINIMUM_REQUIRED(VERSION 3.8)
PROJECT(MT3620_Grove_Shield_Library C)

################################################################################
# Source groups
################################################################################
set(no_group_source_files
    "applibs_versions.h"
    "Grove.h"
    "mt3620_rdb.h"
)
source_group("" FILES ${no_group_source_files})

set(Common
    "Common/Delay.c"
    "Common/Delay.h"
)
source_group("Common" FILES ${Common})

set(HAL
    "HAL/GroveI2C.c"
    "HAL/GroveI2C.h"
    "HAL/GroveShield.c"
    "HAL/GroveShield.h"
    "HAL/GroveUART.c"
    "HAL/GroveUART.h"
)
source_group("HAL" FILES ${HAL})

set(Sensors
    "Sensors/Grove4DigitDisplay.c"
    "Sensors/Grove4DigitDisplay.h"
    "Sensors/GroveAD7992.c"
    "Sensors/GroveAD7992.h"
    "Sensors/GroveLEDButton.c"
    "Sensors/GroveLEDButton.h"
    "Sensors/GroveLightSensor.c"
    "Sensors/GroveLightSensor.h"
    "Sensors/GroveOledDisplay96x96.c"
    "Sensors/GroveOledDisplay96x96.h"
    "Sensors/GroveRelay.c"
    "Sensors/GroveRelay.h"
    "Sensors/GroveRotaryAngleSensor.c"
    "Sensors/GroveRotaryAngleSensor.h"
    "Sensors/GroveTempHumiBaroBME280.c"
    "Sensors/GroveTempHumiBaroBME280.h"
    "Sensors/GroveTempHumiSHT31.c"
    "Sensors/GroveTempHumiSHT31.h"
)
source_group("Sensors" FILES ${Sensors})

set(ALL_FILES
    ${no_group_source_files}
    ${Common}
    ${HAL}
    ${Sensors}
)

################################################################################
# Target
################################################################################
add_library(${PROJECT_NAME} STATIC ${ALL_FILES})


set(ROOT_NAMESPACE MT3620_Grove_Shield_Library)

set_target_properties(${PROJECT_NAME} PROPERTIES
    VS_GLOBAL_KEYWORD "AzureSphere"
)


target_link_libraries (${PROJECT_NAME} applibs pthread gcc_s c)

set_source_files_properties(Sensors/GroveOledDisplay96x96.c PROPERTIES COMPILE_FLAGS -Wno-conversion)



