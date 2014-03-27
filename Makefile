FLAGS = -Wall -l curl -g
BIN = test
SRC = $(wildcard *.c)

test: $(SRC)
	$(CC) -o $(BIN) $(SRC) $(FLAGS)

clean:
	rm $(BIN)
	rm -rf $(BIN).dSYM
