add_executable(
    test

    test.c
    greatest.h
    )

target_link_libraries(test requests curl)
