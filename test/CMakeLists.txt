add_executable(
    test

    test.c
    )

target_link_libraries(test  requests curl)
