function(add_example_executable name)
    add_executable(${name} ${name}.c)

    target_link_libraries(${name} requests)

endfunction()

add_example_executable(get)
add_example_executable(multi_get)
add_example_executable(post)
