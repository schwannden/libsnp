CFLAGS=-g

all:  np_lib.o error_function.o

error_function.o: error_functions.c
	gcc ${CFLAGS} -c error_functions.c

np_lib.o: np_lib.c
	gcc ${CFLAGS} -c np_lib.c

clean:
	rm *.o
