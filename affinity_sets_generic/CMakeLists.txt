cmake_minimum_required(VERSION 3.12.0)
set(CMAKE_DISABLE_SOURCE_CHANGES ON)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)
project(affinity_sets_generic)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

find_package(nlohmann_json 3.2.0 REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(spdlog 1.3.1 REQUIRED)
find_package(derecho CONFIG REQUIRED)
find_package(cascade CONFIG REQUIRED)

# Entry UDL
add_library(entry_udl SHARED entry_udl.cpp)
target_include_directories(entry_udl PRIVATE
    $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}>
)

# Category UDL
add_library(category_udl SHARED category_udl.cpp)
target_include_directories(category_udl PRIVATE
    $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}>
)

# Return UDL
add_library(return_udl SHARED return_udl.cpp)
target_include_directories(return_udl PRIVATE
    $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}>
)

# Client and config files
add_executable(pipeline_client pipeline_client.cpp)
set_target_properties(pipeline_client PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES ${cascade_INCLUDE_DIRS}
)
target_link_libraries(pipeline_client derecho::cascade)

if(NOT DEFINED NUM_SHARDS)
    set(NUM_SHARDS 2)
endif(NOT DEFINED NUM_SHARDS)

if(NOT DEFINED RUN_MODE)
    set(RUN_MODE "local")
endif(NOT DEFINED RUN_MODE)

if(DEFINED NUM_CATEGORIES)
    target_compile_definitions(pipeline_client PRIVATE NUM_CATEGORIES=${NUM_CATEGORIES})
endif(DEFINED NUM_CATEGORIES)

if(DEFINED BENCHMARK_TIME)
    target_compile_definitions(pipeline_client PRIVATE BENCHMARK_TIME=${BENCHMARK_TIME})
endif(DEFINED BENCHMARK_TIME)

if(DEFINED NO_AFFINITY)
    target_compile_definitions(pipeline_client PRIVATE NO_AFFINITY=${NO_AFFINITY})
    target_compile_definitions(entry_udl PRIVATE NO_AFFINITY=${NO_AFFINITY})
    target_compile_definitions(category_udl PRIVATE NO_AFFINITY=${NO_AFFINITY})
    target_compile_definitions(return_udl PRIVATE NO_AFFINITY=${NO_AFFINITY})
endif(DEFINED NO_AFFINITY)

add_custom_command(TARGET pipeline_client POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/cfg
        ${CMAKE_CURRENT_BINARY_DIR}/cfg
    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/cfg/setup_config.sh ${NUM_SHARDS} ${RUN_MODE}
    COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/run_servers.sh ${CMAKE_CURRENT_BINARY_DIR}/
    COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/run_client.sh ${CMAKE_CURRENT_BINARY_DIR}/
    COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/cloudlab-client.sh ${CMAKE_CURRENT_BINARY_DIR}/
    COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/cloudlab-server.sh ${CMAKE_CURRENT_BINARY_DIR}/
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/cfg/setup_config.sh
            ${CMAKE_CURRENT_SOURCE_DIR}/cfg/layout.json.tmp
            ${CMAKE_CURRENT_SOURCE_DIR}/cfg/dfgs.json.tmp
            ${CMAKE_CURRENT_SOURCE_DIR}/cfg/udl_dlls.cfg.tmp
            ${CMAKE_CURRENT_SOURCE_DIR}/cfg/derecho.cfg.local
            ${CMAKE_CURRENT_SOURCE_DIR}/run_servers.sh
            ${CMAKE_CURRENT_SOURCE_DIR}/run_client.sh
            ${CMAKE_CURRENT_SOURCE_DIR}/cloudlab-client.sh
            ${CMAKE_CURRENT_SOURCE_DIR}/cloudlab-server.sh
)

