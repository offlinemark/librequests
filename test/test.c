#include <stdio.h>
#include "requests.h"
#include "greatest.h"

#ifdef _DEBUG_
# define DEBUG(M, ...) fprintf(stderr, "DEBUG %s:%d: " M "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#else
# define DEBUG(M, ...)
#endif

char *example = "https://gist.githubusercontent.com/mossberg/91093537c33e69e4da2c/raw/librequests_test.txt";
char *example_text = "Simple test file for librequests.";
char *posttestserver = "http://posttestserver.com/post.php";

void test_print(req_t *req)
{
    printf("Request URL: %s\n", req->url);
    printf("Response Code: %lu\n", req->code);
    printf("Response Size: %zu\n", req->size);
    printf("Response Body:\n%s\n", req->text);
}

TEST get()
{
    long code = 200;
    size_t size = 33;

    req_t req;
    CURL *curl = requests_init(&req);
    requests_get(curl, &req, example);

    ASSERT_EQ(code, req.code);
    ASSERT_EQ(size, req.size);
    ASSERT(strcmp(example_text, req.text) == 0);
    ASSERT(strcmp(req.resp_hdrv[0], "HTTP/1.1 200 OK\r\n") == 0);
    ASSERT_EQ(1, req.ok);

    requests_close(&req);
    PASS();
}

TEST get_headers()
{
    long code = 200;
    size_t size = 33;
    char *custom_hdrv[] = {
        "Content-Type: application/json",
        "Content-Hype: dude"
    };
    int headers_size = sizeof(custom_hdrv)/sizeof(char*);

    req_t req;
    CURL *curl = requests_init(&req);
    requests_get_headers(curl, &req, example, custom_hdrv, headers_size);

    ASSERT_EQ(code, req.code);
    ASSERT_EQ(size, req.size);
    ASSERT(strcmp(example_text, req.text) == 0);
    ASSERT(strcmp(req.resp_hdrv[0], "HTTP/1.1 200 OK\r\n") == 0);
    ASSERT(strcmp(req.req_hdrv[0], "Content-Type: application/json") == 0);
    ASSERT_EQ(1, req.ok);
    ASSERT_EQ(2, req.req_hdrc);

    requests_close(&req);
    PASS();
}

TEST post()
{
    long code = 200;
    char *data[] = {
        "apple", "red",
        "banana", "yellow"
    };
    int data_size = sizeof(data)/sizeof(char*);

    req_t req;
    CURL *curl = requests_init(&req);
    char *body = requests_url_encode(curl, data, data_size);
    requests_post(curl, &req, posttestserver, body);

    ASSERT_EQ(code, req.code);
    ASSERT(strcmp(req.resp_hdrv[0], "HTTP/1.1 200 OK\r\n") == 0);
    ASSERT(strstr(req.text, "Successfully") != NULL);
    ASSERT_EQ(1, req.ok);

    curl_free(body);
    requests_close(&req);
    PASS();
}

TEST post_nodata()
{
    long code = 200;

    req_t req;
    CURL *curl = requests_init(&req);
    requests_post(curl, &req, posttestserver, NULL);

    ASSERT_EQ(code, req.code);
    ASSERT(strcmp(req.resp_hdrv[0], "HTTP/1.1 200 OK\r\n") == 0);
    ASSERT(strstr(req.text, "Successfully") != NULL);
    ASSERT_EQ(1, req.ok);

    requests_close(&req);
    PASS();
}

TEST post_headers()
{
    long code = 200;
    char *custom_hdrv[] = {
        "Content-Type: application/json",
        "Content-Hype: dude"
    };
    int headers_size = sizeof(custom_hdrv)/sizeof(char*);

    req_t req;
    CURL *curl = requests_init(&req);
    requests_post_headers(curl, &req, posttestserver, NULL, custom_hdrv,
                          headers_size);

    ASSERT_EQ(code, req.code);
    ASSERT(strcmp(req.resp_hdrv[0], "HTTP/1.1 200 OK\r\n") == 0);
    ASSERT(strcmp(req.req_hdrv[1], "Content-Type: application/json") == 0);
    ASSERT(strstr(req.text, "Successfully") != NULL);
    ASSERT_EQ(1, req.ok);
    ASSERT_EQ(3, req.req_hdrc);

    requests_close(&req);
    PASS();
}

TEST put()
{
    long code = 200;
    char *data[] = {
        "apple", "red",
        "banana", "yellow"
    };
    int data_size = sizeof(data)/sizeof(char*);

    req_t req;
    CURL *curl = requests_init(&req);
    char *body = requests_url_encode(curl, data, data_size);
    requests_put(curl, &req, posttestserver, body);

    ASSERT_EQ(code, req.code);
    ASSERT(strcmp(req.resp_hdrv[0], "HTTP/1.1 200 OK\r\n") == 0);
    ASSERT(strstr(req.text, "Successfully") != NULL);
    ASSERT_EQ(1, req.ok);

    curl_free(body);
    requests_close(&req);
    PASS();
}

TEST urlencode()
{
    CURL *curl = curl_easy_init();
    char *data[] = {
        "apple", "red",
        "banana", "yellow"
    };
    int data_size = sizeof(data)/sizeof(char*);
    char *ideal = "apple%3Dred%26banana%3Dyellow";
    char *test = requests_url_encode(curl, data, data_size);

    ASSERT(strcmp(ideal, test) == 0);

    curl_free(test);
    curl_easy_cleanup(curl);

    PASS();
}

SUITE(tests)
{
    RUN_TEST(get);
    RUN_TEST(get_headers);
    RUN_TEST(post);
    RUN_TEST(post_nodata);
    RUN_TEST(post_headers);
    RUN_TEST(put);
    RUN_TEST(urlencode);
}

GREATEST_MAIN_DEFS();

int main(int argc, char **argv)
{
    GREATEST_MAIN_BEGIN();
    RUN_SUITE(tests);
    GREATEST_MAIN_END();
    return 0;
}
