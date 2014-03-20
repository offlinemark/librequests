# c-requests

Just because you're writing in C, doesn't mean everything has to be painful.

This is a wrapper library over [libcurl](http://curl.haxx.se/libcurl/) which
attempts to simplify submitting simple HTTP requests.

*Disclaimer: I'm learning C through this project, so it's entirely possible
that this code uses bad practices, is incorrect, etc. If you find something,
do [let me know](mailto:mark.mossberg@gmail.com).*

## example

```c
#include <stdio.h>
#include "requests.h"

int main(int argc, const char *argv[])
{
    REQ req; // declare struct used to store data
    CURL *curl = requests_init(&req, "http://example.com"); // setup

    requests_get(curl, &req); // submit GET request
    printf("Request URL: %s\n", req.url);
    printf("Response Code: %lu\n", req.code);
    printf("Response Size: %zu\n", req.size);
    printf("Response Body:\n%s", req.text);

    requests_close(curl, &req); // clean up
    return 0;
}
```

Save the above as `test.c`, and make sure `requests.c` and `requests.h`
are in the same directory. Also make sure you have gcc or equivalent and
libcurl installed. Then compile it using:

```
$ gcc -o test test.c requests.c -l curl
```

And you should see:

```bash
$ ./test
Request URL: http://example.com
Response Code: 200
Response Size: 1270
Response Body:
<!doctype html>
<html>
<head>
    <title>Example Domain</title>
    ...
```
