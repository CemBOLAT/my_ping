
#pragma once

#include <stddef.h>
#include <stdbool.h>

void    my_memset(void *s, int c, size_t n);
void    my_bzero(void *s, size_t n);
void    my_strcpy(char *dst, const char *src);
int     my_strcmp(const char *s1, const char *s2);
size_t  my_strlen(const char *s);
void    my_memcpy(void *dst, const void *src, size_t n);
int     my_atoi(const char *str);
bool    my_isDigit(char c);