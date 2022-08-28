CC=gcc
CFLAGS=-I . -I view/ -I model/ -I core/
PROGRAM=en_typing

${PROGRAM}: en_typing.o view/*.o model/*.o core/*.o
	${CC} ${CFLAGS} -o ${PROGRAM} en_typing.o view/*.o model/*.o core/*.o -l ncursesw

.c.o: en_typing.c view/*.c model/*.c core/*.c
	cd view && make
	cd model && make
	cd core && make
	${CC} ${CFLAGS} -c en_typing.c

all: ${PROGRAM}

clean:
	cd view && make clean
	cd model && make clean
	cd core && make clean
	rm *.o ${PROGRAM}
