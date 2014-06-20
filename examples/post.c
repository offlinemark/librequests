/*
 * Submit a POST request.
 */

#include <stdio.h>
#include "requests.h"

int main(int argc, const char *argv[])
{
    char *data[] = {
        "apple",  "red", /* array of key-value pairs */
        "banana", "yellow"
    };
    int data_size = sizeof(data)/sizeof(char*); /* recommended way to get size
                                                   of array */
    req_t req;                        /* declare struct used to store data */
    CURL *curl = requests_init(&req); /* setup */

    /* returns apple%3Dred%26banana%3Dyellow */
    char *body = requests_url_encode(curl, data, data_size);
    requests_post(curl, &req, "http://www.posttestserver.com/post.php", body); /* submit POST request */

    printf("Request URL: %s\n", req.url);
    printf("Response Code: %lu\n", req.code);
    printf("Response Size: %zu\n", req.size);
    printf("Response Body:\n%s", req.text);

    curl_free(body); /* necessary, due to internal curl allocation */
    requests_close(&req); /* clean up */
    return 0;
}
