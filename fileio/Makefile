include ../Makefile.inc

GEN_EXE = atomic_append bad_exclusive_open copy \
	multi_descriptors seek_io t_readv t_truncate read-test open-test tee-test copy-hole read-by-two-descriptors truncate t_readv_writev

LINUX_EXE = large_file

EXE = ${GEN_EXE} ${LINUX_EXE}

all : ${EXE}

allgen : ${GEN_EXE}

clean : 
	${RM} ${EXE} *.o

showall :
	@ echo ${EXE}

${EXE} : ${LPLIB}		# True as a rough approximation
