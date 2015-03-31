include ../Makefile.inc

MOD = mod1.o mod2.o mod3.o

mod1.o: mod1.c
	${CC} -c mod1.c ${CFLAGS}

mod2.o: mod2.c
	${CC} -c mod2.c ${CFLAGS}

mod3.o: mod3.c
	${CC} -c mod3.c ${CFLAGS}

libmod.a: ${MOD}
	ar -r libmod.a ${MOD}

main.o: main.c
	${CC} -c main.c ${CFLAGS}

all : libmod.a main.o
	${CC} -g -o main main.o -L. -lmod

clean :
	${RM} *.o *.a main

showall :
	@ echo ${MOD} libmod.a main
