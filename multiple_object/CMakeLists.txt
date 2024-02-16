cmake_minimum_required(VERSION 3.12.0)
set(CMAKE_DISABLE_SOURCE_CHANGES ON)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)
project(multiple_object)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

find_package(nlohmann_json 3.2.0 REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(spdlog 1.3.1 REQUIRED)
find_package(derecho CONFIG REQUIRED)
find_package(cascade CONFIG REQUIRED)

add_executable(multiobj_test multiobj_test.cpp)
set_target_properties(multiobj_test PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES ${cascade_INCLUDE_DIRS}
)
target_link_libraries(multiobj_test derecho::cascade)

# config
add_custom_command(TARGET multiobj_test POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/cfg
        ${CMAKE_CURRENT_BINARY_DIR}/cfg
    COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/setup_config.sh ${CMAKE_CURRENT_BINARY_DIR}/
    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/setup_config.sh
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/setup_config.sh
            ${CMAKE_CURRENT_SOURCE_DIR}/cfg/layout.json.tmp
            ${CMAKE_CURRENT_SOURCE_DIR}/cfg/derecho.cfg
)

