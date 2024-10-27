#include <stdint.h>
#include <stddef.h>
#include <limine.h>

__attribute__((used, section(".requests"))) static volatile LIMINE_BASE_REVISION(2);
__attribute__((used, section(".requests"))) static volatile struct limine_framebuffer_request framebuffer_request = { .id = LIMINE_FRAMEBUFFER_REQUEST, .revision = 0 };
__attribute__((used, section(".requests_start_marker"))) static volatile LIMINE_REQUESTS_START_MARKER;
__attribute__((used, section(".requests_end_marker"))) static volatile LIMINE_REQUESTS_END_MARKER;

void *memcpy(void *d, const void *s, size_t n) {
    for (size_t i = 0; i < n; i++) ((uint8_t *)d)[i] = ((const uint8_t *)s)[i];
    return d;
}

void *memset(void *s, int c, size_t n) {
    uint8_t *p = s;
    while (n--) *p++ = (uint8_t)c;
    return s;
}

void *memmove(void *d, const void *s, size_t n) {
    if (s > d) for (size_t i = 0; i < n; i++) ((uint8_t *)d)[i] = ((const uint8_t *)s)[i];
    else for (size_t i = n; i-- > 0;) ((uint8_t *)d)[i] = ((const uint8_t *)s)[i];
    return d;
}

int memcmp(const void *s1, const void *s2, size_t n) {
    const uint8_t *p1 = s1, *p2 = s2;
    while (n--) {
        if (*p1 != *p2) return (*p1 < *p2) ? -1 : 1;
        p1++; p2++;
    }
    return 0;
}

static void hcf(void) {
    for (;;) asm ("hlt");
}

void olive_entry(void) {
    if (!LIMINE_BASE_REVISION_SUPPORTED || !framebuffer_request.response || framebuffer_request.response->framebuffer_count < 1) hcf();
    struct limine_framebuffer *fb = framebuffer_request.response->framebuffers[0];
    for (size_t i = 0; i < 100; i++) *((volatile uint32_t *)fb->address + i * (fb->pitch / 4) + i) = 0xffffff;
    hcf();
}