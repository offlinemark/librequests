function(requestsMain)

    add_library(requests

        requests.c
        )

    target_link_libraries(requests PUBLIC requests_headers curl)

    generateRequestsHeaders()

endfunction()

function(generateRequestsHeaders)
    add_library(requests_headers INTERFACE)
    target_include_directories(requests_headers INTERFACE "${CMAKE_SOURCE_DIR}/include")
endfunction()

requestsMain()
