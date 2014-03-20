#ifndef REQUESTS_H
#define REQUESTS_H

/*
 * Mark Mossberg, 2014
 * mark.mossberg@gmail.com
 */

#include <string.h>
#include <stdlib.h>
#include <curl/curl.h>

typedef struct {
    long code;
    char *url;
    char *text;
    size_t size;
} REQ;

void requests_get(CURL *curl, REQ *req);
void requests_close(CURL *curl, REQ *req);
CURL *requests_init(REQ *req, char *url);

#endif
