CC=gcc
CFLAGS=-I . -I .. -I ../model -I ../core

all: *.o

.c.o: *.c
	${CC} ${CFLAGS} -c $*.c

clean:
	rm *.o
