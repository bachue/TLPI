include ../Makefile.inc

SRC = mod1.c mod2.c mod3.c

libmod.so: ${SRC}
	${CC} -g -fPIC ${CFLAGS} -Wl,-Bsymbolic,-soname,libmod.so.1 ${SRC} -shared -o libmod.so.1.0.1
	ldconfig -nv .
	ln -s libmod.so.1 libmod.so

main.o: main.c
	${CC} -c main.c ${CFLAGS}

all : libmod.so main.o
	${CC} -g -Wall -Wl,-rpath,'$$ORIGIN' -o main main.o -L. -lmod

clean :
	${RM} *.o *.a *.so *.so.* main

showall :
	@ echo ${MOD} libmod.a main
