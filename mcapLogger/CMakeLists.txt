set(TARGET mcapLogger)

add_executable(${TARGET}
    main.cpp
    vehicle_sim_generated.h
)

# --- MSVC-Specific Fixes ---
if(MSVC)
  # Force C++17 through compiler flags instead of CMAKE_CXX_STANDARD
  add_compile_options(
    "/std:c++17"  # Explicit MSVC C++17 flag
    "/Zc:__cplusplus"
    "/permissive-"
  )

  add_compile_definitions(
    _SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING
    _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS
    NOMINMAX
  )
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