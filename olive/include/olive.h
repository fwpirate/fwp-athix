#ifndef OLIVE_H
#define OLIVE_H

#include <limine.h>
#include <flanterm/flanterm.h>
#include <printf.h>

extern struct flanterm_context *ft_ctx;

#ifndef DEBUG
#define DEBUG 0
#endif

#ifndef RELEASE
#define RELEASE 1
#endif

#endif // OLIVE_H