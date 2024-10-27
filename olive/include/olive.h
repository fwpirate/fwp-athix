#ifndef OLIVE_H
#define OLIVE_H

#include <limine.h>
#include <flanterm/flanterm.h>
#include <printf.h>

extern struct flanterm_context *ft_ctx;

#ifndef _DEBUG
#define _DEBUG 0
#endif // DEBUG

#ifndef _RELEASE
#define _RELEASE 1
#endif // RELEASE

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
        printf("\n");                             \
    } while (0)

#define INFO(...) LOG("INFO", __VA_ARGS__)
#define WARN(...) LOG("WARN", __VA_ARGS__)
#define ERROR(...) LOG("ERROR", __VA_ARGS__)
#if _DEBUG
#define DEBUG(...) LOG("DEBUG", __VA_ARGS__)
#define TRACE(...) LOG("TRACE", __VA_ARGS__)
#else
#define DEBUG(...) ((void)0)
#define TRACE(...) ((void)0)
#endif

#define FATAL(...) LOG("FATAL", __VA_ARGS__)
#define CRITICAL(...) LOG("CRITICAL", __VA_ARGS__)
#define ALERT(...) LOG("ALERT", __VA_ARGS__)
#define NOTICE(...) LOG("NOTICE", __VA_ARGS__)
#define TODO(...) LOG("TODO", __VA_ARGS__)

#endif // OLIVE_H