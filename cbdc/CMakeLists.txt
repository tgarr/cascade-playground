cmake_minimum_required(VERSION 3.12.0)
set(CMAKE_DISABLE_SOURCE_CHANGES ON)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)
project(cascade_cbdc)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_executable(generate_workload generate_workload.cpp benchmark_workload.cpp)
target_link_libraries(generate_workload gzstream z)

