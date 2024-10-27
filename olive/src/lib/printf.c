#include <printf.h>

#define NANOPRINTF_USE_FIELD_WIDTH_FORMAT_SPECIFIERS 1
#define NANOPRINTF_USE_PRECISION_FORMAT_SPECIFIERS 1
#define NANOPRINTF_USE_FLOAT_FORMAT_SPECIFIERS 0
#define NANOPRINTF_USE_LARGE_FORMAT_SPECIFIERS 1
#define NANOPRINTF_USE_BINARY_FORMAT_SPECIFIERS 0
#define NANOPRINTF_USE_WRITEBACK_FORMAT_SPECIFIERS 0
#define NANOPRINTF_SNPRINTF_SAFE_TRIM_STRING_ON_OVERFLOW 1
typedef long ssize_t;

#define NANOPRINTF_IMPLEMENTATION
#include <internal/nanoprintf.h>

int putchar(char ch)
{
    if (ft_ctx)
    {
        flanterm_write(ft_ctx, &ch, 1);
    }
    return ch;
}

int puts(const char *str)
{
    int length = 0;
    for (length = 0; *str; length++, putchar(*str++))
        ;
    putchar('\n');
    return length + 1;
}

int vprintf(const char *fmt, va_list args)
{
    return vfprintf(NULL, fmt, args);
}

int vfprintf(void *stream, const char *fmt, va_list args)
{
    (void)stream; // Unused parameter, todo: Add UNIX FILE stream support
    char buffer[1024];
    int length = npf_vsnprintf(buffer, sizeof(buffer), fmt, args);

    if (length >= 0 && length < (int)sizeof(buffer))
    {
        flanterm_write(ft_ctx, buffer, length);
    }

    return length;
}

int printf(const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    int length = vprintf(fmt, args);
    va_end(args);
    return length;
}

int fprintf(void *stream, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    int length = vfprintf(stream, fmt, args);
    va_end(args);
    return length;
}

int snprintf(char *str, size_t size, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    int length = npf_vsnprintf(str, size, fmt, args);
    va_end(args);
    return length;
}
