#include <stdio.h>
#include "requests.h"
#include "greatest.h"

void test_print(REQ *req)
{
    printf("Request URL: %s\n", req->url);
    printf("Response Code: %lu\n", req->code);
    printf("Response Size: %zu\n", req->size);
    printf("Response Body:\n%s\n", req->text);
}

TEST get()
{
    long code = 200;
    size_t size = 1270;

    REQ req;
    CURL *curl = requests_init(&req, "http://example.com");
    requests_get(curl, &req);

    ASSERT_EQ(code, req.code);
    ASSERT_EQ(size, req.size);

    requests_close(curl, &req);
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

    REQ req;
    CURL *curl = requests_init(&req, "http://www.posttestserver.com/post.php");
    requests_post(curl, &req, data, data_size);

    ASSERT_EQ(code, req.code);

    requests_close(curl, &req);
    PASS();
}

TEST post_nodata()
{
    long code = 200;

    REQ req;
    CURL *curl = requests_init(&req, "http://www.posttestserver.com/post.php");
    requests_post(curl, &req, NULL, 0);

    ASSERT_EQ(code, req.code);

    requests_close(curl, &req);
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

    REQ req;
    CURL *curl = requests_init(&req, "http://www.posttestserver.com/post.php");

    requests_put(curl, &req, data, data_size);
    test_print(&req);

    ASSERT_EQ(code, req.code);

    requests_close(curl, &req);
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

    char *test = url_encode(curl, data, data_size);

    ASSERT(!strcmp(ideal, test));

    PASS();
}

SUITE(tests)
{
    RUN_TEST(get);
    RUN_TEST(post);
    RUN_TEST(post_nodata);
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
