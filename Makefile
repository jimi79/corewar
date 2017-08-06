BIN=./bin
SOURCE=./
CFLAGS?=-ggdb -pthread
LIBS=lib_red.c 
MKDIR_P=mkdir -p 
LIST=$(BIN)/red_poll $(BIN)/red_duelling $(BIN)/red_engine $(BIN)/red_test 
all: $(LIST)

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

$(BIN)/%: $(SOURCE)%.c
	${MKDIR_P} ${BIN}
	$(CC) $(INC) $< $(CFLAGS) -o $@ $(LIBS)

clean:
	rm -f $(LIST)

re: clean all
