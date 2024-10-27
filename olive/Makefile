-include ../.config
-include olive.config

DEBUG := 0
RELEASE := 0

ifeq ($(CONFIG_BUILD_DEBUG), y)
    DEBUG := 1
	KCFLAGS += -DDEBUG
else ifeq ($(CONFIG_BUILD_RELEASE), y)
    RELEASE := 1
endif

SRCS := $(shell cat olive.files)
OBJS := $(SRCS:.c=.o)

.PHONY: all clean

all: deps limine $(TARGET)

deps:
	@./get-deps.sh external deps.json

limine:
	@./limine.sh limine


%.o: %.c
	@printf "  CC\t$<\n"
	@$(KCC) $(KCFLAGS) $(KINCLUDE) -c $< -o $@

$(TARGET): $(OBJS)
	@printf "  LD\t$@\n"
	@$(KLD) $(KLDFLAGS) -o $@ $(OBJS)

clean:
	@rm -rf $(OBJS) $(TARGET)
