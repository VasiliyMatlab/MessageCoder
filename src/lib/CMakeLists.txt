cmake_minimum_required(VERSION 3.15.0)
project(MessageCoderLib
        LANGUAGES C)

add_library(messcoder STATIC mess_coder.c)

install(TARGETS messcoder DESTINATION ${OUTPUT_DIRECTORY})
