include ../Makefile.inc

GEN_EXE = ptmr_null_evp ptmr_sigev_signal ptmr_sigev_thread \
	real_timer t_nanosleep timed_read test_alarm test_setitimer test_clock_nanosleep test_ptmr_sigev_signal_2

LINUX_EXE = demo_timerfd t_clock_nanosleep

EXE = ${GEN_EXE} ${LINUX_EXE}

all : ${EXE}

allgen : ${GEN_EXE}

LDLIBS = ${IMPL_LDLIBS} ${LINUX_LIBRT}
	# Many of the programs in this directory need the 
	# realtime library, librt; to keep this Makefile simple,
	# we link *all* of the programs against that library.

clean : 
	${RM} ${EXE} *.o

showall :
	@ echo ${EXE}

${EXE} : ${LPLIB}		# True as a rough approximation
