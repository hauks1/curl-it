#include "bad_string.h"
/* BAD string operations library */
int bad_strlen(const char *s)
{
    int len = 0;
    while (*s++)
        len++;
    return len;
}
void bad_strcpy(char *dest, const char *src)
{
    int i = 0;
    while ((dest[i] = src[i]) != '\0')
        i++;
}
int bad_strncpy(char *dest, const char *src, unsigned long long n)
{
    if (n == 0)
        return -1;

    int i;
    for (i = 0; i < n - 1 && src[i] != '\0'; i++)
        dest[i] = src[i];

    dest[i] = '\0';
    return i;
}
int bad_atoi(const char *s)
{
    int num = 0;
    int sign = 1;
    // check sign
    if (*s == '-')
    {
        sign = -1;
        s++;
    }
    while (*s != '\0')
    {
        if (*s < '0' || *s > '9')
            break;
        num = num * 10 + (*s - '0');
        s++;
    }
    return num * sign;
}