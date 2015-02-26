#include <sys/time.h>
#include <time.h>
#include "tlpi_hdr.h"

int main(int argc, char  *argv[]) {
    struct timeval timev;
    struct timezone zone;
    time_t t;
    struct tm *ptm;
    if (gettimeofday(&timev, &zone) == -1)
        errExit("gettimeofday error");
    if ((t = time(NULL)) == -1)
        errExit("time error");
    printf("sec: %ld, usec: %ld, minuteswest: %d, dsttime: %d\n", (long) timev.tv_sec, (long) timev.tv_usec,
                                                                  zone.tz_minuteswest, zone.tz_dsttime);
    printf("time sec: %ld, format: %s", (long) t, ctime(&t));

    ptm = gmtime(&t);
    printf("tm_sec: %d, tm_min: %d, tm_hour: %d, tm_mday: %d, tm_mon: %d, tm_year: %d, tm_wday: %d, tm_yday: %d, tm_isdst: %d\n", ptm->tm_sec, ptm->tm_min, ptm->tm_hour, ptm->tm_mday, ptm->tm_mon, ptm->tm_year, ptm->tm_wday, ptm->tm_yday, ptm->tm_isdst);
    printf("format: %s", asctime(ptm));
    ptm = localtime(&t);
    printf("tm_sec: %d, tm_min: %d, tm_hour: %d, tm_mday: %d, tm_mon: %d, tm_year: %d, tm_wday: %d, tm_yday: %d, tm_isdst: %d\n", ptm->tm_sec, ptm->tm_min, ptm->tm_hour, ptm->tm_mday, ptm->tm_mon, ptm->tm_year, ptm->tm_wday, ptm->tm_yday, ptm->tm_isdst);
    printf("format: %s", asctime(ptm));
    return 0;
}
