project(nlohmann_json LANGUAGES CXX VERSION 3.11.2)

add_library(nlohmann_json INTERFACE)

target_include_directories(nlohmann_json
  SYSTEM INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
)
# Enable extended diagnostics information
# https://github.com/nlohmann/json/releases/tag/v3.10.0
target_compile_definitions(nlohmann_json INTERFACE JSON_DIAGNOSTICS=1)

# provide a namespaced alias for clients to 'link' against if nlohman_json is included as a sub-project
add_library(nlohmann_json::nlohmann_json ALIAS nlohmann_json)
