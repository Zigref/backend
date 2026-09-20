COMPILER = g++
BINARY_NAME = server
BUILD_FOLDER = build
TARGET = $(BUILD_FOLDER)/$(BINARY_NAME)
FLAGS = -std=c++23 -DASIO_STANDALONE $(shell pkg-config --cflags asio 2>/dev/null) -lsqlite3

$(TARGET): ./main.cpp
	mkdir -p $(BUILD_FOLDER)
	$(COMPILER) ./main.cpp -o $(TARGET) $(FLAGS)

build: $(TARGET)

download_database:
	wget -q -O ./zigistry.db "https://huggingface.co/buckets/Zigistry/Zigistry/resolve/zigistry.db"

run: $(TARGET)
	./$(TARGET)
