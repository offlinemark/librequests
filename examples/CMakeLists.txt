add_executable(
    get

    get.c
    )
target_include_directories(get PRIVATE ../include)
target_link_libraries(get requests curl)

add_executable(
   multi_get 

   multi_get.c
    )
target_include_directories(multi_get PRIVATE ../include)
target_link_libraries(multi_get requests curl)

add_executable(
   post 

   post.c
    )
target_include_directories(post PRIVATE ../include)
target_link_libraries(post requests curl)
