/*
 * Mark Mossberg, 2014
 * mark.mossberg@gmail.com
 */

#include "requests.h"

size_t callback(char *content, size_t size, size_t nmemb, REQ *userdata)
{
    size_t real_size = size * nmemb;

    userdata->text = realloc(userdata->text, userdata->size + real_size + 1); // null byte!
    if (userdata->text == NULL)
        printf("ERROR: Memory allocation failed.\n");

    userdata->size += real_size;

    char *responsetext = strndup(content, size * nmemb);
    if (responsetext == NULL)
        printf("ERROR: Memory allocation failed.\n");
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

void requests_close(CURL *curl, REQ *req)
{
    curl_easy_cleanup(curl);
    free(req->text);
}

CURL *requests_init(REQ *req, char *url)
{
    req->code = 0;
    req->url = url;
    req->text = calloc(1, 1);
    req->size = 0;

    return curl_easy_init();
}
