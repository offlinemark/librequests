# librequests

Just because you're writing in C, doesn't mean everything has to be painful.

librequests is a wrapper library over [libcurl](http://curl.haxx.se/libcurl/)
which attempts to simplify submitting simple HTTP requests. It is influenced,
to a certain degree, by [python-requests](http://python-requests.org).

## dependencies

Make sure you have libcurl installed. On OS X, use [homebrew](http://brew.sh)
and run `brew install curl`.
On [Ubuntu](http://askubuntu.com/questions/78183/installing-curl-h-library)
`sudo apt-get install libcurl4-openssl-dev` will work.

CMake is also needed for the build.

## getting started

Go to the [releases](https://github.com/mossberg/librequests/releases) page
and download the latest `librequests-[platform].a` file available. This is a
static C library you can compile your own code against. Rename it to
`librequests.a` and take a look at the example below.

Alternatively, you can build librequests yourself (it's pretty easy).

```bash
git clone https://github.com/mossberg/librequests.git
cd librequests
mkdir build && cd build
cmake ..
cmake --build .
```

The build library will be in `build/src`.

## example

```c
#include <stdio.h>
#include "requests.h"

int main(int argc, const char **argv)
{
    req_t req;                     /* declare struct used to store data */
    int ret = requests_init(&req); /* setup */
    if (ret) {
        return 1;
    }

    requests_get(&req, "http://example.com"); /* submit GET request */
    printf("Request URL: %s\n", req.url);
    printf("Response Code: %lu\n", req.code);
    printf("Response Body:\n%s", req.text);

    requests_close(&req); /* clean up */
    return 0;
}
```

Save the above as `get.c`, and make sure `requests.h` (located in `include/`)
and `librequests.a` are in the same directory. Also make sure you have gcc or
equivalent and libcurl installed. Then compile it using:

```bash
$ gcc -o get get.c -L. -lrequests -lcurl
```

And you should see:

```bash
$ ./get
Request URL: http://example.com
Response Code: 200
Response Body:
<!doctype html>
<html>
<head>
    <title>Example Domain</title>
    ...
```

For more examples, look in the "examples" directory.

## documentation

Looking at the examples is probably the easiest way to learn to
use this. That said, there's a few core things that are good to know.

First off, all the good stuff happens with the `req_t` datatype which is just
a special struct that holds all the stuff from the request. Source below.

```
typedef struct {
    long code;        /* Response code */
    char *url;        /* Request URL */
    char *text;       /* Response body */
    size_t size;      /* Body Length */
    char **req_hdrv;  /* Request headers */
    int req_hdrc;     /* Number of request headers */
    char **resp_hdrv; /* Response headers */
    int resp_hdrc;    /* Number of response headers */
    int ok;           /* Bool value. Response codes < 400 are "ok" */
} req_t;
```

At the beginning of every program that uses this library should be two
lines(ish).

```
req_t req;
int ret = requests_init(&req);
if (ret) {
    // perform error handling
}
```

The first line declares the `req_t` struct, and the second line performs
initialization.  At this point you're all set to actually make requests using
the core functions:

```
CURLcode requests_get(req_t *req, char *url);
CURLcode requests_post(req_t *req, char *url, char *data);
CURLcode requests_put(req_t *req, char *url, char *data);
```

The data parameter of POST and PUT needs to already be url-encoded, but
the `requests_url_encode()` function can help you with that.

```
/* array of key-value pairs */
char *data[] = {
    "apple",  "red",
    "banana", "yellow"
};
int data_size = sizeof(data)/sizeof(char*); /* recommended way to get size
                                               of array */
...
char *body = requests_url_encode(&req, data, data_size);
requests_post(&req, "http://www.posttestserver.com/post.php", body);
```

If you want to supply custom request headers, you can use these functions:

```
CURLcode requests_get_headers(req_t *req, char *url, 
                              char **custom_hdrv, int custom_hdrc);
CURLcode requests_post_headers(req_t *req, char *url, char *data,
                               char **custom_hdrv, int custom_hdrc);
CURLcode requests_put_headers(req_t *req, char *url, char *data,
                              char **custom_hdrv, int custom_hdrc);
```

The last two parameters correspond to an array of the headers you want to
provide, and the length of that array, respectively.

Lastly, make sure to call the cleanup functions once you're done. If you used
the url encode function, you'll need to separately `curl_free()` the returned
string, but otherwise, a simple call to `requests_close()` will do.

```
requests_close(&req);
```

## contribution

Feel free to fork this repo and tackle some of the
[issues](http://github.com/markmossberg/librequests/issues?page=1&state=open).

### tests

librequests uses the [greatest](https://github.com/silentbicycle/greatest) C
unit testing library, written by Scott Vokes. To compile and run tests, build
librequests, and within the build directory run:

```bash
$ ./test/test
```

[![Analytics](https://ga-beacon.appspot.com/UA-36552439-3/librequests/readme)](https://github.com/igrigorik/ga-beacon)
