CC := clang

CPPFLAGS := -Icore/include \
            -DLATHE_ARENA_POISON \
            -DLATHE_DEBUG_ENABLED

WARNINGS := -Wall \
            -Wextra \
            -Wpedantic \
            -Wconversion \
            -Werror

CFLAGS := -std=c23 $(WARNINGS)
OBJCFLAGS := -std=c23 $(WARNINGS)

LDFLAGS :=
LDLIBS :=

CORE_SRC := core/source/math/lathe_mat3.c \
            core/source/math/lathe_vec3.c \
            core/source/memory/lathe_memory.c \
            core/source/system/lathe_logger.c

ifeq ($(OS),Windows_NT)
    CPPFLAGS += -DLATHE_WINDOWS
else
    UNAME_S := $(shell uname -s)

    ifeq ($(UNAME_S),Linux)
        CPPFLAGS += -DLATHE_LINUX
        LDLIBS += -lm
    else ifeq ($(UNAME_S),Darwin)
        CPPFLAGS += -DLATHE_OSX

        CORE_SRC += core/source/system/lathe_controller.m \
                    core/source/platform/platform_mac.m \
                    core/source/render/mac_opengl_renderer.m

        LDLIBS += -framework GameController \
                  -framework Foundation \
                  -framework Cocoa \
                  -framework OpenGL \
                  -framework QuartzCore \
                  -lobjc
    else
        $(error Unsupported operating system: $(UNAME_S))
    endif
endif

TARGET := build/main

CORE_OBJ := $(addprefix build/,$(CORE_SRC))
CORE_OBJ := $(CORE_OBJ:.c=.o)
CORE_OBJ := $(CORE_OBJ:.m=.o)

APP_OBJ := build/main.o
OBJ := $(CORE_OBJ) $(APP_OBJ)

TEST_DIR := test
TEST_SRC := $(wildcard $(TEST_DIR)/*.c)
TEST_OBJ := $(patsubst $(TEST_DIR)/%.c,build/tests/%.o,$(TEST_SRC))
TEST_BINS := $(patsubst $(TEST_DIR)/%.c,$(TEST_DIR)/bin/%,$(TEST_SRC))

DEPS := $(OBJ:.o=.d) $(TEST_OBJ:.o=.d)

.DEFAULT_GOAL := all

all: $(TARGET)

$(TARGET): $(OBJ)
	@mkdir -p "$(@D)"
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

$(TEST_DIR)/bin/%: build/tests/%.o $(CORE_OBJ)
	@mkdir -p "$(@D)"
	$(CC) $(LDFLAGS) $^ -o $@ $(LDLIBS)

build/tests/%.o: $(TEST_DIR)/%.c
	@mkdir -p "$(@D)"
	$(CC) $(CPPFLAGS) $(CFLAGS) -MMD -MP -c $< -o $@

build/%.o: %.c
	@mkdir -p "$(@D)"
	$(CC) $(CPPFLAGS) $(CFLAGS) -MMD -MP -c $< -o $@

build/%.o: %.m
	@mkdir -p "$(@D)"
	$(CC) $(CPPFLAGS) $(OBJCFLAGS) -MMD -MP -c $< -o $@

test: $(TEST_BINS)
	@set -e; for test_bin in $(TEST_BINS); do \
		echo "Running $$test_bin"; \
		"./$$test_bin"; \
	done

clean:
	rm -rf build "$(TEST_DIR)/bin" LogFile

# Keep test object files between builds.
.SECONDARY: $(TEST_OBJ)

-include $(DEPS)

.PHONY: all test clean