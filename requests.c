/*
 * C-Requests Library
 * Mark Mossberg, 2014
 * mark.mossberg@gmail.com
 */

#include "requests.h"

CURL *requests_init(REQ *req, char *url)
{
    req->code = 0;
    req->url = url;
    req->text = calloc(1, 1);
    req->size = 0;

    return curl_easy_init();
}

void requests_close(CURL *curl, REQ *req)
{
    curl_easy_cleanup(curl);
    free(req->text);
}

size_t callback(char *content, size_t size, size_t nmemb, REQ *userdata)
{
    size_t real_size = size * nmemb;

    userdata->text = realloc(userdata->text, userdata->size + real_size + 1); // null byte!
    if (userdata->text == NULL)
    {
        printf("ERROR: Memory allocation failed.\n");
        exit(1);
    }

    userdata->size += real_size;

    char *responsetext = strndup(content, size * nmemb);
    if (responsetext == NULL)
    {
        printf("ERROR: Memory allocation failed.\n");
        exit(1);
    }
    strcat(userdata->text, responsetext);

    free(responsetext);
    return size * nmemb;
}

void requests_get(CURL *curl, REQ *req)
{
    if (req->url == NULL) {
        printf("ERROR: No URL provided.\n");
        exit(1);
    }

    long code = 0;

    curl_easy_setopt(curl, CURLOPT_URL, req->url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, req);
    curl_easy_perform(curl);
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &code);

    req->code = code;
}

char *post_encode(CURL *curl, char **data, int data_size)
{

    // loop through and get total sum of lengths
    size_t total_size = 0;
    int i = 0;
    for (i = 0; i < data_size; i++) {
        char *tmp = data[i];
        size_t tmp_len = strlen(tmp);
        total_size += tmp_len;
    }

    char encoded[total_size]; // clear junk bytes
    snprintf(encoded, total_size, "%s", "");

    // loop in groups of two, assembling key/val pairs
    for (i = 0; i < data_size; i+=2) {
        char *key = data[i];
        char *val = data[i+1];
        int offset = i == 0 ? 2 : 3; // =, \0 and maybe &
        size_t term_size = strlen(key) + strlen(val) + offset;
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

void requests_post(CURL *curl, REQ *req, char **data, int data_size)
{
    if (data_size % 2 != 0)
    {
        printf("ERROR: Data size must be even");
        exit(1);
    }

    char *encoded = post_encode(curl, data, data_size);

    curl_free(encoded);
}
