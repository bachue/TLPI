include ../Makefile.inc

GEN_EXE = bad_symlink file_type_stats list_files list_files_readdir_r \
	nftw_dir_tree t_dirbasename t_unlink view_symlink test_remove test_realpath test_wrong_chmod

LINUX_EXE =

EXE = ${GEN_EXE} ${LINUX_EXE}

all : ${EXE}

allgen : ${GEN_EXE}

clean : 
	${RM} ${EXE} *.o

showall :
	@ echo ${EXE}

${EXE} : ${LPLIB}		# True as a rough approximation
