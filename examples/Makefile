CFLAGS = -L ../build -std=gnu99 -I ../include
LIBDIR = ../build
LIBS = -lrequests -lcurl

all: examples

examples: get multi_get post

get: get.c
	$(CC) -o $@ $< $(CFLAGS) $(LIBS)

multi_get: multi_get.c
	$(CC) -o $@ $< $(CFLAGS) $(LIBS)

post: post.c
	$(CC) -o $@ $< $(CFLAGS) $(LIBS)

clean:
	rm get multi_get post

.PHONY: clean
