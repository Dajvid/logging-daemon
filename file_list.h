#ifndef FILE_LIST_H
#define FILE_LIST_H

#include "common.h"
#include <unistd.h>

struct file_list {
    FILE **files;
    int count;
};

RETCODE init_file_list(struct file_list *file_list, int size);

void free_file_list(struct file_list files);

void add_file_to_file_list(char *path, struct file_list *file_list);

#endif //FILE_LIST_H
