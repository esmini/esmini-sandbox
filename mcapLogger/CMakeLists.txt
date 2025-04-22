cmake_minimum_required(VERSION 3.10)
set(TARGET mcapLogger)

project(TARGET LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(${TARGET}
    main.cpp
)

# Specify the include directories for your executable to find the header-only library
target_include_directories(${TARGET} PRIVATE
    ${CMAKE_SOURCE_DIR}/externals/mcap/include
    ${CMAKE_SOURCE_DIR}/externals/zstd/include/lib
)

target_link_libraries(${TARGET} PRIVATE zstd)

# Disable LZ4 and Zstd compression for mcapLogger
target_compile_definitions(${TARGET} PRIVATE
    MCAP_COMPRESSION_NO_LZ4
    # MCAP_COMPRESSION_NO_ZSTD
)