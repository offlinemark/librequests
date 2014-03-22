#ifndef REQUESTS_H
#define REQUESTS_H

/*
 * requests.h -- c-requests: header file
 * Copyright (c) 2014 Mark Mossberg
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
char *url_encode(CURL *curl, char **data, int data_size);
size_t callback(char *content, size_t size, size_t nmemb, REQ *userdata);

#endif
