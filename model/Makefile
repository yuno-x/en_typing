CC=gcc
CFLAGS=-I . -I .. -I ../view -I ../core

all: *.o

.c.o: *.c
	${CC} ${CFLAGS} -c $*.c

clean:
	rm *.o
