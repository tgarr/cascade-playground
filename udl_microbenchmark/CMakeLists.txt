cmake_minimum_required(VERSION 3.12.0)
set(CMAKE_DISABLE_SOURCE_CHANGES ON)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)
project(udl_microbenchmark)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

find_package(nlohmann_json 3.2.0 REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(spdlog 1.3.1 REQUIRED)
find_package(derecho CONFIG REQUIRED)
find_package(cascade CONFIG REQUIRED)

# Debug UDL
add_library(requester_udl SHARED requester_udl.cpp)
target_include_directories(requester_udl PRIVATE
    $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}>
)

# experiment binary
add_executable(udl_microbenchmark udl_microbenchmark.cpp)
set_target_properties(udl_microbenchmark PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES ${cascade_INCLUDE_DIRS}
)
target_link_libraries(udl_microbenchmark derecho::cascade)

# config
add_custom_command(TARGET udl_microbenchmark POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/cfg
        ${CMAKE_CURRENT_BINARY_DIR}/cfg
    COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/setup_config.sh ${CMAKE_CURRENT_BINARY_DIR}/
    COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/run_server.sh ${CMAKE_CURRENT_BINARY_DIR}/
    COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/run_client.sh ${CMAKE_CURRENT_BINARY_DIR}/
    COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/dump_timestamps.sh ${CMAKE_CURRENT_BINARY_DIR}/
    COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/get_latencies.py ${CMAKE_CURRENT_BINARY_DIR}/
    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/setup_config.sh
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/setup_config.sh
            ${CMAKE_CURRENT_SOURCE_DIR}/cfg/layout.json.tmp
            ${CMAKE_CURRENT_SOURCE_DIR}/cfg/dfgs.json.tmp
            ${CMAKE_CURRENT_SOURCE_DIR}/cfg/udl_dlls.cfg.tmp
            ${CMAKE_CURRENT_SOURCE_DIR}/cfg/derecho.cfg.rdma
)

