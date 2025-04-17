set(TARGET mcapLogger)

add_executable(${TARGET}
    main.cpp
    vehicle_sim_generated.h
)

# --- Mandatory C++17 Setup ---
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)  # Disable MSVC extensions

# --- Windows-Specific Fixes ---
if(MSVC)
  add_compile_definitions(
    _SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING
    _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS
  )
  add_compile_options(/Zc:__cplusplus)  # Proper __cplusplus macro
endif()

# Specify the include directories for your executable to find the header-only library
target_include_directories(${TARGET} PRIVATE
    ${CMAKE_SOURCE_DIR}/externals/mcap/include
    ${CMAKE_SOURCE_DIR}/externals/flatbuffers/include
)

# Disable LZ4 and Zstd compression for mcapLogger
target_compile_definitions(${TARGET} PRIVATE
    MCAP_COMPRESSION_NO_LZ4
    MCAP_COMPRESSION_NO_ZSTD
)