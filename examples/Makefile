LIBDIR = ../build
LIBS = -lrequests -lcurl

all: examples

examples: get multi_get post

get: get.c
	$(CC) -o $@ $< -L ../build $(LIBS)

multi_get: multi_get.c
	$(CC) -o $@ $< -L ../build $(LIBS)

post: post.c
	$(CC) -o $@ $< -L ../build $(LIBS)

clean:
	rm get multi_get post

.PHONY: clean
