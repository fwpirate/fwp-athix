#include <sys/cpu.h>

void hcf(void) {
    __asm__ volatile("cli");
    hlt();
}

void hlt(void) {
    for (;;) {
        __asm__ volatile (
            "hlt"
        );
    }
}