ifdef DEBUG
	FLAGS = -Wall -g -D _DEBUG_ -I ../include
else
	FLAGS = -Wall -g -I ../include
endif

BIN = test
SRC = $(wildcard *.c)
BUILD = ../build

test:
	$(CC) -o $(BIN) $(SRC) $(FLAGS) -L $(BUILD) -l requests -l curl
	@echo
	@echo Compiled Tests.

clean:
	rm $(BIN)
	rm -rf $(BIN).dSYM
