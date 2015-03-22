include ../Makefile.inc

GEN_EXE = catch_SIGHUP disc_SIGHUP job_mon \
	orphaned_pgrp_SIGHUP handling_SIGTSTP t_setsid test_ctermid test_catch_SIGCONT_and_SIGHUP \
	test_set_pgid_before_exec test_set_sid_for_process_group_leader

EXE = ${GEN_EXE} ${LINUX_EXE}

all : ${EXE}

allgen : ${GEN_EXE}

clean : 
	${RM} ${EXE} *.o

showall :
	@ echo ${EXE}

${EXE} : ${LPLIB}		# True as a rough approximation
