#include "common.h"

struct file_list {
    FILE **files;
    int count;
};

RETCODE
init_file_list(struct file_list *file_list, int size);

void
free_file_list(struct file_list files);
