BIN=./bin
SOURCE=./
CFLAGS?=-ggdb -pthread
LIBS=lib_red.c 

LIST=$(BIN)/red_poll $(BIN)/red_duelling $(BIN)/red_engine

all: $(LIST)

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

$(BIN)/%: $(SOURCE)%.c
	$(CC) $(INC) $< $(CFLAGS) -o $@ $(LIBS)

clean:
	rm -f $(LIST)

re: clean all
