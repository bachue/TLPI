include ../Makefile.inc

GEN_EXE = svsem_create svsem_demo svsem_mon svsem_op svsem_rm svsem_setall test_fork_sync test_binary_sems_via_fifo

LINUX_EXE = svsem_info

EXE = ${GEN_EXE} ${LINUX_EXE}

all : ${EXE}

allgen : ${GEN_EXE}

test_binary_sems_via_fifo: test_binary_sems_via_fifo.c binary_sems_via_fifo.c binary_sems_via_fifo.h
	${CC} ${CFLAGS} binary_sems_via_fifo.c test_binary_sems_via_fifo.c ${LDLIBS} -lm -o test_binary_sems_via_fifo

clean : 
	${RM} ${EXE} *.o

showall :
	@ echo ${EXE}

${EXE} : ${LPLIB}		# True as a rough approximation
