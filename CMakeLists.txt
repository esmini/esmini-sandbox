cmake_minimum_required(VERSION 3.10)

project(esmini-sandbox)

add_subdirectory(pathfinder)
add_subdirectory(rounding)

if(NOT EXISTS "externals/ryml/.git")
    execute_process(
        COMMAND git submodule update --init --recursive
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
endif()

add_subdirectory(externals/ryml)
add_subdirectory(yamlparser)
