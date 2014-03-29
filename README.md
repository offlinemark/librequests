# librequests

Just because you're writing in C, doesn't mean everything has to be painful.

librequests is a wrapper library over [libcurl](http://curl.haxx.se/libcurl/)
which attempts to simplify submitting simple HTTP requests. It is influenced,
to a certain degree, by [python-requests](http://python-requests.org).

*Disclaimer: I'm learning C through this project, so it's entirely possible
that this code uses bad practices, is incorrect, etc. If you find something,
do [let me know](http://github.com/markmossberg/librequests/issues?page=1&state=open).*

## building

Simply run

```bash
$ make
```

This will create a `build/` directory which contains, `librequests.a`, a
static C library that you can compile your own code against.

## example

```c
#include <stdio.h>
#include "requests.h"

int main(int argc, const char *argv[])
{
    REQ req; // declare struct used to store data
    CURL *curl = requests_init(&req); // setup

    requests_get(curl, &req, "http://example.com"); // submit GET request
    printf("Request URL: %s\n", req.url);
    printf("Response Code: %lu\n", req.code);
    printf("Response Size: %zu\n", req.size);
    printf("Response Body:\n%s", req.text);

    requests_close(curl, &req); // clean up
    return 0;
}
```

Save the above as `get.c`, and make sure `requests.h` (located in `include/`)
and `librequests.a` are in the same directory. Also make sure you have gcc or
equivalent and libcurl installed. Then compile it using:

```bash
$ gcc -o get get.c -L . -l requests -l curl
```

And you should see:

```bash
$ ./get
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

For more examples, look in the "examples" directory.

## contribution

Feel free to fork this repo and tackle some of the
[issues](http://github.com/markmossberg/librequests/issues?page=1&state=open).

### tests

librequests uses the [greatest](https://github.com/silentbicycle/greatest) C
unit testing library, written by Scott Vokes. To compile and run tests, just
run

```bash
$ make test
$ ./test/test
```

To compile with debug statements, run

```bash
$ make test-debug
```

### debugging

`test.c` includes one of
[Zed's Awesome Debug Macros](http://c.learncodethehardway.org/book/ex20.html)
for debug statements that can easily be compiled in
or out. In your code, simply put `DEBUG("your message");` where the accepted
parameters are just like the ones for `printf()`.


[![Analytics](https://ga-beacon.appspot.com/UA-36552439-3/librequests/readme)](https://github.com/igrigorik/ga-beacon)
