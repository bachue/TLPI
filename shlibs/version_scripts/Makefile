include ../../Makefile.inc

sv_lib_v1.o: sv_lib_v1.c
	${CC} -g -c -fPIC -Wall sv_lib_v1.c

libsv1.so: sv_lib_v1.o sv_v1.map
	${CC} -g -shared -o libsv1.so sv_lib_v1.o -Wl,--version-script,sv_v1.map

p1: sv_prog.c libsv1.so
	${CC} -g -o p1 sv_prog.c libsv1.so

sv_lib_v2.o: sv_lib_v2.c
	${CC} -g -c -fPIC -Wall sv_lib_v2.c

libsv2.so: sv_lib_v2.o sv_v2.map
	${CC} -g -shared -o libsv2.so sv_lib_v2.o -Wl,--version-script,sv_v2.map

p2: sv_prog.c libsv2.so
	${CC} -g -o p2 sv_prog.c libsv2.so

clean:
	rm -f p1 p2 *.o *.so

all: clean p1 p2
