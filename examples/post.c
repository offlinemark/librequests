/*
 * Submit a POST request.
 */

#include <stdio.h>
#include "requests.h"

int main(int argc, const char *argv[])
{
    char *data[] = {
        "apple",  "red", // array of key-value pairs
        "banana", "yellow"
    };
    int data_size = sizeof(data)/sizeof(char*); // recommended way to get size
                                                // of array
    REQ req; // declare struct used to store data
    CURL *curl = requests_init(&req, "http://www.posttestserver.com/post.php"); // setup

    requests_post(curl, &req, data, data_size); // submit POST request
    printf("Request URL: %s\n", req.url);
    printf("Response Code: %lu\n", req.code);
    printf("Response Size: %zu\n", req.size);
    printf("Response Body:\n%s", req.text);

    requests_close(curl, &req); // clean up
    return 0;
}
