include ../Makefile.inc

GEN_EXE = pshm_create pshm_read pshm_write pshm_unlink \
	test_pshm_xfr_reader test_pshm_xfr_writer

LINUX_EXE =

EXE = ${GEN_EXE} ${LINUX_EXE}

all : ${EXE}

allgen : ${GEN_EXE}

LDLIBS = ${IMPL_LDLIBS} ${LINUX_LIBRT} ${IMPL_THREAD_FLAGS}
	# All of the programs in this directory need the
	# realtime library, librt.
	# and so require building with '-pthread'. To keep the Makefile
        # simple, we build all of the programs with '-pthread'

clean :
	${RM} ${EXE} *.o

showall :
	@ echo ${EXE}

${EXE} : ${LPLIB}		# True as a rough approximation
