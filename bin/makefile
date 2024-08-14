# 指定C++编译器
CC = g++

# 定义头文件目录
INC_DIR = ../inc

# 指定C++编译时使用的标志
CFLAGS = -std=c++11 `pkg-config --cflags gtk+-3.0` -I$(INC_DIR)

# 指定链接时使用的库
LIBS = -lSDL2 -lgtk-3

# 定义源文件目录
SRC_DIR = src/

# 定义目标文件目录
BIN_DIR = bin/

# 定义源文件列表，使用通配符包含所有的.cpp文件
SOURCES = $(shell find $(SRC_DIR) -type f -name '*.cpp')

# 从源文件列表生成目标文件列表（即.o文件）
OBJECTS = $(patsubst $(SRC_DIR)%.cpp,$(BIN_DIR)%.o,$(SOURCES))

# 定义目标
all: $(BIN_DIR)test

# 链接object文件生成可执行文件
$(BIN_DIR)test: $(OBJECTS)
    $(CC) $(CFLAGS) $(OBJECTS) -o $@ $(LIBS)

# 编译源文件生成object文件
$(BIN_DIR)%.o: $(SRC_DIR)%.cpp
    $(CC) $(CFLAGS) -c $< -o $@

# 清理所有object文件和可执行文件
clean:
    rm -f $(BIN_DIR)*.o
    rm -f $(BIN_DIR)test

# 创建目标文件目录（如果还不存在）
$(BIN_DIR):
    mkdir -p $@

# 在all目标中添加对目标文件目录的依赖
all: $(BIN_DIR) $(BIN_DIR)test