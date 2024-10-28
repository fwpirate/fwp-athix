#include <olive.h>
#include <flanterm/flanterm.h>
#include <flanterm/backends/fb.h>
#include <sys/cpu.h>
#include <printf.h>
#include <sys/gdt.h>
#include <sys/idt.h>
#include <mm/pmm.h>
#include <dev/serial.h>

__attribute__((used, section(".requests"))) static volatile LIMINE_BASE_REVISION(2);
__attribute__((used, section(".requests"))) static volatile struct limine_framebuffer_request framebuffer_request = {.id = LIMINE_FRAMEBUFFER_REQUEST, .revision = 0};
__attribute__((used, section(".requests"))) static volatile struct limine_memmap_request memmap_request = {.id = LIMINE_MEMMAP_REQUEST, .revision = 0};
__attribute__((used, section(".requests"))) static volatile struct limine_hhdm_request hhdm_request = {.id = LIMINE_HHDM_REQUEST, .revision = 0};
__attribute__((used, section(".requests_start_marker"))) static volatile LIMINE_REQUESTS_START_MARKER;
__attribute__((used, section(".requests_end_marker"))) static volatile LIMINE_REQUESTS_END_MARKER;

struct flanterm_context *ft_ctx;

static char *logo[] = {
    "  ___  _ _                 ",
    " / _ \\| (_)_   _____      ",
    "| | | | | \\ \\ / / _ \\   ",
    "| |_| | | |\\ V /  __/     ",
    " \\___/|_|_| \\_/ \\___|   "};

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

    if (gdt_init() != 0)
    {
        FATAL("Failed to initialize GDT");
        hcf();
    }

    if (idt_init() != 0)
    {
        FATAL("Failed to initialize IDT");
        hcf();
    }

    for (size_t i = 0; i < sizeof(logo) / sizeof(logo[0]); i++)
    {
        printf("%s\n", logo[i]);
    }
    printf("\n");
    printf("(Olive v%d.%d.%d-%s (%s))\n", OLIVE_VERSION_MAJOR, OLIVE_VERSION_MINOR, OLIVE_VERSION_PATCH, _DEBUG ? "debug" : "release", _TRACE ? "trace" : "no trace");
    printf("\n");

    if (memmap_request.response == NULL || memmap_request.response->entry_count == 0)
    {
        FATAL("No memory map available");
        hcf();
    }

    if (hhdm_request.response == NULL || hhdm_request.response->offset == 0)
    {
        FATAL("No HHDM offset available");
        hcf();
    }

    if (pmm_init(memmap_request.response, hhdm_request.response->offset) != 0)
    {
        FATAL("Failed to initialize Physical Memory Manager");
        hcf();
    }

    char *a = (char *)pmm_request_pages(1);
    if (a == NULL)
    {
        FATAL("Failed to allocate memory for test page");
        hcf();
    }

    *a = 'A';
    DEBUG("Allocated test page at 0x%p, value: %c", a, *a);
    pmm_free_pages(a, 1);

    hlt();
}
