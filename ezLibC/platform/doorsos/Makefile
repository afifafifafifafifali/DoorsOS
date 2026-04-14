CC = gcc
CFLAGS = -Wall -Wextra -ffreestanding -nostdlib -I../include -I. -O2 -mno-red-zone
LDFLAGS =

# Auto-discover source files
SRCDIR = ../src
DEPS_SRC = _ezLibC_deps.c
EZLIBC_SRCS = $(wildcard $(SRCDIR)/*.c)
CRT0 = crt0.c
PLATFORM_ASM = $(wildcard *.S)

# Object files
OUT_DIR = out
OBJS = $(patsubst %.c,$(OUT_DIR)/%.o,$(notdir $(DEPS_SRC) $(EZLIBC_SRCS) $(CRT0))) \
       $(patsubst %.S,$(OUT_DIR)/%.o,$(notdir $(PLATFORM_ASM)))

# Output
OUT_LIB = $(OUT_DIR)/libezlibc.a
OUT_BIN = $(OUT_DIR)/test_ezlibc

.PHONY: all clean test

all: $(OUT_LIB) $(OUT_BIN)

$(OUT_DIR):
	mkdir -p $(OUT_DIR)

# Archive
$(OUT_LIB): $(OBJS)
	ar rcs $@ $^

# Platform sources
$(OUT_DIR)/_ezLibC_deps.o: _ezLibC_deps.c | $(OUT_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OUT_DIR)/crt0.o: crt0.c | $(OUT_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Assembly sources
$(OUT_DIR)/%.o: %.S | $(OUT_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Library sources (auto-discovered from ../src/)
$(OUT_DIR)/%.o: $(SRCDIR)/%.c | $(OUT_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Link
$(OUT_BIN): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

test: $(OUT_BIN)
	./$(OUT_BIN)

clean:
	rm -rf $(OUT_DIR)
