#include <ApplicationServices/ApplicationServices.h>

#include "buffer.h"
#include "speak.h"

static SpeechChannel g_speech_channel = NULL;

static int speak_next(void) {
    char *text;
    size_t len;

    int r = buffer_shift(&text, &len);
    if (r) return r;
    if (NULL == text) return 0;

    r = SpeakText(g_speech_channel, text, len);

    free(text);
    return r;
}

static void speech_done_cb(SpeechChannel channel, SRefCon refcon) {
    speak_next();
}

int speak(const char *text, size_t len) {
    int r = 0;

    if (!g_speech_channel) {
        r = NewSpeechChannel(NULL, &g_speech_channel);
        if (r) return r;

        r = SetSpeechInfo(g_speech_channel, soSpeechDoneCallBack, speech_done_cb);
        if (r) return r;
    }

    r = buffer_push(text, len);
    if (r) return r;

    if (!SpeechBusy()) {
        r = speak_next();
    }

    return r;
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
