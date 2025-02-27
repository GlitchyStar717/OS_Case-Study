#define _XOPEN_SOURCE 600
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

static void fail_io(const char *msg);

const char *program_name;

int main(int argc, char *argv[]) {
    const char *socket_name;
    struct sockaddr_un sun;
    int sock;
    pid_t pid;

    if (argc < 3) {
        fprintf(stderr, "usage: %s SOCKET COMMAND [ARG]...\n", argv[0]);
        return EXIT_FAILURE;
    }

    program_name = argv[0];
    socket_name = argv[1];

    /* Create socket. */
    sock = socket(PF_LOCAL, SOCK_STREAM, 0);
    if (sock < 0)
        fail_io("socket");

    /* Configure socket. */
    sun.sun_family = AF_LOCAL;
    strncpy(sun.sun_path, socket_name, sizeof sun.sun_path);
    sun.sun_path[sizeof sun.sun_path - 1] = '\0';

    /* Connect. */
    if (connect(sock, (struct sockaddr *) &sun, sizeof sun) < 0)
        fail_io("connect");

    /* Fork. */
    pid = fork();
    if (pid < 0)
        fail_io("fork");
    else if (pid != 0) {
        /* In parent. */
        char buf[1024];
        int n;

        /* Read from socket, write to stdout. */
        while ((n = read(sock, buf, sizeof buf)) > 0) {
            write(STDOUT_FILENO, buf, n);
        }

        /* Child will inherit file descriptors. */
        close(sock);
        return 0;
    } else {
        /* In child. */
        /* Close stdin. */
        close(STDIN_FILENO);
        
        /* Make socket stdin, stdout, stderr. */
        dup2(sock, STDOUT_FILENO);
        dup2(sock, STDERR_FILENO);
        close(sock);

        /* Execute command. */
        execvp(argv[2], argv + 2);
        fail_io("exec");
        return 1;
    }
}

static void fail_io(const char *msg) {
    fprintf(stderr, "%s: %s: %s\n", program_name, msg, strerror(errno));
    exit(EXIT_FAILURE);
}
