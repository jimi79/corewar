BIN=./bin
SOURCE=./
CFLAGS?=-ggdb
LIBS=

LIST=$(BIN)/red

all: $(LIST)

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

$(BIN)/%: $(SOURCE)%.c
	$(CC) $(INC) $< $(CFLAGS) -o $@ $(LIBS)

clean:
	rm -f $(LIST)

re: clean all
