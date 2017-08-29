CC=gcc
CFLAGS=-I.
LIBS=-lm -lSDL2 -lSDL2_image
DEPS=engine.h
OBJ=chess.o engine.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)
chess: $(OBJ)
	gcc -o $@ $^ $(CFLAGS) $(LIBS)
clean:
	rm -rf *.o
.PHONY: clean
