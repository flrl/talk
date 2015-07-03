#include <sys/queue.h>

#include <errno.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

#include "buffer.h"

typedef struct buffer_node {
    STAILQ_ENTRY(buffer_node) entries;
    size_t len;
    char str[0];
} BufferNode;

typedef STAILQ_HEAD(buffer_head, buffer_node) BufferHead;

struct s_buffer {
    BufferHead head;
    pthread_mutex_t mutex;
};

static void _buffer_empty_unlocked(BufferHead *head) {
    BufferNode *n1 = STAILQ_FIRST(head);
    while (NULL != n1) {
        BufferNode *n2 = STAILQ_NEXT(n1, entries);
        free(n2);
        n1 = n2;
    }
    STAILQ_INIT(head);
}

Buffer *buffer_new(void) {
    Buffer *buffer = malloc(sizeof *buffer);
    if (NULL == buffer) return NULL;

    STAILQ_INIT(&buffer->head);
    pthread_mutex_init(&buffer->mutex, NULL);

    return buffer;
}

void buffer_delete(Buffer **buffer) {
    Buffer *tmp = *buffer;

    pthread_mutex_lock(&tmp->mutex);

    *buffer = NULL;

    _buffer_empty_unlocked(&tmp->head);

    pthread_mutex_unlock(&tmp->mutex);
    pthread_mutex_destroy(&tmp->mutex);
    free(tmp);
}

int buffer_empty(Buffer *buffer) {
    int r = pthread_mutex_lock(&buffer->mutex);
    if (r) return r;

    _buffer_empty_unlocked(&buffer->head);

    return pthread_mutex_unlock(&buffer->mutex);
}

int buffer_push(Buffer *buffer, const char *str, size_t len) {
    int r = pthread_mutex_lock(&buffer->mutex);
    if (r) return r;

    BufferNode *node = malloc(sizeof(BufferNode) + len + 1);
    if (NULL == node) {
        r = ENOMEM;
        goto cleanup;
    }

    node->len = len;
    strncpy(node->str, str, len + 1);

    STAILQ_INSERT_TAIL(&buffer->head, node, entries);

cleanup:
    pthread_mutex_unlock(&buffer->mutex);
    return r;
}

int buffer_shift(Buffer *buffer, char **str, size_t *len) {
    int r = pthread_mutex_lock(&buffer->mutex);
    if (r) return r;

    BufferNode *node = STAILQ_FIRST(&buffer->head);
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
    STAILQ_REMOVE_HEAD(&buffer->head, entries);
    free(node);

cleanup:
    pthread_mutex_unlock(&buffer->mutex);
    return r;
}
