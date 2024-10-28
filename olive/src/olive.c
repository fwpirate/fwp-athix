/*
    Olive the kernel and Athix the OS.
    Copyright (C) 2024  fwpirate <alavik@disroot.org>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include <olive.h>
#include <flanterm/flanterm.h>
#include <flanterm/backends/fb.h>
#include <sys/cpu.h>
#include <printf.h>
#include <sys/gdt.h>
#include <sys/idt.h>
#include <mm/pmm.h>
#include <dev/serial.h>
#include <stdlib.h>
#include <mm/vmm.h>

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
        FATAL("Failed to initialize GDT\n");
        hcf();
    }

    if (idt_init() != 0)
    {
        FATAL("Failed to initialize IDT\n");
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
        FATAL("No memory map available\n");
        hcf();
    }

    if (hhdm_request.response == NULL || hhdm_request.response->offset == 0)
    {
        FATAL("No HHDM offset available\n");
        hcf();
    }

    if (pmm_init(memmap_request.response, hhdm_request.response->offset) != 0)
    {
        FATAL("Failed to initialize Physical Memory Manager\n");
        hcf();
    }

    if (vmm_init() != 0)
    {
        FATAL("Failed to initialize Virtual Memory Manager\n");
        hcf();
    }

    char *a = (char *)kmalloc(sizeof(char));
    if (a == NULL)
    {
        FATAL("Failed to allocate memory for test malloc, failed to use liballoc\n");
        hcf();
    }

    *a = 'A';
    INFO("Allocated 1 byte at %p (%c)\n", a, *a);
    kfree(a);

    hlt();
}
