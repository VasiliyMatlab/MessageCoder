cmake_minimum_required(VERSION 3.0.0)
project(MessageCoderExample
        LANGUAGES C)

add_executable(example.elf main.c)

target_link_libraries(example.elf ${CMAKE_BINARY_DIR}/src/lib/libmesscoder.a)

install(TARGETS example.elf DESTINATION ${OUTPUT_DIRECTORY})
