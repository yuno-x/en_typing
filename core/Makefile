CC=gcc
CFLAGS=-I . -I .. -I ../view -I ../model

all: *.o

.c.o: *.c
	${CC} ${CFLAGS} -c $*.c

clean:
	rm *.o
