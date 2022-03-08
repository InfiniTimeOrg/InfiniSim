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
  set(CMAKE_CXX_STANDARD 17)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
  set(CMAKE_CXX_EXTENSIONS OFF)
  message(STATUS "Using default C++ standard ${CMAKE_CXX_STANDARD}")
else()
  message(STATUS "Using user specified C++ standard ${CMAKE_CXX_STANDARD}")
endif()
set(CMAKE_C_STANDARD 11)#C11

file(GLOB_RECURSE INCLUDES "lv_drivers/*.h" "${InfiniTime_DIR}/src/libs/lvgl/src/*.h"  "./*.h" )
file(GLOB_RECURSE SOURCES  "lv_drivers/*.c" "${InfiniTime_DIR}/src/libs/lvgl/src/*.c" )

add_executable(infinisim main.cpp ${SOURCES} ${INCLUDES})

# add simulator files
target_sources(infinisim PUBLIC
  sim/displayapp/LittleVgl.h
  sim/displayapp/LittleVgl.cpp
  sim/displayapp/screens/Missing.h
  sim/displayapp/screens/Missing.cpp
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
  sim/components/ble/weather/WeatherService.h
  sim/components/ble/weather/WeatherService.cpp
  sim/components/brightness/BrightnessController.h
  sim/components/brightness/BrightnessController.cpp
  sim/components/firmwarevalidator/FirmwareValidator.h
  sim/components/firmwarevalidator/FirmwareValidator.cpp
  sim/components/fs/FS.h
  sim/components/fs/FS.cpp
  sim/components/heartrate/HeartRateController.h
  sim/components/heartrate/HeartRateController.cpp
  sim/components/motion/MotionController.h
  sim/components/motion/MotionController.cpp
  sim/components/motor/MotorController.h
  sim/components/motor/MotorController.cpp
  sim/drivers/Watchdog.h
  sim/drivers/Watchdog.cpp
  sim/drivers/Bma421.h
  sim/drivers/Bma421.cpp
  sim/drivers/Cst816s.h
  sim/drivers/Cst816s.cpp
  sim/drivers/Hrs3300.h
  sim/drivers/Hrs3300.cpp
  sim/drivers/SpiMaster.h
  sim/drivers/SpiMaster.cpp
  sim/drivers/TwiMaster.h
  sim/drivers/TwiMaster.cpp
  sim/drivers/SpiNorFlash.h
  sim/drivers/SpiNorFlash.cpp
  sim/heartratetask/HeartRateTask.h
  sim/heartratetask/HeartRateTask.cpp
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
  # nrf/components/libraries/timer
  sim/libraries/timer/app_timer.h
  sim/libraries/timer/app_timer.cpp
  sim/libraries/gpiote/app_gpiote.h # includes hal/nrf_gpio.h
  )
target_include_directories(infinisim PRIVATE "sim")
target_include_directories(infinisim PRIVATE "sim/libraries/log") # for nrf_log.h
target_include_directories(infinisim PRIVATE "sim/libraries/timer") # for app_timer.h
target_include_directories(infinisim PRIVATE "sim/nrfx") # for nrfx_log.h and others
target_include_directories(infinisim PRIVATE "sim/nrfx/hal") # for nrfx_log.h

target_compile_definitions(infinisim PRIVATE LV_CONF_INCLUDE_SIMPLE)
target_sources(infinisim PUBLIC
  # LVGL Fonts
  ${InfiniTime_DIR}/src/libs/lvgl/src/lv_font/lv_font_montserrat_14.c
  ${InfiniTime_DIR}/src/libs/lvgl/src/lv_font/lv_font_montserrat_18.c
  ${InfiniTime_DIR}/src/libs/lvgl/src/lv_font/lv_font_montserrat_22.c
  ${InfiniTime_DIR}/src/libs/lvgl/src/lv_font/lv_font_montserrat_28.c

  ${InfiniTime_DIR}/src/displayapp/fonts/lv_font_navi_80.c

  ${InfiniTime_DIR}/src/displayapp/fonts/jetbrains_mono_extrabold_compressed.c
  ${InfiniTime_DIR}/src/displayapp/fonts/jetbrains_mono_bold_20.c
  ${InfiniTime_DIR}/src/displayapp/fonts/jetbrains_mono_76.c
  ${InfiniTime_DIR}/src/displayapp/fonts/jetbrains_mono_42.c
  ${InfiniTime_DIR}/src/displayapp/fonts/lv_font_sys_48.c
  ${InfiniTime_DIR}/src/displayapp/fonts/open_sans_light.c
)

target_include_directories(infinisim PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
target_include_directories(infinisim PRIVATE "${InfiniTime_DIR}/src/libs")
target_include_directories(infinisim PRIVATE "lv_drivers")

# add dates library
target_include_directories(infinisim SYSTEM PRIVATE "${InfiniTime_DIR}/src/libs/date/includes")

# add Screens with a globbing expression, to enable easier CI test-runs for PRs adding new Screens
file(GLOB InfiniTime_SCREENS
  "${InfiniTime_DIR}/src/displayapp/screens/*.h"
  "${InfiniTime_DIR}/src/displayapp/screens/*.cpp"
  "${InfiniTime_DIR}/src/displayapp/screens/settings/*.h"
  "${InfiniTime_DIR}/src/displayapp/screens/settings/*.cpp"
)
target_sources(infinisim PUBLIC ${InfiniTime_SCREENS})

# add files directly from InfiniTime sources
target_include_directories(infinisim PRIVATE "${InfiniTime_DIR}/src")
target_sources(infinisim PUBLIC
  ${InfiniTime_DIR}/src/BootloaderVersion.h
  ${InfiniTime_DIR}/src/BootloaderVersion.cpp
  ${InfiniTime_DIR}/src/displayapp/Colors.h
  ${InfiniTime_DIR}/src/displayapp/Colors.cpp
  ${InfiniTime_DIR}/src/displayapp/DisplayApp.h
  ${InfiniTime_DIR}/src/displayapp/DisplayApp.cpp
  ${InfiniTime_DIR}/src/displayapp/lv_pinetime_theme.h
  ${InfiniTime_DIR}/src/displayapp/lv_pinetime_theme.c
  ${InfiniTime_DIR}/src/displayapp/icons/bg_clock.c # used by WatchFaceAnalog.cpp
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
  ${InfiniTime_DIR}/src/components/timer/TimerController.h
  ${InfiniTime_DIR}/src/components/timer/TimerController.cpp
  ${InfiniTime_DIR}/src/drivers/PinMap.h
  ${InfiniTime_DIR}/src/drivers/Spi.h
  ${InfiniTime_DIR}/src/drivers/Spi.cpp
  ${InfiniTime_DIR}/src/drivers/St7789.h
  ${InfiniTime_DIR}/src/drivers/St7789.cpp
  ${InfiniTime_DIR}/src/touchhandler/TouchHandler.h
  ${InfiniTime_DIR}/src/touchhandler/TouchHandler.cpp
  ${InfiniTime_DIR}/src/systemtask/SystemTask.h
  ${InfiniTime_DIR}/src/systemtask/SystemTask.cpp
  ${InfiniTime_DIR}/src/systemtask/SystemMonitor.h
  ${InfiniTime_DIR}/src/systemtask/SystemMonitor.cpp
)


list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
# Special case for SDL2 dependency, goal is to find a config that exports SDL2::SDL2 target
# libsdl2-dev has a `sdl2-config.cmake` that doesn't export this, but vcpkg does..
find_package(SDL2 CONFIG QUIET)
if(NOT TARGET SDL2::SDL2)
  find_package(SDL2 MODULE REQUIRED)
endif()
target_link_libraries(infinisim PRIVATE SDL2::SDL2)

# Get the latest abbreviated commit hash of the working branch
execute_process(
  COMMAND ${GIT_EXECUTABLE} log -1 --format=%h
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  OUTPUT_VARIABLE PROJECT_GIT_COMMIT_HASH
  OUTPUT_STRIP_TRAILING_WHITESPACE
)
set(VERSION_EDIT_WARNING "// Do not edit this file, it is automatically generated by CMAKE!")
configure_file("${InfiniTime_DIR}/src/Version.h.in" "${CMAKE_CURRENT_BINARY_DIR}/Version.h")

target_include_directories(infinisim PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")

option(WITH_PNG "Compile with libpng support to dump current screen as png" ON)
if(WITH_PNG)
  target_compile_definitions(infinisim PRIVATE WITH_PNG)
  add_subdirectory(libpng EXCLUDE_FROM_ALL)
  target_link_libraries(infinisim PRIVATE png_static)
endif()
