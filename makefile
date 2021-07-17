IDIR = ./include
ODIR = ./build
SDIR = ./src

CC=g++
CFLAGS=-Wall -I$(IDIR)

LDLIBS=-lglfw -lGL

_DEPS = Game.h Ball.h Paddle.h Coords.h
DEPS = $(patsubst %, $(IDIR)/%,$(_DEPS))

_OBJ = Game.o Ball.o Paddle.o
OBJ = $(patsubst %, $(ODIR)/%,$(_OBJ))


$(ODIR)/%.o: $(SDIR)/%.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

shittypong: $(SDIR)/main.cpp $(OBJ)
	$(CC) -o $@ -g $^ $(CFLAGS) $(LDLIBS)

.PHONY: clean

clean: 
	rm -f $(ODIR)/*.o *~ core shittypong
