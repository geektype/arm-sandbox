#include "string.h"
#include "common.h" // IWYU pragma: keep
#include "stdint.h"

uint32_t k_strlen(const char *str) {
  const char *p;

  if (str == 0x0)
    return 0;

  p = str;
  while (*p != '\0')
    p++;

  return p - str;
}

uint32_t k_strcmp(const char *s1, const char *s2) {
  while (*s1 && (*s1 == *s2)) {
    s1++;
    s2++;
  }
  return (unsigned char)*s1 - (unsigned char)*s2;
}

char *k_strcpy(char *dest, const char *src) {
  char *start = dest;

  do {
    *dest = *src;
    dest++;
    src++;
  } while (*src != '\0');

  *dest = '\0';
  return start;
}
