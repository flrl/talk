#include <sys/queue.h>

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "buffer.h"

typedef struct str_queue_entry {
    STAILQ_ENTRY(str_queue_entry) entries;
    size_t len;
    char str[0];
} StrQueueEntry;

typedef STAILQ_HEAD(str_queue_head, str_queue_entry) StrQueueHead;

static StrQueueHead g_buffer_head = STAILQ_HEAD_INITIALIZER(g_buffer_head);
static pthread_mutex_t g_buffer_mutex = PTHREAD_MUTEX_INITIALIZER;

int buffer_push(const char *str, size_t len) {
    int r = pthread_mutex_lock(&g_buffer_mutex);
    if (r) return r;

    StrQueueEntry *node = malloc(sizeof(StrQueueEntry) + len + 1);
    if (NULL == node) {
        r = ENOMEM;
        goto cleanup;
    }

    node->len = len;
    strncpy(node->str, str, len + 1);

    STAILQ_INSERT_TAIL(&g_buffer_head, node, entries);

cleanup:
    pthread_mutex_unlock(&g_buffer_mutex);
    return r;
}

int buffer_shift(char **str, size_t *len) {
    int r = pthread_mutex_lock(&g_buffer_mutex);
    if (r) return r;

    StrQueueEntry *node = STAILQ_FIRST(&g_buffer_head);
    if (NULL == node) {
        *str = NULL;
        *len = 0;
        r = 0;
        goto cleanup;
    }

    *str = strndup(node->str, node->len);
    if (NULL == *str) {
        *len = 0;
        r = ENOMEM;
        goto cleanup;
    }

    *len = node->len;
    STAILQ_REMOVE_HEAD(&g_buffer_head, entries);
    free(node);

cleanup:
    pthread_mutex_unlock(&g_buffer_mutex);
    return r;
}
