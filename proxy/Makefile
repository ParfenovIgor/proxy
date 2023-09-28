BUILD_DIR=../build/proxy

SRCS := $(wildcard src/*.c)
OBJS := $(patsubst src/%.c, $(BUILD_DIR)/%.o, $(SRCS))

all: $(OBJS) link

$(BUILD_DIR)/%.o: src/%.c
	gcc -Wall $< -Iinclude -c -o $@

link: $(OBJS)
	gcc $(OBJS) -o $(BUILD_DIR)/proxy

clean:
	rm -f $(BUILD_DIR)/*
