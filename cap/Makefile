include ../Makefile.inc

GEN_EXE =

LINUX_EXE = check_password_caps test_cap get_cap get_cap_and_exec cap_sched_set

EXE = ${GEN_EXE} ${LINUX_EXE}

all : ${EXE}

allgen : ${GEN_EXE}

LDLIBS = ${IMPL_LDLIBS} ${LINUX_LIBCAP} ${LINUX_LIBCRYPT}
        # The only program we build here needs libcap and libcrypt

clean :
	${RM} ${EXE} *.o

showall :
	@ echo ${EXE}

${EXE} : ${LPLIB}		# True as a rough approximation
