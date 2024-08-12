
#include "../includes/libc.h"

void my_memset(void *s, int c, size_t n)
{
    unsigned char *ptr = (unsigned char *)s;
    while (n--)
        *ptr++ = (unsigned char)c;
}

void my_bzero(void *s, size_t n)
{
    my_memset(s, 0, n);
}

void my_strcpy(char *dst, const char *src)
{
    while (*src)
        *dst++ = *src++;
    *dst = '\0';
}

int my_strcmp(const char *s1, const char *s2)
{
    while (*s1 && *s2 && *s1 == *s2)
    {
        s1++;
        s2++;
    }
    return (*s1 - *s2);
}

size_t my_strlen(const char *s)
{
    size_t len = 0;
    while (*s++)
        len++;
    return len;
}

int my_atoi(const char *str)
{
    int res = 0;
    int sign = 1;
    int i = 0;

    if (str[0] == '-')
    {
        sign = -1;
        i++;
    }
    for (; str[i] != '\0'; ++i)
        res = res * 10 + str[i] - '0';
    return sign * res;
}

void my_memcpy(void *dst, const void *src, size_t n)
{
    unsigned char *d = (unsigned char *)dst;
    unsigned char *s = (unsigned char *)src;
    while (n--)
        *d++ = *s++;
}

bool my_isDigit(char c)
{
    return (c >= '0' && c <= '9');
}

void my_strcat(char *dst, const char *src)
{
    while (*dst)
        dst++;
    while (*src)
        *dst++ = *src++;
    *dst = '\0';
}

int my_pow(int base, int exp)
{
    int result = 1;
    while (exp--)
        result *= base;
    return result;
}
