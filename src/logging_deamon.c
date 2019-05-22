#include "logging_deamon.h"

htable table;
struct pollfd pfd = {-1, -1, -1};
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
parse_args(int argc, char **argv, bool *fork_option, struct file_list *file_list)
{
    IF_RET(init_file_list(file_list, argc) != SUCCES, INT_ERR);

    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-' && argv[i][1] == 'f' && argv[i][2] == '\0') {
            *fork_option = true;
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
most_frequented()
{
    struct htable_data *data = htable_get_most_frequented(&table);
    if (data) {
        printf("---most frequented message---\n(%d) %s\n", data->occurence, data->msg);
    }
}

void
free_resources()
{
    htable_free(&table);
    free_socket(&pfd.fd);
    free_file_list(file_list);

    exit(SUCCES);
}

void
exit_logger()
{
    most_frequented();
    free_resources();
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

void
deamonize()
{
    setsid();
    signal(SIGHUP,SIG_IGN);
    pid_t pid = fork();
    if (pid == -1) {
        fprintf(stderr, "ERROR: failed to fork while daemonising");
        exit(DEAM_ERR);
    } else if (pid != 0) {
        _exit(0);
    }

    chdir("/");
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
    if (open("/dev/null",O_RDONLY) == -1) {
        exit(DEAM_ERR);
    }
    if (open("/dev/null",O_WRONLY) == -1) {
        exit(DEAM_ERR);
    }
    if (open("/dev/null",O_RDWR) == -1) {
        exit(DEAM_ERR);
    }
}

int
main(int argc, char **argv)
{
    bool fork_option = false;
    RETCODE ret = SUCCES;
    char *msg = NULL;
    char *stripped_msg = NULL;

    IF_RET(parse_args(argc, argv, &fork_option, &file_list) != SUCCES, NO_FILE_ERR);

    if (fork_option) {
        pid_t pid = fork();
        if (pid == -1) {
            fprintf(stderr, "ERROR: failed to fork while daemonising");
            exit(DEAM_ERR);
        } else if (pid != 0) {
            _exit(0);
        }
        //deamonize forked process
        deamonize();
    }

    signal(SIGINT, exit_logger);
    signal(SIGKILL, exit_logger);

    IF_RET(create_socket(&pfd.fd) != SUCCES, SOCK_ERR);
    htable_init(&table);

    pfd.events = POLLIN;

    while(1) {
        poll(&pfd, 1, -1);
        load_message(pfd.fd, &msg);
        if (msg) {
            printf("message_read");
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

    free_resources();
    return EXIT_SUCCESS;
}
