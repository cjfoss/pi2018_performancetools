# Fire Serial - Makefile for Blue Waters
# Author: Aaron Weeden, Shodor, 2015

CC=cc
CFLAGS=
LIBS=-lm
SRC=fire-serial.c
EXECUTABLE=fire-serial

$(EXECUTABLE): $(SRC)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

clean:
	rm -f $(EXECUTABLE)

