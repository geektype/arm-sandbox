#ifndef K_STRING_H
#define K_STRING_H

#include <stdint.h>

uint32_t k_strlen(const char *str);

uint32_t k_strcmp(const char *s1, const char *s2);

char *k_strcpy(char *dest, const char *src);

#endif
