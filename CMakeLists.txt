cmake_minimum_required(VERSION 3.13.3)
project(librequests)

function(main)
    add_library(requests
        src/requests.c
        include/requests.h
        )
    target_include_directories(requests PRIVATE include)

    add_subdirectory(test)
    add_subdirectory(examples)
endfunction()

main()


