#ifndef REQUESTS_H
#define REQUESTS_H

/*
 * C-Requests Library
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

CURL *requests_init(REQ *req, char *url);
void requests_close(CURL *curl, REQ *req);
void requests_get(CURL *curl, REQ *req);
void requests_post(CURL *curl, REQ *req, char **data, int data_size);
void common_opt(CURL *curl, REQ *req);
size_t callback(char *content, size_t size, size_t nmemb, REQ *userdata);

#endif
