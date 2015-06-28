include ../Makefile.inc

GEN_EXE = demo_sigio poll_pipes select_mq self_pipe t_select select_pipes

LINUX_EXE = epoll_input is_id_echo_sv

EXE = ${GEN_EXE} ${LINUX_EXE}

all : ${EXE}

allgen : ${GEN_EXE}

clean :
	${RM} ${EXE} *.o

showall :
	@ echo ${EXE}

${EXE} : ${LPLIB}		# True as a rough approximation

is_id_echo_sv: ../sockets/inet_sockets.h
