#include <string.h>

void *memcpy(void *d, const void *s, size_t n)
{
    for (size_t i = 0; i < n; i++)
        ((uint8_t *)d)[i] = ((const uint8_t *)s)[i];
    return d;
}

void *memset(void *s, int c, size_t n)
{
    uint8_t *p = s;
    while (n--)
        *p++ = (uint8_t)c;
    return s;
}

void *memmove(void *d, const void *s, size_t n)
{
    if (s > d)
        for (size_t i = 0; i < n; i++)
            ((uint8_t *)d)[i] = ((const uint8_t *)s)[i];
    else
        for (size_t i = n; i-- > 0;)
            ((uint8_t *)d)[i] = ((const uint8_t *)s)[i];
    return d;
}

int memcmp(const void *s1, const void *s2, size_t n)
{
    const uint8_t *p1 = s1, *p2 = s2;
    while (n--)
    {
        if (*p1 != *p2)
            return (*p1 < *p2) ? -1 : 1;
        p1++;
        p2++;
    }
    return 0;
}