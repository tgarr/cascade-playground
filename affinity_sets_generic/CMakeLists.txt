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

add_library(entry_udl SHARED entry_udl.cpp)
target_include_directories(entry_udl PRIVATE
    $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}>
)

add_library(category_udl SHARED category_udl.cpp)
target_include_directories(category_udl PRIVATE
    $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}>
)

add_library(return_udl SHARED return_udl.cpp)
target_include_directories(return_udl PRIVATE
    $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}>
)

add_executable(pipeline_client pipeline_client.cpp)
set_target_properties(pipeline_client PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES ${cascade_INCLUDE_DIRS}
)
target_link_libraries(pipeline_client derecho::cascade)

