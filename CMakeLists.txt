cmake_minimum_required(VERSION 3.0.0)
project(MessageCoder
        VERSION 1.0.0
        LANGUAGES C)

file(GLOB SRCS src/*.c)
add_library(mescoder ${SRCS})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/lib")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -O2")

install(TARGETS mescoder DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
