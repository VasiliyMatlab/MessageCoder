cmake_minimum_required(VERSION 3.15.0)
project(MessageCoderServer
        LANGUAGES C)

add_executable(server.elf main.c)

target_link_libraries(server.elf ${CMAKE_BINARY_DIR}/src/lib/libmesscoder.a)

install(TARGETS server.elf DESTINATION ${OUTPUT_DIRECTORY})
