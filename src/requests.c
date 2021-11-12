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

/*
 * Prototypes
 */
static void common_opt(req_t *req);
static char *user_agent(void);
static int check_ok(long code);
static CURLcode requests_pt(req_t *req, char *url, char *data,
                            char **custom_hdrv, int custom_hdrc, int put_flag);
static int hdrv_append(char ***hdrv, int *hdrc, char *_new);
static CURLcode process_custom_headers(struct curl_slist **slist,
                                       req_t *req, char **custom_hdrv,
                                       int custom_hdrc);

/*
 * requests_init - Initializes requests struct data members
 *
 * Returns 0 on success, or -1 on failure.
 *
 * @req: reference to req_t to be initialized
 */
int requests_init(req_t *req)
{
    req->code = 0;
    req->url = NULL;
    req->size = 0;
    req->req_hdrc = 0;
    req->resp_hdrc = 0;
    req->ok = -1;

    req->text = calloc(1, 1);
    if (req->text == NULL){
        goto fail;
    }

    req->req_hdrv = calloc(1, 1);
    if (req->req_hdrv == NULL) {
        free(req->text);
        goto fail;
    }

    req->resp_hdrv = calloc(1, 1);
    if (req->resp_hdrv == NULL) {
        free(req->text);
        free(req->req_hdrv);
        goto fail;
    }

    req->curlhandle = curl_easy_init();
    if (req->curlhandle == NULL) {
        free(req->text);
        free(req->req_hdrv);
        free(req->resp_hdrv);
        goto fail;
    }

    return 0;

fail:
    return -1;
}

/*
 * requests_close - Calls curl clean up and free allocated memory
 *
 * @req: requests struct
 */
void requests_close(req_t *req)
{
    for (int i = 0; i < req->resp_hdrc; i++)
        free(req->resp_hdrv[i]);

    for (int i = 0; i < req->req_hdrc; i++)
        free(req->req_hdrv[i]);

    free(req->text);
    free(req->resp_hdrv);
    free(req->req_hdrv);

    curl_easy_cleanup(req->curlhandle);
}

/*
 * resp_callback - Callback function for requests, may be called multiple
 * times per request. Allocates memory and assembles response data.
 *
 * Note: `content' will not be NULL terminated.
 */
static size_t resp_callback(char *content, size_t size, size_t nmemb,
                            req_t *userdata)
{
    size_t real_size = size * nmemb;
    long original_userdata_size = userdata->size;

    /* extra 1 is for NULL terminator */
    userdata->text = (char*) realloc(userdata->text, userdata->size + real_size + 1);
    if (userdata->text == NULL)
        return -1;

    userdata->size += real_size;
    /* concatenate userdata->text with the response content */
    memcpy(userdata->text + original_userdata_size, content, real_size);
    userdata->text[original_userdata_size + real_size] = '\0';
    return real_size;
}

/*
 * header_callback - Callback function for headers, called once for each 
 * header. Allocates memory and assembles headers into string array.
 *
 * Note: `content' will not be NULL terminated.
 */
static size_t header_callback(char *content, size_t size, size_t nmemb,
                              req_t *userdata)
{
    size_t real_size = size * nmemb;

    /* the last header is always "\r\n" which we'll intentionally skip */
    if (strcmp(content, "\r\n") == 0)
        return real_size;

    if (hdrv_append(&userdata->resp_hdrv, &userdata->resp_hdrc, content))
        return -1;

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
 * @req:  request struct
 * @url:  url to send request to
 */
CURLcode requests_get(req_t *req, char *url)
{
    CURLcode rc;
    char *ua = user_agent();
    req->url = url;
    long code;
    CURL *curl = req->curlhandle;


    common_opt(req);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, resp_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, req);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, ua);
    rc = curl_easy_perform(curl);
    if (rc != CURLE_OK)
        return rc;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &code);

    req->code = code;
    req->ok = check_ok(code);
    free(ua);

    return rc;
}

/*
 * requests_get_headers - Performs GET request (same as above) but allows
 * custom headers.
 *
 * Returns the CURLcode return code provided from curl_easy_perform. CURLE_OK
 * is returned on success.
 *
 * @req:  request struct
 * @url:  url to send request to
 * @custom_hdrv: char* array of custom headers
 * @custom_hdrc: length of `custom_hdrv`
 */
CURLcode requests_get_headers(req_t *req, char *url, 
                              char **custom_hdrv, int custom_hdrc)
{
    CURLcode rc;
    struct curl_slist *slist = NULL;
    char *ua = user_agent();
    req->url = url;
    long code;
    CURL *curl = req->curlhandle;

    /* headers */
    if (custom_hdrv != NULL) {
        rc = process_custom_headers(&slist, req, custom_hdrv, custom_hdrc);
        if (rc != CURLE_OK)
            return rc;
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, slist);
    }

    common_opt(req);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, resp_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, req);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, ua);
    rc = curl_easy_perform(curl);
    if (rc != CURLE_OK)
        return rc;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &code);

    req->code = code;
    if (slist != NULL)
        curl_slist_free_all(slist);
    req->ok = check_ok(code);
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
 * unsuccessful. Returned pointer must be free'd with `curl_free`
 *
 * @req:  request struct
 * @data:      char* array as described above
 * @data-size: length of array
 */
char *requests_url_encode(req_t *req, char **data, int data_size)
{
    char *key, *val, *tmp;
    int offset;
    size_t term_size;
    size_t tmp_len;
    CURL *curl = req->curlhandle;

    if (data_size % 2 != 0)
        return NULL;

    /* loop through and get total sum of lengths */
    size_t total_size = 0;
    for (int i = 0; i < data_size; i++) {
        tmp = data[i];
        tmp_len = strlen(tmp);
        total_size += tmp_len;
    }

    char encoded[total_size]; /* clear junk bytes */
    snprintf(encoded, total_size, "%s", "");

    /* loop in groups of two, assembling key/val pairs */
    for (int i = 0; i < data_size; i+=2) {
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

CURLcode requests_post(req_t *req, char *url, char *data)
{
    return requests_pt(req, url, data, NULL, 0, 0);
}

CURLcode requests_put(req_t *req, char *url, char *data)
{
    return requests_pt(req, url, data, NULL, 0, 1);
}

CURLcode requests_post_headers(req_t *req, char *url, char *data,
                               char **custom_hdrv, int custom_hdrc)
{
    return requests_pt(req, url, data, custom_hdrv, custom_hdrc, 0);
}

CURLcode requests_put_headers(req_t *req, char *url, char *data,
                              char **custom_hdrv, int custom_hdrc)
{
    return requests_pt(req, url, data, custom_hdrv, custom_hdrc, 1);
}

/*
 * requests_pt - Performs POST or PUT request using supplied data and populates
 * req struct text member with request response, code with response code, and
 * size with size of response. To submit no data, use NULL for data, and 0 for
 * data_size.
 *
 * Returns CURLcode provided from curl_easy_perform. CURLE_OK is returned on
 * success. -1 returned if there are issues with libcurl's internal linked list
 * append.
 *
 * Typically this function isn't used directly, use requests_post() or
 * requests_put() instead.
 *
 * @req: request struct
 * @url: url to send request to
 * @data: url encoded data to send in request body
 * @custom_hdrv: char* array of custom headers
 * @custom_hdrc: length of `custom_hdrv`
 * @put_flag: if not zero, sends PUT request, otherwise uses POST
 */
static CURLcode requests_pt(req_t *req, char *url, char *data,
                            char **custom_hdrv, int custom_hdrc, int put_flag)
{
    CURLcode rc;
    struct curl_slist *slist = NULL;
    req->url = url;
    CURL *curl = req->curlhandle;

    /* body data */
    if (data != NULL) {
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data);
    } else {
        /* content length header defaults to -1, which causes request to fail
           sometimes, so we need to manually set it to 0 */
        char *cl_header = "Content-Length: 0";
        slist = curl_slist_append(slist, cl_header);
        if (slist == NULL)
            return (CURLcode) -1;
        if (custom_hdrv == NULL)
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, slist);

        hdrv_append(&req->req_hdrv, &req->req_hdrc, cl_header);
    }

    /* headers */
    if (custom_hdrv != NULL) {
        rc = process_custom_headers(&slist, req, custom_hdrv, custom_hdrc);
        if (rc != CURLE_OK)
            return rc;
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, slist);
    }

    common_opt(req);
    if (put_flag)
        /* use custom request instead of dedicated PUT, because dedicated
           PUT doesn't work with arbitrary request body data */
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
    else
        curl_easy_setopt(curl, CURLOPT_POST, 1);
    char *ua = user_agent();
    curl_easy_setopt(curl, CURLOPT_USERAGENT, ua);
    rc = curl_easy_perform(curl);
    if (rc != CURLE_OK)
        return rc;

    long code;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &code);
    req->code = code;
    req->ok = check_ok(code);

    if (slist != NULL)
        curl_slist_free_all(slist);
    free(ua);

    return rc;
}

/*
 * process_custom_headers - Adds custom headers to request and populates the
 * req_headerv and req_hdrc fields of the request struct using the supplied
 * custom headers.
 *
 * Returns CURLE_OK on success, CURLE_OUT_OF_MEMORY if realloc failed to
 * increase size of req_hdrv, or -1 if libcurl's linked list append fails.
 *
 * @slist: internal libcurl slist (string linked list)
 * @req: request struct
 * @custom_hdrv: char* array of custom headers
 * @custom_hdrc: length of `custom_hdrv`
 */
static CURLcode process_custom_headers(struct curl_slist **slist, req_t *req,
                                       char **custom_hdrv, int custom_hdrc)
{
    for (int i = 0; i < custom_hdrc; i++) {
        /* add header to request */
        *slist = curl_slist_append(*slist, custom_hdrv[i]);
        if (*slist == NULL)
            return (CURLcode) -1;
        if (hdrv_append(&req->req_hdrv, &req->req_hdrc, custom_hdrv[i]))
            return CURLE_OUT_OF_MEMORY;
    }

    return CURLE_OK;
}

/*
 * hdrv_append - Appends to an arbitrary char* array and increments the given
 * array length.
 *
 * Returns 0 on success and -1 on memory error.
 *
 * @hdrv: pointer to the char* array
 * @hdrc: length of `hdrv' (NOTE: this value gets updated)
 * @_new: char* to append
 */
static int hdrv_append(char ***hdrv, int *hdrc, char *_new)
{
    /* current array size in bytes */
    size_t current_size = *hdrc * sizeof(char*);
    char *newdup = strndup(_new, strlen(_new));
    if (newdup == NULL)
        return -1;

    *hdrv = (char**) realloc(*hdrv, current_size + sizeof(char*));
    if (*hdrv == NULL)
        return -1;
    (*hdrc)++;
    (*hdrv)[*hdrc - 1] = newdup;
    return 0;
}

/*
 * common_opt - Sets common libcurl options.
 *
 * @curl: libcurl handle
 * @req: request struct
 */
static void common_opt(req_t *req)
{
    CURL *curl = req->curlhandle;
    curl_easy_setopt(curl, CURLOPT_URL, req->url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, resp_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, req);
    curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, header_callback);
    curl_easy_setopt(curl, CURLOPT_HEADERDATA, req);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
}

/*
 * user_agent - Creates custom user agent.
 *
 * Returns a char* containing the user agent, or NULL on failure.
 */
static char *user_agent(void)
{
    struct utsname name;
    uname(&name);
    char *kernel = name.sysname;
    char *version = name.release;

    const char* fmt = "librequests/%s %s/%s";
    size_t len = snprintf(NULL, 0, fmt, __LIBREQ_VERS__, kernel, version);

    char *ua = malloc(len + 1);
    snprintf(ua, len + 1, fmt, __LIBREQ_VERS__, kernel, version);

    return ua;
}

/*
 * check_ok - Utility function for setting "ok" struct field. Response codes
 * of 400+ are considered "not ok".
 *
 * @req: request struct
 */
static int check_ok(long code)
{
    if (code >= 400 || code == 0)
        return 0;
    else
        return 1;
}
