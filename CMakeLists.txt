cmake_minimum_required(VERSION 3.15.0)
project(MessageCoder
        LANGUAGES C)

option(LIB "building lib" ON)
option(EXAMPLE "building example" OFF)
option(DOC "building documentation" OFF)

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
    add_subdirectory(src/client)
    add_subdirectory(src/server)
endif()

if (DOC)
    find_package(Doxygen
                 REQUIRED dot)
    if (NOT DOXYGEN_FOUND)
        message(FATAL_ERROR "Doxygen is needed to build the documentation")
    endif()

    add_custom_command(COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile
                       WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                       OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/doc/latex/Makefile)
    add_custom_target(documentation
                      DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/doc/latex/Makefile
                      COMMAND make -C ${CMAKE_CURRENT_SOURCE_DIR}/doc/latex)
endif()
