/*
 * Submit a GET request.
 */

#include <stdio.h>
#include "requests.h"

int main(int argc, const char *argv[])
{
    req_t req;                        /* declare struct to store data */
    CURL *curl = requests_init(&req); /* setup */

    requests_get(curl, &req, "http://example.com"); /* submit GET request */
    printf("Request URL: %s\n", req.url);
    printf("Response Code: %lu\n", req.code);
    printf("Response Size: %zu\n", req.size);
    printf("Response Headers:\n");
    int i = 0;
    for (i = 0; i < req.req_hdrc; i++) {
        printf("\t %s", req.req_hdrv[i]);
    }
    printf("Response Body:\n%s", req.text);

    requests_close(&req); /* always do this at the end */
    return 0;
}
