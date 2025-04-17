set(TARGET mcapLogger)

add_executable(${TARGET}
    main.cpp
    vehicle_sim_generated.h
)

# Specify the include directories for your executable to find the header-only library
target_include_directories(${TARGET} PRIVATE
    ${CMAKE_SOURCE_DIR}/externals/mcap/include
    ${CMAKE_SOURCE_DIR}/externals/flatbuffers
)

# Disable LZ4 and Zstd compression for mcapLogger
target_compile_definitions(${TARGET} PRIVATE
    MCAP_COMPRESSION_NO_LZ4
    MCAP_COMPRESSION_NO_ZSTD
)