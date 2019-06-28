add_library(greatest_headers INTERFACE)
target_include_directories(greatest_headers INTERFACE third-party/greatest/include)

add_executable(
    test

    test.c
    )

target_link_libraries(test greatest_headers requests)
