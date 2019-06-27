cmake_minimum_required(VERSION 3.13.0)

project(librequests)


add_library(requests
    src/requests.c
    include/requests.h
    )
target_include_directories(requests PRIVATE include)

add_executable(
    test

    test/test.c
    test/greatest.h
    )

target_include_directories(test PRIVATE include)
target_link_libraries(test requests curl)

add_subdirectory(examples)
