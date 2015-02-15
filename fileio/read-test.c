/*************************************************************************\
*                  Copyright (C) Michael Kerrisk, 2014.                   *
*                                                                         *
* This program is free software. You may use, modify, and redistribute it *
* under the terms of the GNU General Public License as published by the   *
* Free Software Foundation, either version 3 or (at your option) any      *
* later version. This program is distributed without any warranty.  See   *
* the file COPYING.gpl-v3 for details.                                    *
\*************************************************************************/

/* Listing 4-1 */

/* copy.c

   Copy the file named argv[1] to a new file named in argv[2].
*/
#include <sys/stat.h>
#include <fcntl.h>
#include "tlpi_hdr.h"

#ifndef BUF_SIZE        /* Allow "cc -D" to override definition */
#define BUF_SIZE 10
#endif

int
main(int argc, char *argv[])
{
    int inputFd;
    ssize_t numRead;
    char buf[BUF_SIZE];

    inputFd = open(argv[1], O_RDONLY);
    if (inputFd == -1)
        errExit("opening file %s", argv[1]);

    while ((numRead = read(inputFd, buf, BUF_SIZE-1)) > 0) {
buf[BUF_SIZE-1]=0;
        printf("%s|", buf);
    }

    if (numRead == -1)
        errExit("read");

    if (close(inputFd) == -1)
        errExit("close input");

    exit(EXIT_SUCCESS);
}
