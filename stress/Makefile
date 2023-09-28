BUILD_DIR=../build/stress

SRCS := $(wildcard *.c)
OBJS := $(patsubst %.c, $(BUILD_DIR)/%.o, $(SRCS))

all: $(OBJS) link

$(BUILD_DIR)/%.o: %.c
	gcc $< -c -o $@

link: $(OBJS)
	gcc $(OBJS) -o $(BUILD_DIR)/stress

clean:
	rm -f $(BUILD_DIR)/*
