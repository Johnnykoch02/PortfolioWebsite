# Compiler
CC = gcc

# Compiler Flags
CFLAGS = -g -O0 -I. -W -Wall -Wextra -O2 -fsanitize=address,undefined -DMG_ENABLE_LINES

# Linker Flags -lmongoose
# LDFLAGS = -DMG_ENABLE_MBEDTLS=1 -lsqlite3 -lmbedtls -lmbedcrypto -lmbedx509 -pthread -fsanitize=address,undefined
LDFLAGS = -DMG_ENABLE_OPENSSL=1 -lsqlite3 -lssl -lcrypto -pthread -fsanitize=address,undefined

# Source Files (Auto discover all C files in src directory)
SRCS = ./server.c ./mongoose.c
SRCS += $(shell find src -name '*.c')

# Object Files with bin/ directory
OBJS = $(patsubst %.c, bin/%.o, $(SRCS))

# Target Executable
TARGET = bin/portfolio-server

# Cert Files
CERT_FILES = ca.pem cert.pem key.pem client_ca.pem client_cert.pem client_key.pem

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	cp $(CERT_FILES) ./bin/ # Copy cert files to bin/

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