#ifndef BUFFER_H
#define BUFFER_H

int buffer_push(const char *str, size_t len);
int buffer_shift(char **str, size_t *len);

#endif
