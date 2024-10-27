# fwp-athix (Athix)

> [!IMPORTANT]  
> Athix and Olive is not made for production use nor daily use. It's a hobby project and not recommended for anything serious.

Yet another Unix-like operating system™️ written for x86_64 processors in C. Athix and Olive are both written my fwpirate \<alavik@disroot.org\>.

Athix is a simple to use and lightweight Unix-like operating system for x86_64 processors. It's designed to be small, simple, and efficient.

## Design

Athix in the core runs on the Olive kernel, see "olive" directory. Olive follows a UNIX-like design, with a focus on simplicity, efficiency, and minimalism. Olive is designed to be easy to understand and modify, making it suitable for learning and experimentation.

## Features
*Note: Athix is still in early development and some features may not be implemented yet.*

...

## Building

To build an ISO for Athix, follow these steps:

1. Install the necessary dependencies:
    - x86_64-elf toolchain (GCC, binutils).
    - xorriso (for creating ISO images).
    - wget or curl (for downloading dependencies).

2. Build the Olive kernel:
    ```sh
    cd olive
    make
    ```

3. Build Athix:
    ```sh
    cd ..
    make
    ```

4. Create the ISO:
    ```sh
    make iso
    ```


You can also build the ISO with a single command:
1. Create the ISO
     ```sh
    make iso
    ```
    *Note: This will also build the Olive kernel as well as the Athix OS and generate a iso*


---
The resulting ISO file will be in the current directory named "Athix.iso".

## Running Athix

To run Athix, you can use QEMU:
```sh
qemu-system-x86_64 -cdrom Athix.iso -serial stdio
```
*This launches QEMU with Athix.iso as the boot disk and outputs serial console output*

Or using our run script, which uses QEMU: 
```sh
make test
```

## License

Athix and Olive are licensed under the CC0 license, meaning they are free to use, copy, modify, distribute, and sell without any restrictions. See the "LICENSE" file for more information.
