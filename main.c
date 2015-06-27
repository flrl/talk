#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

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
            // FIXME wait around for current speech to finish
            sleep(2);
        }

        DisposeSpeechChannel(g_speech_channel);
        g_speech_channel = NULL;
    }

    exit(code);
}

int main (int argc, const char **argv) {
    OSErr r = noErr;

    r = NewSpeechChannel(NULL, &g_speech_channel);
    if (r) exit(r);

    const char *hello = "hello world";
    speak(hello, strlen(hello));

    shutdown(0, 0);
}
