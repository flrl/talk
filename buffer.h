#ifndef BUFFER_H
#define BUFFER_H

typedef struct s_buffer Buffer;

Buffer *buffer_new(void);
void buffer_delete(Buffer **);

int buffer_empty(Buffer *buffer);

int buffer_push(Buffer *buffer, const char *str, size_t len);
int buffer_shift(Buffer *buffer, char **str, size_t *len);

#endif
