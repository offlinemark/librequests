ifdef DEBUG
	FLAGS = -Wall -g -D _DEBUG_
else
	FLAGS = -Wall -g
endif

BIN = test
SRC = $(wildcard *.c)
BUILD = ../build

test: $(SRC)
	$(CC) -o $(BIN) $(SRC) $(FLAGS) -L $(BUILD) -l requests -l curl
	@echo
	@echo Compiled Tests.

clean:
	rm $(BIN)
	rm -rf $(BIN).dSYM
