# Compiler
CC = gcc

# Compiler Flags
CFLAGS = -g -I. -W -Wall -Wextra -O2 -fsanitize=address,undefined -DMG_ENABLE_LINES

# Linker Flags -lmongoose
LDFLAGS = -lsqlite3 -pthread -fsanitize=address,undefined

# Source Files (Auto discover all C files in src directory)
SRCS = ./server.c ./mongoose.c
SRCS += $(shell find src -name '*.c')

# Object Files with bin/ directory
OBJS = $(patsubst %.c, bin/%.o, $(SRCS))

# Target Executable
TARGET = bin/portfolio-server

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

bin/%.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJS): bin/%.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(shell mkdir -p ./bin/static)
$(shell cp -r ./static ./bin/static)

clean:
	rm -rf bin

run:
	./$(TARGET)

run-unsafe:
	ASAN_OPTIONS=detect_odr_violation=0 ./$(TARGET)
