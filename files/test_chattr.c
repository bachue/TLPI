#include "tlpi_hdr.h"
#include <sys/ioctl.h>
#include <linux/fs.h>
#include <fcntl.h>

static void chattr(const char*, char, char*);
static int attr(char *flags);

int main(int argc, char *argv[]) {
    int i;
    char mode, *flags;
    if (argc <= 2 || (argc > 1 && strcmp(argv[1], "--help") == 0))
        usageErr("%s [+-=][acdeijstuADST] file [files ...]\n", argv[0]);
    mode = argv[1][0];
    flags = argv[1] + 1;

    for (i = 2; i < argc; ++i)
        chattr(argv[i], mode, flags);

    return 0;
}

void chattr(const char *path, char mode, char *flags) {
    int nattr = attr(flags), oattr, fd;

    fd = open(path, O_RDONLY);
    if (fd == -1)
        errExit("open error");

    if (mode != '=' && ioctl(fd, FS_IOC_GETFLAGS, &oattr) == -1)
        errExit("ioctl to get flags");

    switch (mode) {
    case '+': nattr |= oattr; break;
    case '-': nattr = oattr & ~nattr; break;
    case '=': break;
    default:
       fatal("unsupport mode");
    }

    if (ioctl(fd, FS_IOC_SETFLAGS, &nattr) == -1)
        errExit("ioctl to set flags");
}

int attr(char *flags) {
    char *pflag;
    int attr = 0;
    for (pflag = flags; *pflag != '\0'; ++pflag) {
        switch(*pflag) {
        case 'a': attr |= FS_APPEND_FL; break;
        case 'c': attr |= FS_COMPR_FL; break;
        case 'D': attr |= FS_DIRSYNC_FL; break;
        case 'i': attr |= FS_IMMUTABLE_FL; break;
        case 'j': attr |= FS_JOURNAL_DATA_FL; break;
        case 'A': attr |= FS_NOATIME_FL; break;
        case 'd': attr |= FS_NODUMP_FL; break;
        case 't': attr |= FS_NOTAIL_FL; break;
        case 's': attr |= FS_SECRM_FL; break;
        case 'S': attr |= FS_SYNC_FL; break;
        case 'T': attr |= FS_TOPDIR_FL; break;
        case 'u': attr |= FS_UNRM_FL; break;
        default: ;
        }
    }
    return attr;
}
