#include <ApplicationServices/ApplicationServices.h>

#include "speak.h"

static SpeechChannel g_speech_channel = NULL;

int speak(const char *text, size_t len) {
    if (!g_speech_channel) {
        int r = NewSpeechChannel(NULL, &g_speech_channel);
        if (r) return r;
    }

    return SpeakText(g_speech_channel, text, len);
}

void shush(void) {
    if (g_speech_channel && SpeechBusy()) {
        StopSpeech(g_speech_channel);
    }
}

void unload(int immediately) {
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
}
