#ifndef BINARY_SEMS_FIFO_H           /* Prevent accidental double inclusion */
#define BINARY_SEMS_FIFO_H

#include "tlpi_hdr.h"

/* Variables controlling operation of functions below */

extern Boolean bsUseSemUndo;            /* Use SEM_UNDO during semop()? */
extern Boolean bsRetryOnEintr;          /* Retry if semop() interrupted by
                                           signal handler? */

int initSemAvailable(char*);

int initSemInUse(char*);

int reserveSem(char*);

int releaseSem(char*);

#endif
