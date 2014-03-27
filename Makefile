ifdef DEBUG
	FLAGS = -Wall -l curl -g -D _DEBUG_
else
	FLAGS = -Wall -l curl -g
endif

BIN = test
SRC = $(wildcard *.c)

test: $(SRC)
	$(CC) -o $(BIN) $(SRC) $(FLAGS)

clean:
	rm $(BIN)
	rm -rf $(BIN).dSYM
