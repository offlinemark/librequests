add_executable(
    get

    get.c
    )
target_link_libraries(get requests curl)

add_executable(
   multi_get 

   multi_get.c
    )
target_link_libraries(multi_get requests curl)

add_executable(
   post 

   post.c
    )
target_link_libraries(post requests curl)
