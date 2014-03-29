/*
 * Submit a GET request.
 */

#include <stdio.h>
#include "requests.h"

int main(int argc, const char *argv[])
{
    REQ req; // declare struct used to store data
    CURL *curl = requests_init(&req); // setup

    requests_get(curl, &req, "http://example.com"); // submit GET request
    printf("Request URL: %s\n", req.url);
    printf("Response Code: %lu\n", req.code);
    printf("Response Size: %zu\n", req.size);
    printf("Response Body:\n%s", req.text);

    requests_close(curl, &req); // clean up
    return 0;
}
