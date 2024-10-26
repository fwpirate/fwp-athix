#!/bin/bash
KERNEL_PATH="fwp-athix-kernel/"
KERNEL="$KERNEL_PATH/athix-kernel"

make build -C $KERNEL_PATH

if [[ ! -f "$KERNEL_PATH/limine/limine" ]]; then
    echo "Limine is not found. Please build Limine first, expected limine in "limine/""
    exit 1
fi

ISO_ROOT="iso_root"
mkdir -p "$ISO_ROOT/boot/limine" "$ISO_ROOT/EFI/BOOT" "$ISO_ROOT/athix"

if [[ -f "$KERNEL" ]]; then
    cp -v "$KERNEL" "$ISO_ROOT/athix/"
else
    echo "Kernel file not found: $KERNEL"
    exit 1
fi

cp -v $KERNEL/limine/limine-bios.sys "$ISO_ROOT/boot/limine/"
cp -v $KERNEL/limine/limine-bios-cd.bin "$ISO_ROOT/boot/limine/"
cp -v $KERNEL/limine/limine-uefi-cd.bin "$ISO_ROOT/boot/limine/"
cp -v $KERNEL/limine/BOOTX64.EFI "$ISO_ROOT/EFI/BOOT/"
cp -v $KERNEL/limine/BOOTIA32.EFI "$ISO_ROOT/EFI/BOOT/"
cp -v $KERNEL/config/limine.conf "$ISO_ROOT/boot/limine/"

tar -cvf "$ISO_ROOT/athix/initrd" --format=ustar -C "initrd" .
cp -Rv sysroot "$ISO_ROOT/athix/"

ISO_NAME="Athix.iso"
xorriso -as mkisofs -b boot/limine/limine-bios-cd.bin \
    -no-emul-boot -boot-load-size 4 -boot-info-table \
    --efi-boot boot/limine/limine-uefi-cd.bin \
    -efi-boot-part --efi-boot-image --protective-msdos-label \
    "$ISO_ROOT" -o "$ISO_NAME"

rm -rf "$ISO_ROOT"
echo ">>> ISO created: $ISO_NAME"
