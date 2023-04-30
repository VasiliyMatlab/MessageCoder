cmake_minimum_required(VERSION 3.0.0)
project(MessageCoder
        LANGUAGES C)

option(LIB "building lib" ON)
option(EXAMPLE "building example" OFF)

set(OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
add_compile_options(-Wall -Wextra -O2)
add_link_options(-Wall -Wextra -O2)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

if (LIB)
    add_subdirectory(src/lib)
endif()

if (EXAMPLE)
    add_subdirectory(src/example)
endif()
