cmake_minimum_required(VERSION 3.12.0)
set(CMAKE_DISABLE_SOURCE_CHANGES ON)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)
project(p2p_send_debug)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

find_package(nlohmann_json 3.2.0 REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(spdlog 1.3.1 REQUIRED)
find_package(derecho CONFIG REQUIRED)
find_package(cascade CONFIG REQUIRED)

# Debug UDL
#[[add_library(debug_udl SHARED debug_udl.cpp)
target_include_directories(debug_udl PRIVATE
    $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}>
)]]

# Client and config files
add_executable(debug_setup debug_setup.cpp)
set_target_properties(debug_setup PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES ${cascade_INCLUDE_DIRS}
)
target_link_libraries(debug_setup derecho::cascade)

#[[add_executable(debug_local debug_local.cpp)
set_target_properties(debug_local PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES ${cascade_INCLUDE_DIRS}
)
target_link_libraries(debug_local derecho::cascade)]]

#[[add_executable(debug_remote debug_remote.cpp)
set_target_properties(debug_remote PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES ${cascade_INCLUDE_DIRS}
)
target_link_libraries(debug_remote derecho::cascade)]]

add_custom_command(TARGET debug_setup POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/cfg
        ${CMAKE_CURRENT_BINARY_DIR}/cfg
    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/cfg/setup_config.sh
    COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/cloudlab-server.sh ${CMAKE_CURRENT_BINARY_DIR}/
    COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/cloudlab-setup.sh ${CMAKE_CURRENT_BINARY_DIR}/
    #COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/cloudlab-local.sh ${CMAKE_CURRENT_BINARY_DIR}/
    #COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/cloudlab-remote.sh ${CMAKE_CURRENT_BINARY_DIR}/
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/cfg/setup_config.sh
            ${CMAKE_CURRENT_SOURCE_DIR}/cfg/layout.json.tmp
            ${CMAKE_CURRENT_SOURCE_DIR}/cfg/dfgs.json.tmp
            ${CMAKE_CURRENT_SOURCE_DIR}/cfg/udl_dlls.cfg.tmp
            ${CMAKE_CURRENT_SOURCE_DIR}/cfg/derecho.cfg.rdma
            ${CMAKE_CURRENT_SOURCE_DIR}/cloudlab-server.sh
            ${CMAKE_CURRENT_SOURCE_DIR}/cloudlab-setup.sh
            #${CMAKE_CURRENT_SOURCE_DIR}/cloudlab-local.sh
            #${CMAKE_CURRENT_SOURCE_DIR}/cloudlab-remote.sh
)

