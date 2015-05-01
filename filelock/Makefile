include ../Makefile.inc

GEN_EXE = i_fcntl_locking t_flock test_fcntl_flock test_f_setlk_perf_1 test_f_setlk_perf_2

EXE = ${GEN_EXE} ${LINUX_EXE}

all : ${EXE}

allgen : ${GEN_EXE}

clean :
	${RM} ${EXE} *.o

showall :
	@ echo ${EXE}

${EXE} : ${LPLIB}		# True as a rough approximation
