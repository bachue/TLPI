include ../Makefile.inc

GEN_EXE = mq_notify_sig mq_notify_sigwaitinfo mq_notify_thread \
	  mq_notify_via_signal mq_notify_via_thread \
	  pmsg_create pmsg_getattr pmsg_receive pmsg_send pmsg_unlink \
	  test_pmsg_timedreceive mq_seqnum_client mq_seqnum_server

LINUX_EXE =

EXE = ${GEN_EXE} ${LINUX_EXE}

all : ${EXE}

allgen : ${GEN_EXE}

LDLIBS = ${IMPL_LDLIBS} ${LINUX_LIBRT}
	# All of the programs in this directory need the
	# realtime library, librt.

mq_seqnum_server: mq_seqnum_server.c mq_seqnum.c
	${CC} ${CFLAGS} mq_seqnum.c mq_seqnum_server.c ${LDLIBS} -o mq_seqnum_server

mq_seqnum_client: mq_seqnum_client.c mq_seqnum.c
	${CC} ${CFLAGS} mq_seqnum.c mq_seqnum_client.c ${LDLIBS} -o mq_seqnum_client

clean :
	${RM} ${EXE} *.o

showall :
	@ echo ${EXE}

${EXE} : ${LPLIB}		# True as a rough approximation
