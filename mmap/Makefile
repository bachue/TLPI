include ../Makefile.inc

GEN_EXE = anon_mmap mmcat mmcopy t_mmap test_sync test_memory_share_reader test_memory_share_writer test_out_of_file

LINUX_EXE = t_remap_file_pages test_remap_file_pages_via_fixed

EXE = ${GEN_EXE} ${LINUX_EXE}

all : ${EXE}

allgen : ${GEN_EXE}

clean :
	${RM} ${EXE} *.o

showall :
	@ echo ${EXE}

${EXE} : ${LPLIB}		# True as a rough approximation
