#include <errno.h>
#include <signal.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

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

static int talk(FILE *stream) {
    const int interactive = isatty(fileno(stream));
    struct sigaction action, old_action;

    if (interactive) {
        action.sa_handler = signal_handler;
        sigemptyset(&action.sa_mask);
        action.sa_flags = 0;

        sigaction(SIGINT, &action, &old_action);
    }

    while (!feof(stream)) {
        char buf[65536];
        char *p;

        if (g_interrupted > 1) {
            shutdown(1, 128 + SIGINT);
        }
        else if (g_interrupted == 1) {
            shush();
        }

        p = fgets(buf, sizeof(buf), stream);
        if (NULL == p) {
            if (ferror(stream)) {
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

    if (interactive) {
        sigaction(SIGINT, &old_action, NULL);
    }

    return 0;
}

int main (int argc, const char **argv) {
    int r = 0;

    if (argc > 1) {
        for (int i = 1; i < argc; i++) {
            if (0 == strcmp(argv[i], "-")) {
                r = talk(stdin);
            }
            else {
                FILE *f = fopen(argv[i], "r");

                if (NULL == f) {
                    perror(argv[i]);
                    r = errno;
                    break;
                }

                r = talk(f);
                fclose(f);
            }
        }
    }
    else {
        r = talk(stdin);
    }

    shutdown(0, r);
}
