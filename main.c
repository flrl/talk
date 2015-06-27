#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <time.h>

#include <ApplicationServices/ApplicationServices.h>

SpeechChannel g_speech_channel = NULL;

static void speak (const char *text, size_t len) {
    OSErr r = noErr;

    if (g_speech_channel) {
        r = SpeakText(g_speech_channel, text, len);
    }
    else {
        fprintf(stderr, "no speech channel?\n");
    }

    if (r) {
        fprintf(stderr, "SpeakText error: %i\n", r);
    }
}

void shutdown(int immediately, int code) {
    if (g_speech_channel) {
        if (!immediately) {
            while(SpeechBusy() > 0) {
                const struct timespec delay = { 0, 2500000000 };
                nanosleep(&delay, NULL);
            }
        }

        StopSpeech(g_speech_channel);
        DisposeSpeechChannel(g_speech_channel);
        g_speech_channel = NULL;
    }

    exit(code);
}

static void interrupt(int pacifier) {
    if (g_speech_channel && SpeechBusy()) {
        StopSpeech(g_speech_channel);
    }
}

int main (int argc, const char **argv) {
    OSErr r = noErr;

    signal(SIGINT, interrupt);

    r = NewSpeechChannel(NULL, &g_speech_channel);
    if (r) exit(r);

    while (1) {
        char buf[1024];
        char *p;

        p = fgets(buf, sizeof(buf), stdin);
        if (NULL == p) break;

        speak(buf, strlen(buf));
    }

    shutdown(0, 0);
}
