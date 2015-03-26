include ../Makefile.inc

GEN_EXE = t_getpwent t_getpwnam_r

LINUX_EXE = check_password idshow test-change-username my_getpwnam-test test_sudo

EXE = ${GEN_EXE} ${LINUX_EXE}

all : ${EXE}

allgen : ${GEN_EXE}

clean : 
	${RM} ${EXE} *.o

check_password : check_password.o
	${CC} -g -o $@ check_password.o ${LDFLAGS} ${LDLIBS} ${LINUX_LIBCRYPT}

test-change-username: test-change-username.o ugid_functions.o
	${CC} -g -o $@ test-change-username.o ugid_functions.o ${LDFLAGS} ${LDLIBS}

test_sudo: test_sudo.o ugid_functions.o
	${CC} -g -o $@ test_sudo.o ${LDFLAGS} ${LDLIBS} ${LINUX_LIBCRYPT}

showall :
	@ echo ${EXE}

${EXE} : ${LPLIB}		# True as a rough approximation
