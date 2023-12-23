message(STATUS "Using CMake version ${CMAKE_VERSION}")
cmake_minimum_required(VERSION 3.10...${CMAKE_VERSION})

SET(InfiniTime_DIR "${CMAKE_SOURCE_DIR}/InfiniTime" CACHE PATH "Path to InfiniTime source path to use for simulator")

if(NOT EXISTS ${InfiniTime_DIR})
  message(FATAL_ERROR "Can't access folder '${InfiniTime_DIR}'. Try `git submodule update --init --recursive` to initialize the git submodule of InfiniTime")
endif()
if(NOT EXISTS ${InfiniTime_DIR}/src/libs/lvgl)
  message(FATAL_ERROR "Can't access folder '${InfiniTime_DIR}/src/libs/lvgl'. Try `git submodule update --init --recursive` to initialize the git submodule of lvgl inside InfiniTime")
endif()

# try to extract the PineTime project version
file(READ "${InfiniTime_DIR}/CMakeLists.txt" main_cmake)
string(REGEX MATCH "project\\(pinetime VERSION ([0-9]*\.[0-9]*\.[0-9]*)" _ ${main_cmake})
set(PROJECT_VERSION ${CMAKE_MATCH_1})
message(STATUS "InfiniTime PROJECT_VERSION extracted: ${PROJECT_VERSION}")

project(InfiniSim VERSION ${PROJECT_VERSION} LANGUAGES C CXX)
# https://cmake.org/cmake/help/latest/prop_tgt/CXX_STANDARD.html
string(COMPARE EQUAL "${CMAKE_CXX_STANDARD}" "" no_cmake_cxx_standard_set)
if(no_cmake_cxx_standard_set)
  set(CMAKE_CXX_STANDARD 20)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
  set(CMAKE_CXX_EXTENSIONS OFF)
  message(STATUS "Using default C++ standard ${CMAKE_CXX_STANDARD}")
else()
  message(STATUS "Using user specified C++ standard ${CMAKE_CXX_STANDARD}")
endif()
set(CMAKE_C_STANDARD 11)#C11

# copy lv_conf.h from InfiniTime project and do little modifications
file(READ "${InfiniTime_DIR}/src/libs/lv_conf.h" lv_conf_main)
# set '#define LV_TICK_CUSTOM     0'
string(REGEX REPLACE "#define[ ]+LV_TICK_CUSTOM[ ]+1" "#define LV_TICK_CUSTOM     0" lv_conf_main "${lv_conf_main}")
# allow more memory in debug mode for the simulator
#define LV_MEM_SIZE    (14U * 1024U)
string(REGEX REPLACE "#define[ ]+LV_MEM_SIZE[ ]+\\([^\)]*\\)"
  "#ifdef NDEBUG
#define LV_MEM_SIZE    (14U * 1024U)
#else // debug mode -> allow more memory
#define LV_MEM_SIZE    (28U * 1024U)
#endif"
  lv_conf_main "${lv_conf_main}")
# write to temporary file and overwrite file to use only if lv_conf changed to keep useless recompiles
# at a minimum
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/lv_conf_modified.h" "${lv_conf_main}")
configure_file(
  "${CMAKE_CURRENT_BINARY_DIR}/lv_conf_modified.h"
  "${CMAKE_CURRENT_BINARY_DIR}/lv_conf.h"
  COPYONLY)

file(GLOB_RECURSE LVGL_INCLUDES "lv_drivers/*.h" "${InfiniTime_DIR}/src/libs/lvgl/src/*.h"  "./*.h" )
file(GLOB_RECURSE LVGL_SOURCES  "lv_drivers/*.c" "${InfiniTime_DIR}/src/libs/lvgl/src/*.c" )

add_library(sim-base STATIC
  # LVGL sources
  ${LVGL_SOURCES} ${LVGL_INCLUDES}
  # FreeRTOS
  sim/FreeRTOS.h
  sim/FreeRTOS.cpp
  sim/task.h
  sim/task.cpp
  sim/timers.h
  sim/timers.cpp
  sim/queue.h
  sim/queue.cpp
  # src/FreeRTOS
  sim/portmacro_cmsis.h
  sim/portmacro_cmsis.cpp
  # nrf
  sim/libraries/log/nrf_log.h
  sim/libraries/delay/nrf_delay.h
  sim/libraries/delay/nrf_delay.cpp
  sim/nrfx/nrfx_log.h
  sim/nrfx/drivers/include/nrfx_twi.h
  sim/nrfx/hal/nrf_gpio.h
  sim/nrfx/hal/nrf_gpio.cpp
  sim/nrfx/hal/nrfx_gpiote.h # includes hal/nrf_gpio.h
  sim/nrfx/hal/nrf_rtc.h
  sim/nrfx/hal/nrf_rtc.cpp
  sim/nrfx/mdk/nrf.h
  sim/nrfx/mdk/nrf52.h
  sim/nrfx/mdk/nrf52.cpp
  sim/nrfx/mdk/nrf52_bitfields.h
  # nrf/components/libraries/timer
  sim/libraries/gpiote/app_gpiote.h # includes hal/nrf_gpio.h
)
# include the generated lv_conf.h file before anything else
target_include_directories(sim-base PUBLIC "${CMAKE_CURRENT_BINARY_DIR}") # lv_conf.h
target_include_directories(sim-base PUBLIC "sim")
target_include_directories(sim-base PUBLIC "sim/libraries/log") # for nrf_log.h
target_include_directories(sim-base PUBLIC "sim/nrfx") # for nrfx_log.h and others
target_include_directories(sim-base PUBLIC "sim/nrfx/hal") # for nrfx_log.h
target_include_directories(sim-base PUBLIC "sim/nrfx/mdk") # for nrf52_bitfields.h

target_include_directories(sim-base PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}") # lv_drv_conf.h
target_include_directories(sim-base PUBLIC "${InfiniTime_DIR}/src/libs")
target_include_directories(sim-base PUBLIC "lv_drivers")

target_include_directories(sim-base PUBLIC "${InfiniTime_DIR}/src") # InfiniTime drivers, components and all


set(MONITOR_ZOOM 1 CACHE STRING "Scale simulator window by this factor")
if(MONITOR_ZOOM MATCHES "^[0-9]\.?[0-9]*")
  message(STATUS "Using MONITOR_ZOOM=${MONITOR_ZOOM}")
  target_compile_definitions(sim-base PUBLIC MONITOR_ZOOM=${MONITOR_ZOOM})
else()
  message(FATAL_ERROR "variable MONITOR_ZOOM=${MONITOR_ZOOM} must be a positive number")
endif()

add_executable(infinisim main.cpp)
# add simulator files
target_sources(infinisim PUBLIC
  sim/displayapp/LittleVgl.h
  sim/displayapp/LittleVgl.cpp
  sim/components/battery/BatteryController.h
  sim/components/battery/BatteryController.cpp
  sim/components/ble/AlertNotificationService.h
  sim/components/ble/AlertNotificationService.cpp
  sim/components/ble/MusicService.h
  sim/components/ble/MusicService.cpp
  sim/components/ble/NavigationService.h
  sim/components/ble/NavigationService.cpp
  sim/components/ble/NimbleController.h
  sim/components/ble/NimbleController.cpp
  sim/components/ble/SimpleWeatherService.h
  sim/components/ble/SimpleWeatherService.cpp
  sim/components/brightness/BrightnessController.h
  sim/components/brightness/BrightnessController.cpp
  sim/components/firmwarevalidator/FirmwareValidator.h
  sim/components/firmwarevalidator/FirmwareValidator.cpp
  sim/components/heartrate/HeartRateController.h
  sim/components/heartrate/HeartRateController.cpp
  sim/components/motion/MotionController.h
  sim/components/motion/MotionController.cpp
  sim/drivers/Bma421.h
  sim/drivers/Bma421.cpp
  sim/drivers/Cst816s.h
  sim/drivers/Cst816s.cpp
  sim/drivers/SpiMaster.h
  sim/drivers/SpiMaster.cpp
  sim/drivers/TwiMaster.h
  sim/drivers/TwiMaster.cpp
  sim/drivers/SpiNorFlash.h
  sim/drivers/SpiNorFlash.cpp
  sim/heartratetask/HeartRateTask.h
  sim/heartratetask/HeartRateTask.cpp
)
target_link_libraries(infinisim PUBLIC sim-base)

# add Screens, fonts and icons with a globbing expression,
# to enable easier CI test-runs for PRs adding new Screens/Fonts/Icons
file(GLOB InfiniTime_SCREENS
  "${InfiniTime_DIR}/src/displayapp/screens/*.h"
  "${InfiniTime_DIR}/src/displayapp/screens/*.cpp"
  "${InfiniTime_DIR}/src/displayapp/screens/settings/*.h"
  "${InfiniTime_DIR}/src/displayapp/screens/settings/*.cpp"
)
file(GLOB InfiniTime_FONTS
  "${InfiniTime_DIR}/src/displayapp/fonts/*.c"
)
file(GLOB InfiniTime_ICONS
  "${InfiniTime_DIR}/src/displayapp/icons/*.c"
)
file(GLOB InfiniTime_WIDGETS
  "${InfiniTime_DIR}/src/displayapp/widgets/*.cpp"
  "${InfiniTime_DIR}/src/displayapp/widgets/*.h"
)
target_sources(infinisim PUBLIC ${InfiniTime_SCREENS})
target_sources(infinisim PUBLIC ${InfiniTime_FONTS})
target_sources(infinisim PUBLIC ${InfiniTime_ICONS})
target_sources(infinisim PUBLIC ${InfiniTime_WIDGETS})

# add files directly from InfiniTime sources
target_sources(infinisim PUBLIC
  ${InfiniTime_DIR}/src/BootloaderVersion.h
  ${InfiniTime_DIR}/src/BootloaderVersion.cpp
  ${InfiniTime_DIR}/src/displayapp/Colors.h
  ${InfiniTime_DIR}/src/displayapp/Colors.cpp
  ${InfiniTime_DIR}/src/displayapp/DisplayApp.h
  ${InfiniTime_DIR}/src/displayapp/DisplayApp.cpp
  ${InfiniTime_DIR}/src/buttonhandler/ButtonHandler.h
  ${InfiniTime_DIR}/src/buttonhandler/ButtonHandler.cpp
  ${InfiniTime_DIR}/src/components/alarm/AlarmController.h
  ${InfiniTime_DIR}/src/components/alarm/AlarmController.cpp
  ${InfiniTime_DIR}/src/components/ble/BleController.h
  ${InfiniTime_DIR}/src/components/ble/BleController.cpp
  ${InfiniTime_DIR}/src/components/datetime/DateTimeController.h
  ${InfiniTime_DIR}/src/components/datetime/DateTimeController.cpp
  ${InfiniTime_DIR}/src/components/settings/Settings.h
  ${InfiniTime_DIR}/src/components/settings/Settings.cpp
  ${InfiniTime_DIR}/src/components/ble/NotificationManager.h
  ${InfiniTime_DIR}/src/components/ble/NotificationManager.cpp
  ${InfiniTime_DIR}/src/components/fs/FS.h
  ${InfiniTime_DIR}/src/components/fs/FS.cpp
  ${InfiniTime_DIR}/src/components/motor/MotorController.h
  ${InfiniTime_DIR}/src/components/motor/MotorController.cpp
  ${InfiniTime_DIR}/src/drivers/Hrs3300.h
  ${InfiniTime_DIR}/src/drivers/Hrs3300.cpp
  ${InfiniTime_DIR}/src/drivers/PinMap.h
  ${InfiniTime_DIR}/src/drivers/Spi.h
  ${InfiniTime_DIR}/src/drivers/Spi.cpp
  ${InfiniTime_DIR}/src/drivers/St7789.h
  ${InfiniTime_DIR}/src/drivers/St7789.cpp
  ${InfiniTime_DIR}/src/drivers/Watchdog.h
  ${InfiniTime_DIR}/src/drivers/Watchdog.cpp
  ${InfiniTime_DIR}/src/touchhandler/TouchHandler.h
  ${InfiniTime_DIR}/src/touchhandler/TouchHandler.cpp
  ${InfiniTime_DIR}/src/systemtask/SystemTask.h
  ${InfiniTime_DIR}/src/systemtask/SystemTask.cpp
  ${InfiniTime_DIR}/src/systemtask/SystemMonitor.h
  ${InfiniTime_DIR}/src/systemtask/SystemMonitor.cpp
  ${InfiniTime_DIR}/src/utility/Math.h
  ${InfiniTime_DIR}/src/utility/Math.cpp
)

target_sources(infinisim PUBLIC
  ${InfiniTime_DIR}/src/displayapp/InfiniTimeTheme.cpp
  ${InfiniTime_DIR}/src/displayapp/InfiniTimeTheme.h
)

if(EXISTS ${InfiniTime_DIR}/src/components/timer/Timer.h)
  target_sources(infinisim PUBLIC
    ${InfiniTime_DIR}/src/components/timer/Timer.h
    ${InfiniTime_DIR}/src/components/timer/Timer.cpp
  )
else()
  target_compile_definitions(infinisim PUBLIC INFINITIME_TIMERCONTROLLER)
  target_sources(infinisim PUBLIC
    ${InfiniTime_DIR}/src/components/timer/TimerController.h
    ${InfiniTime_DIR}/src/components/timer/TimerController.cpp
  )
endif()

# littlefs
add_library(littlefs STATIC
  ${InfiniTime_DIR}/src/libs/littlefs/lfs_util.h
  ${InfiniTime_DIR}/src/libs/littlefs/lfs.h
  ${InfiniTime_DIR}/src/libs/littlefs/lfs_util.c
  ${InfiniTime_DIR}/src/libs/littlefs/lfs.c
)
target_include_directories(littlefs PUBLIC "${InfiniTime_DIR}/src/libs/littlefs")
target_link_libraries(infinisim PRIVATE littlefs)

# QCBOR
add_library(QCBOR STATIC
  ${InfiniTime_DIR}/src/libs/QCBOR/src/ieee754.c
  ${InfiniTime_DIR}/src/libs/QCBOR/src/qcbor_decode.c
  ${InfiniTime_DIR}/src/libs/QCBOR/src/qcbor_encode.c
  ${InfiniTime_DIR}/src/libs/QCBOR/src/qcbor_err_to_str.c
  ${InfiniTime_DIR}/src/libs/QCBOR/src/UsefulBuf.c)
target_include_directories(QCBOR SYSTEM PUBLIC ${InfiniTime_DIR}/src/libs/QCBOR/inc)
# This is required with the current configuration
target_compile_definitions(QCBOR PUBLIC QCBOR_DISABLE_FLOAT_HW_USE)
# These are for space-saving
target_compile_definitions(QCBOR PUBLIC QCBOR_DISABLE_PREFERRED_FLOAT)
target_compile_definitions(QCBOR PUBLIC QCBOR_DISABLE_EXP_AND_MANTISSA)
target_compile_definitions(QCBOR PUBLIC QCBOR_DISABLE_INDEFINITE_LENGTH_STRINGS)
#target_compile_definitions(QCBOR PUBLIC QCBOR_DISABLE_INDEFINITE_LENGTH_ARRAYS)
target_compile_definitions(QCBOR PUBLIC QCBOR_DISABLE_UNCOMMON_TAGS)
target_compile_definitions(QCBOR PUBLIC USEFULBUF_CONFIG_LITTLE_ENDIAN)
set_target_properties(QCBOR PROPERTIES LINKER_LANGUAGE C)
#target_compile_options(QCBOR PRIVATE
#        $<$<AND:$<COMPILE_LANGUAGE:C>,$<CONFIG:DEBUG>>: ${COMMON_FLAGS} -O0 -g3>
#        $<$<AND:$<COMPILE_LANGUAGE:C>,$<CONFIG:RELEASE>>: ${COMMON_FLAGS} -O3>
#        $<$<COMPILE_LANGUAGE:ASM>: -MP -MD -x assembler-with-cpp>
#        )
target_link_libraries(infinisim PRIVATE QCBOR)

# check version number of installed node package for minimum required
find_program(NODE_EXE "node" NO_CACHE QUIET)
if(NODE_EXE)
  execute_process(COMMAND "${NODE_EXE}" "--version" OUTPUT_VARIABLE NODE_VERSION)
  string(REPLACE "v" "" NODE_VERSION "${NODE_VERSION}")
  if(NODE_VERSION VERSION_LESS 14.0.0)
    message(WARNING "Node version v${NODE_VERSION} is less than required 14+, you will probably encounter build errors")
  else()
    message(STATUS "Founde node version v${NODE_VERSION}")
  endif()
else()
  message(STATUS "Didn't find executable 'node' in path to check its version number")
endif()

if(EXISTS ${InfiniTime_DIR}/src/displayapp/fonts/CMakeLists.txt)
  # available since https://github.com/InfiniTimeOrg/InfiniTime/pull/1097
  message(STATUS "add subdirectory ${InfiniTime_DIR}/src/displayapp/fonts for 'infinitime_fonts' target")
  add_subdirectory(${InfiniTime_DIR}/src/displayapp/fonts fonts)
  target_link_libraries(infinisim PRIVATE infinitime_fonts)
endif()

if(EXISTS ${InfiniTime_DIR}/src/displayapp/apps/CMakeLists.txt)
  # available since https://github.com/InfiniTimeOrg/InfiniTime/pull/1928
  message(STATUS "add subdirectory ${InfiniTime_DIR}/src/displayapp/apps for 'infinitime_apps' target")
  add_subdirectory(${InfiniTime_DIR}/src/displayapp/apps displayapp/apps)
  target_link_libraries(infinisim PRIVATE infinitime_apps)
endif()

option(BUILD_RESOURCES "Generate a resource.zip file to install to spi.raw file" ON)
if(BUILD_RESOURCES)
  if(EXISTS ${InfiniTime_DIR}/src/resources/CMakeLists.txt)
    # set verison variables as used in resources/CMakeLists.txt
    set(pinetime_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
    set(pinetime_VERSION_MINOR ${PROJECT_VERSION_MINOR})
    set(pinetime_VERSION_PATCH ${PROJECT_VERSION_PATCH})
    # available since https://github.com/InfiniTimeOrg/InfiniTime/pull/1097
    message(STATUS "add subdirectory ${InfiniTime_DIR}/src/resources for 'GenerateResources' target")
    add_subdirectory(${InfiniTime_DIR}/src/resources resources)
    add_dependencies(infinisim GenerateResources)
  else()
    message(FATAL_ERROR "BUILD_RESOURCES is enabled, but InfiniTime subdirectory has no resources/CMakeLists.txt file")
  endif()
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
# Special case for SDL2 dependency, goal is to find a config that exports SDL2::SDL2 target
# libsdl2-dev has a `sdl2-config.cmake` that doesn't export this, but vcpkg does..
find_package(SDL2 CONFIG QUIET)
if(NOT TARGET SDL2::SDL2)
  find_package(SDL2 MODULE REQUIRED)
endif()
target_link_libraries(infinisim PRIVATE SDL2::SDL2)

# Some toolchains (e.g. g++-8) require to explicitly link with the standard filesystem library
# See https://github.com/InfiniTimeOrg/InfiniSim/issues/57#issuecomment-1386889378
find_package(Filesystem REQUIRED)
target_link_libraries(infinisim PRIVATE std::filesystem)

# Get the latest abbreviated commit hash of the working branch
execute_process(
  COMMAND ${GIT_EXECUTABLE} log -1 --format=%h
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  OUTPUT_VARIABLE PROJECT_GIT_COMMIT_HASH
  OUTPUT_STRIP_TRAILING_WHITESPACE
)
set(VERSION_EDIT_WARNING "// Do not edit this file, it is automatically generated by CMAKE!")
configure_file("${InfiniTime_DIR}/src/Version.h.in" "${CMAKE_CURRENT_BINARY_DIR}/Version.h")

option(WITH_PNG "Compile with libpng support to dump current screen as png" ON)
if(WITH_PNG)
  find_package(PNG REQUIRED)
  target_compile_definitions(infinisim PRIVATE WITH_PNG)
  target_link_libraries(infinisim PRIVATE PNG::PNG)
endif()

target_include_directories(infinisim PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/gif-h")

# embedd background image for the status window to use
add_subdirectory(img)
add_dependencies(infinisim infinisim_img_background)

install(TARGETS infinisim DESTINATION bin)

# helper library to manipulate littlefs raw image
add_executable(littlefs-do
  littlefs-do-main.cpp
  # want to use FS.h
  ${InfiniTime_DIR}/src/components/fs/FS.h
  ${InfiniTime_DIR}/src/components/fs/FS.cpp
  # dependencies for FS.h
  sim/drivers/SpiNorFlash.h
  sim/drivers/SpiNorFlash.cpp
  # dependencies for SpiNorFlash.h
  ${InfiniTime_DIR}/src/drivers/Spi.h
  ${InfiniTime_DIR}/src/drivers/Spi.cpp
  sim/drivers/SpiMaster.h
  sim/drivers/SpiMaster.cpp

  ${InfiniTime_DIR}/src/components/settings/Settings.h
  ${InfiniTime_DIR}/src/components/settings/Settings.cpp
)

target_link_libraries(littlefs-do PUBLIC sim-base)
target_link_libraries(littlefs-do PUBLIC littlefs)

target_link_libraries(littlefs-do PRIVATE SDL2::SDL2)
target_link_libraries(littlefs-do PRIVATE infinitime_fonts)

add_subdirectory(external/miniz)
add_subdirectory(external/nlohmann_json)
target_link_libraries(littlefs-do PRIVATE miniz)
target_link_libraries(littlefs-do PRIVATE nlohmann_json::nlohmann_json)
target_link_libraries(littlefs-do PRIVATE std::filesystem)
