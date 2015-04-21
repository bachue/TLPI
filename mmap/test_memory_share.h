#ifndef TEST_MEMORY_SHARE_H
#define TEST_MEMORY_SHARE_H

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include "tlpi_hdr.h"

#define REQ_PATH "/tmp/req.fifo"
#define RESP_PATH "/tmp/resp.fifo"
#define SHARED_MEM_PATH "/tmp/shared.mem"

#define BUF_SIZE 4096

#endif
