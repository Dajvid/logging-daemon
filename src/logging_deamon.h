#ifndef LOGGING_DEAMON_H
#define LOGGING_DEAMON_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/ioctl.h>
#include <string.h>
#include <signal.h>
#include <poll.h>
#include <fcntl.h>
#include "common.h"
#include "file_list.h"
#include "htable.h"

#define SOCK_PATH "/dev/log"

#define IF_RET(cond, ret){if (cond) return ret;}

RETCODE create_socket(int *fd);

void free_socket(int *fd);

RETCODE load_message(int fd, char **msg);

RETCODE parse_args(int argc, char **argv, bool *fork, struct file_list *file_list);

RETCODE save_message(char *msg, struct file_list file_list);

void free_resources();

char *remove_prefix(char *msg);

#endif //LOGGING_DEAMON_H
