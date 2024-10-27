-include ../.config
-include olive.config

DEBUG := 0
RELEASE := 0

ifeq ($(CONFIG_BUILD_DEBUG), y)
    DEBUG := 1
	KCFLAGS += -D_DEBUG
else ifeq ($(CONFIG_BUILD_RELEASE), y)
    RELEASE := 1
endif

KCFLAGS += -DOLIVE_VERSION_MAJOR=$(OLIVE_VERSION_MAJOR) -DOLIVE_VERSION_MINOR=$(OLIVE_VERSION_MINOR) -DOLIVE_VERSION_PATCH=$(OLIVE_VERSION_PATCH)

CSRCS := $(shell cat olive.c.files)
ASMSRCS := $(shell cat olive.asm.files)
OBJS := $(CSRCS:.c=.c.o)
OBJS += $(ASMSRCS:.asm=.asm.o)

.PHONY: all clean

all: deps limine $(TARGET)

deps:
	@./get-deps.sh external deps.json

limine:
	@./limine.sh limine

%.c.o: %.c
	@printf "  CC\t$<\n"
	@$(KCC) $(KCFLAGS) $(KINCLUDE) -c $< -o $@

%.asm.o: %.asm
	@printf "  NASM\t$<\n"
	@$(KNASM) $(KNASMFLAGS) $< -o $@

$(TARGET): $(OBJS)
	@printf "  LD\t$@\n"
	@$(KLD) $(KLDFLAGS) -o $@ $(OBJS)

clean:
	@rm -rf $(OBJS) $(TARGET)
