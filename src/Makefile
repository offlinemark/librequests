FLAGS = -Wall -g -I ../include -std=gnu99
BIN = test
SRC = $(wildcard *.c)
NAME = requests
BUILD = ../build

lib: lib$(NAME).a

lib$(NAME).a: $(SRC)
	$(CC) -c $(SRC) $(FLAGS)
	ar -cvq $@ *.o
	rm *.o
	mv $@ $(BUILD)
	@echo
	@echo Build Succeeded!

clean:
	rm *.a
	rm *.o
