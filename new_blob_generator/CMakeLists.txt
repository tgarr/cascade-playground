cmake_minimum_required(VERSION 3.12.0)
set(CMAKE_DISABLE_SOURCE_CHANGES ON)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)
project(new_blob_generator CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

find_package(nlohmann_json 3.2.0 REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(spdlog 1.3.1 REQUIRED)
find_package(derecho CONFIG REQUIRED)
find_package(cascade CONFIG REQUIRED)
find_package(cascade_python_udl CONFIG REQUIRED)
find_package(Python3 3.10.0 REQUIRED COMPONENTS Interpreter Development Development.Module Development.Embed NumPy)

# CPP source UDL
add_library(source_udl SHARED source_udl.cpp)
target_include_directories(source_udl PRIVATE
    $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}>
)

# CPP destination UDL
add_library(dest_udl SHARED dest_udl.cpp)
target_include_directories(dest_udl PRIVATE
    $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}>
)

add_custom_command(TARGET source_udl POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/cfg
        ${CMAKE_CURRENT_BINARY_DIR}/cfg
    COMMAND ln -sf ${CMAKE_CURRENT_BINARY_DIR}/cfg/layout.json.tmp
        ${CMAKE_CURRENT_BINARY_DIR}/cfg/n0/layout.json
    COMMAND ln -sf ${CMAKE_CURRENT_BINARY_DIR}/cfg/layout.json.tmp
        ${CMAKE_CURRENT_BINARY_DIR}/cfg/n1/layout.json
    COMMAND ln -sf ${CMAKE_CURRENT_BINARY_DIR}/cfg/layout.json.tmp
        ${CMAKE_CURRENT_BINARY_DIR}/cfg/n2/layout.json
    COMMAND ln -sf ${CMAKE_CURRENT_BINARY_DIR}/cfg/dfgs.json.tmp
        ${CMAKE_CURRENT_BINARY_DIR}/cfg/n0/dfgs.json
    COMMAND ln -sf ${CMAKE_CURRENT_BINARY_DIR}/cfg/dfgs.json.tmp
        ${CMAKE_CURRENT_BINARY_DIR}/cfg/n1/dfgs.json
    COMMAND ln -sf ${CMAKE_CURRENT_BINARY_DIR}/cfg/dfgs.json.tmp
        ${CMAKE_CURRENT_BINARY_DIR}/cfg/n2/dfgs.json
    COMMAND ln -sf ${CMAKE_CURRENT_BINARY_DIR}/cfg/udl_dlls.cfg.tmp
        ${CMAKE_CURRENT_BINARY_DIR}/cfg/n0/udl_dlls.cfg
    COMMAND ln -sf ${CMAKE_CURRENT_BINARY_DIR}/cfg/udl_dlls.cfg.tmp
        ${CMAKE_CURRENT_BINARY_DIR}/cfg/n1/udl_dlls.cfg
    COMMAND ln -sf ${CMAKE_CURRENT_BINARY_DIR}/cfg/udl_dlls.cfg.tmp
        ${CMAKE_CURRENT_BINARY_DIR}/cfg/n2/udl_dlls.cfg
    COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/*.py ${CMAKE_CURRENT_BINARY_DIR}
    COMMAND ln -sf ${CMAKE_CURRENT_BINARY_DIR}/pyudls.py
        ${CMAKE_CURRENT_BINARY_DIR}/cfg/n0/pyudls.py
    COMMAND ln -sf ${CMAKE_CURRENT_BINARY_DIR}/pyudls.py
        ${CMAKE_CURRENT_BINARY_DIR}/cfg/n1/pyudls.py
    COMMAND ln -sf ${CMAKE_CURRENT_BINARY_DIR}/client.py
        ${CMAKE_CURRENT_BINARY_DIR}/cfg/n2/client.py
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/cfg/layout.json.tmp
            ${CMAKE_CURRENT_SOURCE_DIR}/cfg/dfgs.json.tmp
            ${CMAKE_CURRENT_SOURCE_DIR}/cfg/udl_dlls.cfg.tmp
            ${CMAKE_CURRENT_SOURCE_DIR}/client.py
            ${CMAKE_CURRENT_SOURCE_DIR}/pyudls.py
    COMMENT "prepare configuration"
)


