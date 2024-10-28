#ifndef OLIVE_H
#define OLIVE_H

#include <limine.h>
#include <flanterm/flanterm.h>
#include <printf.h>
#include <sys/cpu.h>

extern struct flanterm_context *ft_ctx;

#ifndef _TRACE
#define _TRACE 0
#endif // TRACE

#ifndef _DEBUG
#define _DEBUG 0
#endif // DEBUG

#ifndef _RELEASE
#define _RELEASE 1
#endif // RELEASE

#ifndef OLIVE_VERSION_MAJOR
#define OLIVE_VERSION_MAJOR 0
#endif // OLIVE_VERSION_MAJOR

#ifndef OLIVE_VERSION_MINOR
#define OLIVE_VERSION_MINOR 0
#endif // OLIVE_VERSION_MINOR

#ifndef OLIVE_VERSION_PATCH
#define OLIVE_VERSION_PATCH 0
#endif // OLIVE_VERSION_PATCH

#define UNUSED(x) (void)(x)

#define DIV_ROUND_UP(x, y) (x + (y - 1)) / y
#define ALIGN_UP(x, y) DIV_ROUND_UP(x, y) * y
#define ALIGN_DOWN(x, y) (x / y) * y

#define LOG(level, ...)                           \
    do                                            \
    {                                             \
        if (_DEBUG)                               \
        {                                         \
            printf("[%s] %s: ", level, __FILE__); \
        }                                         \
        else                                      \
        {                                         \
            printf("[%s]: ", level);              \
        }                                         \
                                                  \
        printf(__VA_ARGS__);                      \
    } while (0)

#define INFO(...) LOG("INFO", __VA_ARGS__)
#define WARN(...) LOG("WARN", __VA_ARGS__)
#define ERROR(...) LOG("ERROR", __VA_ARGS__)
#if _DEBUG
#define DEBUG(...) LOG("DEBUG", __VA_ARGS__)
#else
#define DEBUG(...) ((void)0)
#endif

#if _TRACE
#define TRACE(...) LOG("TRACE", __VA_ARGS__)
#else
#define TRACE(...) ((void)0)
#endif // _TRACE

#define FATAL(...) LOG("FATAL", __VA_ARGS__)
#define CRITICAL(...) LOG("CRITICAL", __VA_ARGS__)
#define ALERT(...) LOG("ALERT", __VA_ARGS__)
#define NOTICE(...) LOG("NOTICE", __VA_ARGS__)
#define TODO(...) LOG("TODO", __VA_ARGS__)

#define ASSERT(x)                              \
    do                                         \
    {                                          \
        if (!(x))                              \
        {                                      \
            ERROR("Assertion failed: %s", #x); \
            hcf();                             \
        }                                      \
    } while (0)

#endif // OLIVE_H