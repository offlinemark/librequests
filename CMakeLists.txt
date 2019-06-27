cmake_minimum_required(VERSION 3.13.3)
project(librequests)

function(main)
    add_subdirectory(src)
    add_subdirectory(test)
    add_subdirectory(examples)
endfunction()

main()


