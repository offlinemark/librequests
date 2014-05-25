/*
 * requests.c -- librequests: libcurl wrapper implementation
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

#include "requests.h"

const req_t REQ_DEFAULT = {0, NULL, NULL, 0, NULL, 0, 0};

/*
 * Initializes requests struct data members
 */
CURL *requests_init(req_t *req)
{
    /* if this is not their first request */
    if (req->url != NULL) {
        free(req->text);
        free(req->headers);
    }

    req->code = 0;
    req->url = NULL;
    req->text = calloc(1, 1);
    req->size = 0;
    req->headers = calloc(1, 1);
    req->headers_size = 0;
    req->ok = 0;

    if (req->text == NULL || req->headers == NULL) {
        printf("[!] librequests error: Memory allocation failed");
        exit(1);
    }

    return curl_easy_init();
}

/*
 * Calls curl clean up and free allocated memory
 */
void requests_close(req_t *req)
{
    free(req->text);
    int i = 0;
    for (i = 0; i < req->headers_size; i++) {
        free(req->headers[i]);
    }
    free(req->headers);
}

/*
 * Callback function for requests, may be called multiple times per request.
 * Allocates memory and assembles response data.
 */
size_t callback(char *content, size_t size, size_t nmemb, req_t *userdata)
{
    size_t real_size = size * nmemb;

    /* extra 1 is for null byte */
    userdata->text = realloc(userdata->text, userdata->size + real_size + 1);
    if (userdata->text == NULL) {
        printf("[!] librequests error: Memory allocation failed");
        exit(1);
    }

    userdata->size += real_size;

    char *responsetext = strndup(content, size * nmemb + 1);
    if (responsetext == NULL) {
        printf("[!] librequests error: Memory allocation failed");
        exit(1);
    }

    strcat(userdata->text, responsetext);

    free(responsetext);
    return real_size;
}

/*
 * Callback function for headers, called once for each header. Allocates
 * memory and assembles headers into string array.
 */
size_t header_callback(char *content, size_t size, size_t nmemb,
                       req_t *userdata)
{
    size_t real_size = size * nmemb;
    size_t current_size = userdata->headers_size * sizeof(char*);

    /* the last header is always "\r\n" which we'll intentionally skip */
    if (strcmp(content, "\r\n") == 0)
        return real_size;

    userdata->headers = realloc(userdata->headers,
                                current_size + sizeof(char*));
    if (userdata->headers == NULL) {
        printf("[!] librequests error: Memory allocation failed.");
        exit(1);
    }

    userdata->headers_size++;
    userdata->headers[userdata->headers_size - 1] = strndup(content,
                                                         size * nmemb + 1);
    return real_size;
}

/*
 * requests_get - Performs GET request and populates req struct text member
 * with request response, code with response code, and size with size of
 * response.
 *
 * Returns the CURLcode return code provided from curl_easy_perform. CURLE_OK
 * is returned on success.
 *
 * @curl: libcurl handle
 * @req:  request struct
 * @url:  url to send request to
 */
CURLcode requests_get(CURL *curl, req_t *req, char *url)
{
    CURLcode rc;
    char *ua = user_agent();
    req->url = url;
    long code = 0;

    common_opt(curl, req);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, req);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, ua);
    rc = curl_easy_perform(curl);
    if (rc != CURLE_OK)
        return rc;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &code);

    req->code = code;
    check_ok(req);
    curl_easy_cleanup(curl);
    free(ua);

    return rc;
}

/*
 * requests_url_encode - Url encoding function. Takes as input an array of
 * char strings and the size of the array. The array should consist of keys
 * and the corresponding value immediately after in the array. There must be
 * an even number of array elements (one value for every key).
 *
 * Returns pointer to url encoded string if successful, or NULL if
 * unsuccessful.
 *
 * @curl:      libcurl handle
 * @data:      char* array as described above
 * @data-size: length of array
 */
char *requests_url_encode(CURL *curl, char **data, int data_size)
{
    char *key, *val;
    int offset;
    size_t term_size;

    if (data_size % 2 != 0)
        return NULL;

    /* loop through and get total sum of lengths */
    size_t total_size = 0;
    int i = 0;
    for (i = 0; i < data_size; i++) {
        char *tmp = data[i];
        size_t tmp_len = strlen(tmp);
        total_size += tmp_len;
    }

    char encoded[total_size]; /* clear junk bytes */
    snprintf(encoded, total_size, "%s", "");

    /* loop in groups of two, assembling key/val pairs */
    for (i = 0; i < data_size; i+=2) {
        key = data[i];
        val = data[i+1];
        offset = i == 0 ? 2 : 3; /* =, \0 and maybe & */
        term_size = strlen(key) + strlen(val) + offset;
        char term[term_size];
        if (i == 0)
            snprintf(term, term_size, "%s=%s", key, val);
        else
            snprintf(term, term_size, "&%s=%s", key, val);
        strncat(encoded, term, strlen(term));
    }

    char *full_encoded = curl_easy_escape(curl, encoded, strlen(encoded));

    return full_encoded;
}

CURLcode requests_post(CURL *curl, req_t *req, char *url, char *data)
{
    return requests_pt(curl, req, url, data, NULL, 0, 0);
}

CURLcode requests_put(CURL *curl, req_t *req, char *url, char *data)
{
    return requests_pt(curl, req, url, data, NULL, 0, 1);
}

CURLcode requests_post_headers(CURL *curl, req_t *req, char *url, char *data,
                               char **headers, int headers_size)
{
    return requests_pt(curl, req, url, data, headers, headers_size, 0);
}

CURLcode requests_put_headers(CURL *curl, req_t *req, char *url, char *data,
                              char **headers, int headers_size)
{
    return requests_pt(curl, req, url, data, headers, headers_size, 1);
}

/*
 * requests_pt - Utility function that performs POST or PUT request using
 * supplied data and populates req struct text member with request response,
 * code with response code, and size with size of response. To submit no
 * data, use NULL for data, and 0 for data_size.
 *
 * Typically this function isn't used directly, use requests_post() or
 * requests_put() instead.
 *
 * @curl: libcurl handle
 * @req: request struct
 * @url: url to send request to
 * @data: url encoded data to send in request body
 * @headers: char* array of custom headers
 * @headers_size: length of `headers`
 * @put_flag: if not zero, sends PUT request, otherwise uses POST
 */
CURLcode requests_pt(CURL *curl, req_t *req, char *url, char *data,
                     char **headers, int headers_size, int put_flag)
{
    CURLcode rc;
    char *ua = user_agent();
    char *encoded = NULL;
    struct curl_slist *slist = NULL;
    long code = 0;
    req->url = url;

    /* body data */
    if (data != NULL) {
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data);
    } else {
        /* content length header defaults to -1, which causes request to fail
           sometimes, so we need to manually set it to 0 */
        slist = curl_slist_append(slist, "Content-Length: 0");
        if (headers == NULL)
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, slist);
    }

    /* headers */
    if (headers != NULL) {
        int i = 0;
        for (i = 0; i < headers_size; i++) {
            slist = curl_slist_append(slist, headers[i]);
        }
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, slist);
    }

    common_opt(curl, req);
    if (put_flag)
        /* use custom request instead of dedicated PUT, because dedicated
           PUT doesn't work with arbitrary request body data */
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
    else
        curl_easy_setopt(curl, CURLOPT_POST, 1);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, ua);
    rc = curl_easy_perform(curl);
    if (rc != CURLE_OK)
        return rc;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &code);

    req->code = code;
    check_ok(req);

    if (encoded != NULL)
        curl_free(encoded);
    if (slist != NULL)
        curl_slist_free_all(slist);
    free(ua);
    curl_easy_cleanup(curl);

    return rc;
}

/*
 * Utility function for executing common curl options.
 */
void common_opt(CURL *curl, req_t *req)
{
    curl_easy_setopt(curl, CURLOPT_URL, req->url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, req);
    curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, header_callback);
    curl_easy_setopt(curl, CURLOPT_HEADERDATA, req);
}

/*
 * Utility function for creating custom user agent.
 */
char *user_agent()
    {
    int ua_size = 3; /* ' ', /, \0 */
    char *basic = "librequests/0.1";
    ua_size += strlen(basic);
    struct utsname name;
    uname(&name);
    char *kernel = name.sysname;
    ua_size += strlen(kernel);
    char *version = name.release;
    ua_size += strlen(version);

    char *ua = malloc(ua_size);
    if (ua == NULL) {
        printf("[!] librequests error: Memory allocation failed");
        exit(1);
    }
    snprintf(ua, ua_size, "%s %s/%s", basic, kernel, version);

    return ua;
}

/*
 * Utility function for setting "ok" struct field. Response codes of 400+
 * are considered "not ok".
 */
void check_ok(req_t *req)
{
    if (req->code >= 400 || req->code == 0)
        req->ok = 0;
    else
        req->ok = 1;
}
