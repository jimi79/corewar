BIN=./bin
SOURCE=./
CFLAGS?=-ggdb
LIBS=lib_red.c 

LIST=$(BIN)/redengine $(BIN)/red_play

all: $(LIST)

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

$(BIN)/%: $(SOURCE)%.c
	$(CC) $(INC) $< $(CFLAGS) -o $@ $(LIBS)

clean:
	rm -f $(LIST)

re: clean all
