cmake_minimum_required(VERSION 3.10)
project(layerspy VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(PROJECT_WARNINGS
    -Wall
    -Wextra
    -Wpedantic
    -Wshadow
    -Wnon-virtual-dtor
    -Werror
)
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -fsanitize=address -fsanitize=undefined" CACHE STRING "Flags used by the C++ compiler for debug builds" FORCE)
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG" CACHE STRING "Flags used by the C++ compiler for release builds" FORCE)

add_subdirectory(third_party/CLI11)
add_subdirectory(third_party/Catch2)

find_package(PCAP QUIET)
if(NOT PCAP_FOUND)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(PCAP_PKG REQUIRED libpcap)
    set(PCAP_LIBRARIES ${PCAP_PKG_LIBRARIES})
    set(PCAP_INCLUDE_DIRS ${PCAP_PKG_INCLUDE_DIRS})
    set(PCAP_FOUND TRUE)
endif()

if(NOT PCAP_FOUND)
    message(FATAL_ERROR "Could not find libpcap (PCAP). Please install libpcap-dev or set CMAKE_PREFIX_PATH/PCAP_DIR.")
endif()

file(GLOB_RECURSE CORE_SOURCES "src/*.cpp" "src/protocols/*.cpp")
add_library(layerspy_lib ${CORE_SOURCES})
target_compile_options(layerspy_lib PRIVATE ${PROJECT_WARNINGS})

target_include_directories(layerspy_lib
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    PRIVATE
        ${PCAP_INCLUDE_DIRS}
)

target_link_libraries(layerspy_lib PRIVATE ${PCAP_LIBRARIES})

add_executable(layerspy app/main.cpp)
target_compile_options(layerspy PRIVATE ${PROJECT_WARNINGS})
target_link_libraries(layerspy PRIVATE layerspy_lib CLI11::CLI11)

enable_testing()

file(GLOB_RECURSE TEST_SOURCES "test/*.cpp")
add_executable(layerspy_test ${TEST_SOURCES})
target_compile_options(layerspy_test PRIVATE ${PROJECT_WARNINGS})
target_link_libraries(layerspy_test PRIVATE layerspy_lib Catch2::Catch2WithMain)

include(CTest)
include(Catch)
catch_discover_tests(layerspy_test)
add_test(NAME layerspy_test COMMAND layerspy_test)
