KERNEL := olive
SYSOLIVE := $(KERNEL)/sys$(KERNEL)

.PHONY: all build clean menuconfig
all: build

menuconfig:
	@kconfig-mconf Kconfig

$(SYSOLIVE): build
build: .config
	@printf "  INFO\tBuilding $(KERNEL) kernel...\n"
	@$(MAKE) -C $(KERNEL)

iso: $(SYSOLIVE)
	@./iso-gen.sh $(SYSOLIVE)

test: iso
	@qemu-system-x86_64 -name "Athix" -cdrom "Athix.iso" -boot d -machine q35 -rtc base=localtime,clock=host -m 2G -serial stdio

clean:
	@$(MAKE) -C $(KERNEL) clean

fullclean: clean
	@rm -f .config

.config:
	@$(MAKE) menuconfig
