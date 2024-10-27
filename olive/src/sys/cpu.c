#include <sys/cpu.h>
#include <olive.h>

void hcf(void)
{
    TRACE("Halting CPU and disabling interrupts...");
    __asm__ volatile("cli");
    hlt();
}

void hlt(void)
{
    TRACE("Halting CPU...");
    for (;;)
    {
        __asm__ volatile(
            "hlt");
    }
}