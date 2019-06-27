add_executable(
    test

    test.c
    greatest.h
    )

target_include_directories(test PRIVATE ../include)
target_link_libraries(test requests curl)
