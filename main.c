#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <time.h>

#include "speak.h"

static void shutdown(int immediately, int code) {
    unload(immediately);

    exit(code);
}

static void interrupt(int pacifier) {
    shush();
}

int main (int argc, const char **argv) {
    signal(SIGINT, interrupt);

    while (1) {
        char buf[1024];
        char *p;

        p = fgets(buf, sizeof(buf), stdin);
        if (NULL == p) break;

        speak(buf, strlen(buf));
    }

    shutdown(0, 0);
}
