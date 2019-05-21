#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/ioctl.h>
#include <string.h>
#include<signal.h>
#include "common.h"
#include "file_list.h"
#include "htable.h"

#define SOCK_PATH "/dev/log"

#define IF_RET(cond, ret){if (cond) return ret;}

RETCODE create_socket(int *fd);

void free_socket(int *fd);
