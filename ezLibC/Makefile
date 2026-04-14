CC = gcc

# Default platform = linux (can override: make PLATFORM=doorsos)
PLATFORM ?= linux

SRCDIR = src
PLATFORMDIR = platform/$(PLATFORM)

CFLAGS = -Wall -Wextra -ffreestanding -nostdlib -Iinclude -I$(PLATFORMDIR) -O2
LDFLAGS =

# Source files
EZLIBC_SRCS = $(wildcard $(SRCDIR)/*.c)
PLATFORM_SRCS = $(wildcard $(PLATFORMDIR)/_ezLibC_deps.c) \
                $(wildcard $(PLATFORMDIR)/crt0.c)
PLATFORM_ASM_SRCS = $(wildcard $(PLATFORMDIR)/*.S)

# Output
OUT_DIR = build/$(PLATFORM)

OBJS = $(patsubst $(SRCDIR)/%.c,$(OUT_DIR)/%.o,$(EZLIBC_SRCS)) \
       $(patsubst $(PLATFORMDIR)/%.c,$(OUT_DIR)/plat_%.o,$(PLATFORM_SRCS)) \
       $(patsubst $(PLATFORMDIR)/%.S,$(OUT_DIR)/plat_%.o,$(PLATFORM_ASM_SRCS)) \
       $(OUT_DIR)/test.o

OUT_BIN = $(OUT_DIR)/test_ezlibc

.PHONY: all clean test

all: $(OUT_BIN)

$(OUT_DIR):
	mkdir -p $(OUT_DIR)

$(OUT_BIN): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(OUT_DIR)/%.o: $(SRCDIR)/%.c | $(OUT_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OUT_DIR)/plat_%.o: $(PLATFORMDIR)/%.c | $(OUT_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OUT_DIR)/plat_%.o: $(PLATFORMDIR)/%.S | $(OUT_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OUT_DIR)/test.o: test.c | $(OUT_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

test: $(OUT_BIN)
	./$(OUT_BIN)

clean:
	rm -rf build