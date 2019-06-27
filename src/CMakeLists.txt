add_library(requests

    requests.c
    ../include/requests.h
    )
target_include_directories(requests PRIVATE ../include)
