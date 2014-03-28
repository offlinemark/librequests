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

typedef struct {
    long code;
    char *url;
    char *text;
    size_t size;
} REQ;

CURL *requests_init(REQ *req, char *url);
void requests_close(CURL *curl, REQ *req);
void requests_get(CURL *curl, REQ *req);
void requests_pt(CURL *curl, REQ *req, char **data, int data_size,
                 char **headers, int headers_size, int put_flag);
void requests_post(CURL *curl, REQ *req, char **data, int data_size);
void requests_put(CURL *curl, REQ *req, char **data, int data_size);
void requests_post_headers(CURL *curl, REQ *req, char **data, int data_size,
                           char **headers, int headers_size);
void requests_put_headers(CURL *curl, REQ *req, char **data, int data_size,
                          char **headers, int headers_size);
void common_opt(CURL *curl, REQ *req);
char *url_encode(CURL *curl, char **data, int data_size);
size_t callback(char *content, size_t size, size_t nmemb, REQ *userdata);
char *user_agent();

#endif
