set(TARGET yamlparser)

add_executable(${TARGET} main.cpp)

target_link_libraries(${TARGET} PRIVATE ryml)