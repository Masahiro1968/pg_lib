#
# CMakeLists.txt
#

cmake_minimum_required(VERSION 3.15)

#
# Project Information
#
project(
    pg_lib
    VERSION 0.1.0
    DESCRIPTION "PostgreSQL C Library"
    LANGUAGES C
)

#
# Build type
#
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

message(STATUS "Build type : ${CMAKE_BUILD_TYPE}")

#
# C Standard
#
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

#
# VSCode
#
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

#
# Options
#
option(PG_LIB_BUILD_SAMPLES    "Build sample programs"       ON)
option(PG_LIB_BUILD_UTILITIES  "Build utilities"             ON)
option(PG_LIB_ENABLE_SANITIZER "Enable Address/UB Sanitizer" ON)
option(PG_LIB_WERROR           "Treat warnings as errors"    ON)
option(PG_LIB_USE_OPENMP       "Use OpenMP for Sample"       ON)

#
# Find PostgreSQL
#
find_package(PostgreSQL REQUIRED)

#
# Include
#
set(PG_LIB_INCLUDES
    ${PROJECT_SOURCE_DIR}/include/pg_lib
)

set(PG_UTIL_INCLUDES
    ${PROJECT_SOURCE_DIR}/include/pg_util
)

#
# Sources
#
file(GLOB PG_LIB_SOURCES
    ${PROJECT_SOURCE_DIR}/src/*.c
)

#
# Library
#
add_library(
    pg_lib
    SHARED
        ${PG_LIB_SOURCES}
)

set_target_properties(
    pg_lib
    PROPERTIES
        DEBUG_POSTFIX D
)

target_include_directories(
    pg_lib
    PUBLIC
        ${PG_LIB_INCLUDES}
)

target_link_libraries(
    pg_lib
    PUBLIC
        PostgreSQL::PostgreSQL
)

#
# Warning Options
#
target_compile_options(
    pg_lib
    PRIVATE
        -Wall
        -Wextra
)

if(PG_LIB_WERROR)
    target_compile_options(
        pg_lib
        PRIVATE
            -Werror
    )
endif()

if(PG_LIB_USE_OPENMP)
    find_package(OpenMP REQUIRED)
    target_compile_options(
        pg_lib
        PRIVATE
            -fopenmp
    )
endif()

#
# Debug
#
if(PG_LIB_ENABLE_SANITIZER)

    add_compile_options(
        $<$<CONFIG:Debug>:-g3>
        $<$<CONFIG:Debug>:-O0>
        $<$<CONFIG:Debug>:-fsanitize=address,undefined>
        $<$<CONFIG:Debug>:-fno-omit-frame-pointer>
        $<$<CONFIG:Release>:-O3>
        $<$<CONFIG:Release>:-DNDEBUG>
    )

    add_link_options(
        $<$<CONFIG:Debug>:-fsanitize=address,undefined>
    )

endif()

#
# Utilities
#
if(PG_LIB_BUILD_UTILITIES)

    file(GLOB PG_UTIL_SOURCES
        ${PROJECT_SOURCE_DIR}/utilities/*.c
    )

    add_library(
        pg_util
        SHARED
            ${PG_UTIL_SOURCES}
    )

    set_target_properties(
        pg_util
        PROPERTIES
            DEBUG_POSTFIX D
    )

    target_include_directories(
        pg_util
        PUBLIC
            ${PG_LIB_INCLUDES}
            ${PG_UTIL_INCLUDES}
    )

endif()

#
# Samples
#
if(PG_LIB_BUILD_SAMPLES)

    file(GLOB PG_SAMPLE_SOURCES
        ${PROJECT_SOURCE_DIR}/sample/*.c
    )

    foreach(SRC ${PG_SAMPLE_SOURCES})

        get_filename_component(SAMPLE_NAME ${SRC} NAME_WE)

        add_executable(
            ${SAMPLE_NAME}
            ${SRC}
        )

        set_target_properties(
            ${SAMPLE_NAME}
            PROPERTIES
                DEBUG_POSTFIX D
        )

        target_include_directories(
            ${SAMPLE_NAME}
            PUBLIC
                ${PG_LIB_INCLUDES}
                ${PG_UTIL_INCLUDES}
        )

        target_link_libraries(
            ${SAMPLE_NAME}
            PRIVATE
                pg_lib pg_util
        )

        if(PG_LIB_USE_OPENMP)
            target_link_libraries(
                ${SAMPLE_NAME}
                PRIVATE
                    OpenMP::OpenMP_C
            )
        endif()

    endforeach()

endif()

#
# Install
#
install(
    TARGETS pg_lib
    ARCHIVE DESTINATION lib
)

install(
    DIRECTORY include/
    DESTINATION include
)
