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

void
add_file_to_file_list(char *path, struct file_list *file_list)
{
    if (access(path, F_OK)) {
        fprintf(stderr, "WARNING: file %s doesn't exist\n", path);
    } else {
        file_list->files[file_list->count] = fopen(path, "a");
        if (!file_list->files[file_list->count]) {
            fprintf(stderr, "WARNING: unable to open file %s for writing\n", path);
        } else {
            setvbuf(file_list->files[file_list->count], NULL, _IONBF, 0);
            file_list->count++;
        }
    }
}
