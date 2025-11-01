# --- 1. Basic Project Setup ---
cmake_minimum_required(VERSION 3.10)
project(layerspy VERSION 0.1.0)

# --- 2. Set C++17 Standard ---
# We enforce C++17. This is the modern standard and gives us
# key features like std::string_view, which is perfect for parsing.
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS Off)

# --- 3. Add Dependencies (as Git Submodules) ---
# This assumes you have run:
# git submodule add https://github.com/CLIUtils/CLI11.git third_party/CLI11
# git submodule add https://github.com/catchorg/Catch2.git third_party/Catch2
#
# We tell CMake to build these submodules.
add_subdirectory(third_party/CLI11)
add_subdirectory(third_party/Catch2)

# --- 4. Find System Library (libpcap) ---
# This finds the libpcap library that must be installed on the system
# (e.g., sudo apt-get install libpcap-dev)
find_package(Pcap REQUIRED)
if(NOT Pcap_FOUND)
    message(FATAL_ERROR "Could not find libpcap. Please install it.")
endif()

# --- 5. Define Your Core Library (Component 2, 3, 4, 5) ---
# This creates a static library called "layerspy_lib"
# It contains all your core logic (engine, sniffer, decoder, display).

# Find all your .cpp files in the src/ directory.
file(GLOB_RECURSE CORE_SOURCES "src/*.cpp" "src/protocols/*.cpp")

add_library(layerspy_lib ${CORE_SOURCES})

# This tells the library where its public headers are.
# PUBLIC: The library itself needs this, and so does any target
# that links to it (like our executable).
target_include_directories(layerspy_lib
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# This links libpcap to your library.
# PRIVATE: Only the library's internal .cpp files need libpcap.
# The executable (main.cpp) doesn't need to know about it.
target_link_libraries(layerspy_lib
    PRIVATE
        ${PCAP_LIBRARIES}
)

# --- 6. Define The Executable (Component 1) ---
# This creates the final "layerspy" executable from app/main.cpp
add_executable(layerspy app/main.cpp)

# This links your executable to:
# 1. layerspy_lib (all your core logic)
# 2. CLI::CLI11 (the command-line parser)
target_link_libraries(layerspy
    PRIVATE
        layerspy_lib
        CLI::CLI11
)

# --- 7. Setup Testing ---
# This block sets up your unit tests.
enable_testing()

# Find all your test source files
file(GLOB_RECURSE TEST_SOURCES "test/*.cpp")

# Create the test executable
add_executable(layerspy_test ${TEST_SOURCES})

# Link your tests to:
# 1. layerspy_lib (to test your decoder, sniffer, etc.)
# 2. Catch2::Catch2WithMain (the testing framework)
target_link_libraries(layerspy_test
    PRIVATE
        layerspy_lib
        Catch2::Catch2WithMain
)

# Register the executable as a test with CTest
add_test(NAME layerspy_test COMMAND layerspy_test)