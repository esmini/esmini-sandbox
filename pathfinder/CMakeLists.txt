set(TARGET pathfinderwithlib)

# Create the shared library in a folder separate from the executable
add_library(libpath SHARED libpath.cpp)
set_target_properties(libpath PROPERTIES RUNTIME_OUTPUT_DIRECTORY lib)
set_target_properties(libpath PROPERTIES LIBRARY_OUTPUT_DIRECTORY lib)

# Create the executable
add_executable(${TARGET} pathFinderWithLib.cpp)
set_target_properties(${TARGET} PROPERTIES RUNTIME_OUTPUT_DIRECTORY bin)

add_executable(pathfinder main.cpp)
set_target_properties(pathfinder PROPERTIES RUNTIME_OUTPUT_DIRECTORY bin)

# Define macro for DLL export on Windows
target_compile_definitions(${TARGET} PRIVATE BUILDING_LIBPATH)

# Specify include directories
target_include_directories(${TARGET} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

if(NOT MSVC)
    target_link_libraries(libpath PRIVATE dl)
endif()

# Link the shared library
target_link_libraries(${TARGET} PRIVATE libpath)
