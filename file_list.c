#include "file_list.h"

RETCODE
init_file_list(struct file_list *file_list, int size)
{
    file_list->count = 0;
    file_list->files = malloc(sizeof(*file_list->files) * size);

    return (file_list->files) ? SUCCES : INT_ERR;
}

void
free_file_list(struct file_list files)
{
    free(files.files);
}
