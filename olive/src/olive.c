#include <olive.h>
#include <flanterm/flanterm.h>
#include <flanterm/backends/fb.h>
#include <sys/cpu.h>
#include <printf.h>

__attribute__((used, section(".requests"))) static volatile LIMINE_BASE_REVISION(2);
__attribute__((used, section(".requests"))) static volatile struct limine_framebuffer_request framebuffer_request = {.id = LIMINE_FRAMEBUFFER_REQUEST, .revision = 0};
__attribute__((used, section(".requests_start_marker"))) static volatile LIMINE_REQUESTS_START_MARKER;
__attribute__((used, section(".requests_end_marker"))) static volatile LIMINE_REQUESTS_END_MARKER;

struct flanterm_context *ft_ctx;

void olive_entry(void)
{
    if (!LIMINE_BASE_REVISION_SUPPORTED || !framebuffer_request.response || framebuffer_request.response->framebuffer_count < 1)
    {
        hcf();
    }

    struct limine_framebuffer *fb = framebuffer_request.response->framebuffers[0];

    ft_ctx = flanterm_fb_init(
        NULL,
        NULL,
        (void *)(uintptr_t)fb->address,
        fb->width,
        fb->height,
        fb->pitch,
        fb->red_mask_size,
        fb->red_mask_shift,
        fb->green_mask_size,
        fb->green_mask_shift,
        fb->blue_mask_size,
        fb->blue_mask_shift,
        NULL, NULL, NULL, NULL, NULL, NULL, NULL,
        0, 0, 1,
        0, 0,
        0, 0);

    if (!ft_ctx)
    {
        hcf();
    };

    ft_ctx->cursor_enabled = false;
    ft_ctx->full_refresh(ft_ctx);
    INFO("Olive (%s) starting...", _DEBUG ? "Debug" : "Release");

    hlt();
}
