cmake_minimum_required(VERSION 3.10)
# HelloWorld example with mcapLogger
# This example demonstrates how to use the mcapLogger library to log data
# in the MCAP format. It includes a simple HelloWorld program that
# initializes the mcapLogger library, creates a log file, and writes
# a message to the log file
set(TARGET HelloWorld)

project(TARGET LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(${TARGET}
    HelloWorld.cpp
)

target_include_directories(${TARGET} PRIVATE
    ${CMAKE_SOURCE_DIR}/externals/mcap/include
)

# Disable LZ4 and Zstd compression for mcapLogger
target_compile_definitions(${TARGET} PRIVATE
    MCAP_COMPRESSION_NO_LZ4
    MCAP_COMPRESSION_NO_ZSTD
)

# HelloWorldWithFlatbuffers example with mcapLogger
# This example demonstrates how to use the mcapLogger library to log data
# in the MCAP format with Flatbuffers. It includes a simple HelloWorld program
# that initializes the mcapLogger library, creates a log file, and writes
# a message to the log file using Flatbuffers serialization and and deserialization of data

set(TARGET1 HelloWorldWithFlatbuffers)

project(TARGET1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(${TARGET1}
    HelloWorldWithFlatbuffers.cpp
    vehicle_sim_generated.h
)

# Specify the include directories for your executable to find the header-only library
target_include_directories(${TARGET1} PRIVATE
    ${CMAKE_SOURCE_DIR}/externals/mcap/include
    ${CMAKE_SOURCE_DIR}/externals/flatbuffers/include
)

# Disable LZ4 and Zstd compression for mcapLogger
target_compile_definitions(${TARGET1} PRIVATE
    MCAP_COMPRESSION_NO_LZ4
    MCAP_COMPRESSION_NO_ZSTD
)

# with schema decoded in schema data

set(TARGET2 HelloWorldWithFlatbuffersWithSchemaEncoded)

project(TARGET2 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(${TARGET2}
    HelloWorldWithFlatbuffersWithSchemaEncoded.cpp
    vehicle_sim_generated.h
)

# Specify the include directories for your executable to find the header-only library
target_include_directories(${TARGET2} PRIVATE
    ${CMAKE_SOURCE_DIR}/externals/mcap/include
    ${CMAKE_SOURCE_DIR}/externals/flatbuffers/include
)

# Disable LZ4 and Zstd compression for mcapLogger
target_compile_definitions(${TARGET2} PRIVATE
    MCAP_COMPRESSION_NO_LZ4
    MCAP_COMPRESSION_NO_ZSTD
)
