message(STATUS "folder img: converting background.bmp to C file to include in binary")

if(CMAKE_VERSION VERSION_GREATER_EQUAL 3.12)
   # FindPython3 module introduces with CMake 3.12
   # https://cmake.org/cmake/help/latest/module/FindPython3.html
   find_package(Python3 REQUIRED)
else()
   set(Python3_EXECUTABLE "python")
endif()

set(bmp_file ${CMAKE_CURRENT_SOURCE_DIR}/sim_background.bmp)
set(out_file ${CMAKE_CURRENT_BINARY_DIR}/sim_background.h)
# call python script to convert image to c file
add_custom_command(
   OUTPUT ${out_file}
   COMMAND "${Python3_EXECUTABLE}" ${CMAKE_CURRENT_SOURCE_DIR}/convert_bmp_to_header.py
   ${bmp_file}
   --var-name "SIM_BACKGROUND"
   --output ${out_file}
   DEPENDS ${bmp_file}
   WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)
# add target that can be added as dependency to infinisim target to trigger creation of C-file
add_custom_target(infinisim_img_background
   DEPENDS ${out_file}
)
