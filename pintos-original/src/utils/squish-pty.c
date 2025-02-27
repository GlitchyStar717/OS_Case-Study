#define _XOPEN_SOURCE 600
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ioctl.h>
#include <features.h>  /* Added this */

static void fail_io(const char *msg);
static int dead_child(void);

const char *program_name;

int main(int argc, char *argv[]) {
    int master, slave;
    int pipe_fds[2];
    pid_t pid;
    struct termios term;
    program_name = argv[0];

    if (argc < 2) {
        fprintf(stderr, "usage: %s COMMAND [ARG]...\n", program_name);
        return EXIT_FAILURE;
    }

    /* Create pipe for child->parent communication. */
    if (pipe(pipe_fds) < 0)
        fail_io("pipe");

    /* Open master PTY. */
    master = posix_openpt(O_RDWR | O_NOCTTY);
    if (master < 0)
        fail_io("posix_openpt");

    /* Grant access to slave PTY. */
    if (grantpt(master) < 0)
        fail_io("grantpt");

    /* Unlock slave PTY. */
    if (unlockpt(master) < 0)
        fail_io("unlockpt");

    /* Open slave PTY. */
    slave = open(ptsname(master), O_RDWR);
    if (slave < 0)
        fail_io("open");

    /* Get terminal attributes. */
    if (tcgetattr(slave, &term) < 0)
        fail_io("tcgetattr");

    /* Set raw mode. */
    cfmakeraw(&term);
    if (tcsetattr(slave, TCSANOW, &term) < 0)
        fail_io("tcsetattr");

    /* Fork. */
    pid = fork();
    if (pid < 0)
        fail_io("fork");
    else if (pid == 0) {
        /* In child. */
        int null;

        /* Close unused file descriptors. */
        close(master);
        close(pipe_fds[0]);

        /* Arrange to get notified when pipe breaks. */
        fcntl(pipe_fds[1], F_SETFD, FD_CLOEXEC);

        /* Open /dev/null as stdin. */
        null = open("/dev/null", O_RDWR);
        if (null < 0)
            fail_io("open /dev/null");
        dup2(null, STDIN_FILENO);
        close(null);

        /* Make stdout/stderr point to slave PTY. */
        dup2(slave, STDOUT_FILENO);
        dup2(slave, STDERR_FILENO);
        close(slave);

        /* Execute command. */
        execvp(argv[1], argv + 1);
        fail_io("exec");
    }

    /* In parent. */
    close(slave);
    close(pipe_fds[1]);

    /* Copy data from PTY to stdout until EOF or read error. */
    for (;;) {
        char buffer[512];
        int n = read(master, buffer, sizeof buffer);
        if (n <= 0)
            break;
        write(STDOUT_FILENO, buffer, n);
    }

    /* Get exit status of child. */
    return dead_child();
}

static void fail_io(const char *msg) {
    fprintf(stderr, "%s: %s: %s\n", program_name, msg, strerror(errno));
    exit(EXIT_FAILURE);
}

static int dead_child(void) {
    int status;
    wait(&status);
    if (WIFEXITED(status))
        return WEXITSTATUS(status);
    else {
        fprintf(stderr, "%s: child died unexpectedly\n", program_name);
        return EXIT_FAILURE;
    }
}
