#include "logging_deamon.h"

htable table;
int fd = -1;
struct file_list file_list;

RETCODE
create_socket(int *fd)
{
    struct sockaddr_un sun;
    memset(&sun, 0, sizeof(sun));
    sun.sun_family = AF_UNIX;
    strncpy(sun.sun_path, SOCK_PATH, sizeof(sun.sun_path));
    sun.sun_path[sizeof(sun.sun_path)-1] = '\0';
    unlink(SOCK_PATH);

    *fd = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (bind(*fd, (struct sockaddr *)&sun, SUN_LEN(&sun)) < 0) {
        free_socket(fd);
        fprintf(stderr, "ERROR: unable to bind socket\n");
        return SOCK_ERR;
    }

    if (chmod(SOCK_PATH, 0666) < 0) {
        free_socket(fd);
        fprintf(stderr, "ERROR: unable to change socket permission\n");
        return SOCK_ERR;
    }

    return SUCCES;
}

void
free_socket(int *fd)
{
    if (*fd != -1) {
        close(*fd);
        *fd = -1;
    }

    unlink(SOCK_PATH);
}

RETCODE
load_message(int fd, char **msg)
{
    size_t len = 0;
    IF_RET(fd < 0, SOCK_ERR);
    ioctl(fd, FIONREAD, &len);
    char tmp[len + 1];

    if (len != 0) {
        read(fd, tmp, len);
        *msg = malloc(sizeof(*msg) * strlen(tmp) + 1);
        strncpy(*msg, tmp, len);
        msg[len] = '\0';
    } else {
        *msg = NULL;
    }

    return SUCCES;
}

RETCODE
parse_args(int argc, char **argv, bool *fork, struct file_list *file_list)
{
    IF_RET(init_file_list(file_list, argc) != SUCCES, INT_ERR);

    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-' && argv[i][1] == 'f' && argv[i][2] == '\0') {
            *fork = true;
        } else {
            add_file_to_file_list(argv[i], file_list);
        }
    }

    if (file_list->count == 0) {
        fprintf(stderr, "No usable files, exiting.\n");
        return NO_FILE_ERR;
    }

    return SUCCES;
}

RETCODE
save_message(char *msg, struct file_list file_list)
{
    int saved = 0;
    int msg_len = strlen(msg);

    for (int i = 0; i < file_list.count; i++) {
        saved = fprintf(file_list.files[i], "%s\n", msg);
        if (saved != msg_len + 1) {
            fprintf(stderr, "WARNING: Unable to save whole message into file");
        }
    }
    printf("%s\n", msg);
    return SUCCES;
}

void
free_resources()
{
    char *msg = htable_get_most_frequented(&table);
    if (msg) {
        printf("---most frequented message---\n%s\n", msg);
    }
    htable_free(&table);
    free_socket(&fd);
    free_file_list(file_list);

    exit(SUCCES);
}

char *
remove_prefix(char *msg)
{
    int double_dot_counter = 0;
    while(msg[0] != '\0') {
        if (msg[0] == ':') {
            double_dot_counter++;
        }
        msg = &msg[1];
        if (double_dot_counter == 3) {
            msg = &msg[1];
            break;
        }
    }

    return msg;
}

int
main(int argc, char **argv)
{
    bool fork = false;
    RETCODE ret = SUCCES;
    char *msg = NULL;
    char *stripped_msg = NULL;

    signal(SIGINT, free_resources);
    signal(SIGKILL, free_resources);

    IF_RET(parse_args(argc, argv, &fork, &file_list) != SUCCES, NO_FILE_ERR);
    IF_RET(create_socket(&fd) != SUCCES, SOCK_ERR);
    htable_init(&table);


    while(load_message(fd, &msg) == SUCCES) {
        if (msg) {
            stripped_msg = remove_prefix(msg);
            if (stripped_msg != NULL) {
                htable_insert(&table, stripped_msg);
            }
            ret = save_message(msg, file_list);
            if (ret != SUCCES) {
                break;
            }
        }
    }

    htable_free(&table);
    free_socket(&fd);
    free_file_list(file_list);
    return EXIT_SUCCESS;
}
