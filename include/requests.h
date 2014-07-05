#ifndef REQUESTS_H
#define REQUESTS_H

/*
 * requests.h -- librequests: header file
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2014 Mark Mossberg <mark.mossberg@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <curl/curl.h>
#include <sys/utsname.h>

#define __LIBREQ_VERS__ "v0.2"

typedef struct {
    long code;
    char *url;
    char *text;
    size_t size;
    char **req_hdrv;
    int req_hdrc;
    char **resp_hdrv;
    int resp_hdrc;
    int ok;
} req_t;

CURL *requests_init(req_t *req);
void requests_close(req_t *req);
CURLcode requests_get(CURL *curl, req_t *req, char *url);
CURLcode requests_post(CURL *curl, req_t *req, char *url, char *data);
CURLcode requests_put(CURL *curl, req_t *req, char *url, char *data);
CURLcode requests_get_headers(CURL *curl, req_t *req, char *url, 
                              char **custom_hdrv, int custom_hdrc);
CURLcode requests_post_headers(CURL *curl, req_t *req, char *url, char *data,
                               char **custom_hdrv, int custom_hdrc);
CURLcode requests_put_headers(CURL *curl, req_t *req, char *url, char *data,
                              char **custom_hdrv, int custom_hdrc);
char *requests_url_encode(CURL *curl, char **data, int data_size);

#endif
