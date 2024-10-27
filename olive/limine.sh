#!/bin/bash

LIMINE_OUT=${1:-limine}

if [ ! -d "$LIMINE_OUT" ]; then
    printf "  INFO\tCloning Limine repository...\n"
    git clone https://github.com/limine-bootloader/limine.git --branch=v8.x-binary --depth=1 "$LIMINE_OUT" >/dev/null 2>&1
    printf "  INFO\tLimine cloned to '%s'.\n" "$LIMINE_OUT"
fi

if command -v make >/dev/null 2>&1; then
    printf "  INFO\tBuilding Limine...\n"
    make -C "$LIMINE_OUT" >/dev/null 2>&1
    printf "  INFO\tLimine built successfully.\n"
else
    printf "  ERROR\tmake is not installed or not found in the system.\n" >&2
    exit 1
fi
