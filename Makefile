KERNEL := olive
SYSOLIVE := $(KERNEL)/sys$(KERNEL)

.PHONY: all build clean menuconfig
all: build

include .config

menuconfig:
	@kconfig-mconf Kconfig
	@$(MAKE) -C $(KERNEL) clean

$(SYSOLIVE): build
build: .config
	@printf "  INFO\tBuilding $(KERNEL) kernel...\n"
	@$(MAKE) -C $(KERNEL)

iso: $(SYSOLIVE)
	@./iso-gen.sh $(SYSOLIVE)

test: iso
ifeq ($(CONFIG_CLEAR_STDIO_BEFORE_EMU),y)
	@clear
endif
	@qemu-system-x86_64 \
		-name "Athix" \
		-cdrom "Athix.iso" \
		-boot d \
		-machine q35 \
		-rtc base=localtime,clock=host \
		-m $(CONFIG_QEMU_MEMORY_SIZE)M \
		-smp $(CONFIG_QEMU_CPU_CORES) \
		$(if $(CONFIG_QEMU_ENABLE_KVM),-enable-kvm) \
		$(if $(CONFIG_QEMU_SERIAL_OUTPUT),-serial stdio) \
		$(if $(CONFIG_QEMU_GRAPHICS_OUTPUT),, -display none) \
		$(if $(CONFIG_QEMU_EXTRA_ARGS),,$(CONFIG_QEMU_EXTRA_ARGS))


clean:
	@$(MAKE) -C $(KERNEL) clean

fullclean: clean
	@rm -rf .config $(KERNEL)/external $(KERNEL)/limine

.config:
	@$(MAKE) menuconfig
