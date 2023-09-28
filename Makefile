BUILD_DIR=build

all: proxy

.PHONY: proxy stress run

proxy:
	make -C proxy

stress:
	make -C stress
	./$(BUILD_DIR)/stress/stress

run: proxy
	./$(BUILD_DIR)/proxy/proxy -l 1234 -h 127.0.0.1 -p 5432 -f logs/log

clean:
	make -C proxy clean
	make -C stress clean
	rm logs/*