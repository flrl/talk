#include <errno.h>
#include <signal.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "speak.h"

volatile sig_atomic_t g_interrupted = 0;

static void shutdown(int immediately, int code) {
    unload(immediately);

    exit(code);
}

static void signal_handler(int signum) {
    int orig_errno = errno;

    switch (signum) {
        case SIGINT:
            g_interrupted++;
            break;

        default:
            break;
    }

    errno = orig_errno;
}

int main (int argc, const char **argv) {
    struct sigaction action;
    action.sa_handler = signal_handler;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;

    sigaction(SIGINT, &action, NULL);

    while (!feof(stdin)) {
        char buf[1024];
        char *p;

        if (g_interrupted > 1) {
            shutdown(1, 128 + SIGINT);
        }
        else if (g_interrupted == 1) {
            shush();
        }

        p = fgets(buf, sizeof(buf), stdin);
        if (NULL == p) {
            if (ferror(stdin)) {
                if (errno != EINTR) {
                    perror("fgets");
                    break;
                }
            }
        }
        else {
            g_interrupted = 0;
            speak(buf, strlen(buf));
        }
    }

    shutdown(0, 0);
}
