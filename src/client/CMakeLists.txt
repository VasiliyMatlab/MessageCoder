cmake_minimum_required(VERSION 3.15.0)
project(MessageCoderClient
        LANGUAGES C)

add_executable(client.elf main.c rbuf.c)

target_link_libraries(client.elf ${CMAKE_BINARY_DIR}/src/lib/libmesscoder.a)

install(TARGETS client.elf DESTINATION ${OUTPUT_DIRECTORY})
