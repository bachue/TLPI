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
#include <fcntl.h>
#include "tlpi_hdr.h"

int
main(int argc, char *argv[])
{
    int inputFd, i;

    for (i = 0; i < 5; ++i) {
        inputFd = open(argv[1], O_RDONLY);
        printf("%d: %d\n", i, inputFd);

        if (inputFd == -1)
            errExit("opening file %s", argv[1]);

        if (close(inputFd) == -1)
            errExit("close input");
    }

    exit(EXIT_SUCCESS);
}
