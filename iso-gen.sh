#!/bin/bash
KERNEL_PATH="${1:-"olive/sysolive"}"

if [[ ! -f "olive/limine/limine" ]]; then
    printf "  ERROR\tLimine is not found. Please build Limine first, expected limine in \"olive/limine/\"\n" >&2
    exit 1
fi

ISO_ROOT="iso_root"
mkdir -p "$ISO_ROOT/boot/limine" "$ISO_ROOT/EFI/BOOT" "$ISO_ROOT/athix"

if [[ -f "$KERNEL_PATH" ]]; then
    cp -v "$KERNEL_PATH" "$ISO_ROOT/athix/" >/dev/null 2>&1
else
    printf "  ERROR\tKernel: \"$KERNEL_PATH\" not found.\n" >&2
    exit 1
fi

cp olive/limine/limine-bios.sys "$ISO_ROOT/boot/limine/" >/dev/null 2>&1
cp olive/limine/limine-bios-cd.bin "$ISO_ROOT/boot/limine/" >/dev/null 2>&1
cp olive/limine/limine-uefi-cd.bin "$ISO_ROOT/boot/limine/" >/dev/null 2>&1
cp olive/limine/BOOTX64.EFI "$ISO_ROOT/EFI/BOOT/" >/dev/null 2>&1
cp olive/limine/BOOTIA32.EFI "$ISO_ROOT/EFI/BOOT/" >/dev/null 2>&1
cp limine.conf "$ISO_ROOT/boot/limine/" >/dev/null 2>&1

ISO_NAME="Athix.iso"
xorriso -as mkisofs -b boot/limine/limine-bios-cd.bin \
    -no-emul-boot -boot-load-size 4 -boot-info-table \
    --efi-boot boot/limine/limine-uefi-cd.bin \
    -efi-boot-part --efi-boot-image --protective-msdos-label \
    "$ISO_ROOT" -o "$ISO_NAME" >/dev/null 2>&1

rm -rf "$ISO_ROOT"
printf "  INFO\t$ISO_NAME created successfully.\n"
