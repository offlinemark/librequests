/*
 * Submit more than one GET request.
 */

#include <stdio.h>
#include "requests.h"

int main(int argc, const char *argv[])
{
    req_t req;                        /* declare struct used to store data */
    CURL *curl = requests_init(&req); /* setup */

    requests_get(curl, &req, "http://example.com"); /* submit GET request */
    printf("Request URL: %s\n", req.url);
    printf("Response Code: %lu\n", req.code);
    printf("Response Size: %zu\n", req.size);
    printf("Response Body:\n%s", req.text);

    puts("---");

    curl = requests_init(&req);
    requests_get(curl, &req, "http://google.com");
    printf("Request URL: %s\n", req.url);
    printf("Response Code: %lu\n", req.code);
    printf("Response Size: %zu\n", req.size);
    printf("Response Body:\n%s", req.text);

    requests_close(&req); /* always do this at the end */
    return 0;
}
