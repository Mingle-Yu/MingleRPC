# 定义编译器
CXX = g++

# 定义编译器标志
CXXFLAGS = -g -std=c++14

# 定义源文件和头文件
SRCS = main.cpp CheckArguments.cpp RPCServer.cpp
HDRS = Function.hpp CheckArguments.hpp RPCServer.hpp

# 定义可执行文件名称
TARGET = Server

# 默认目标
all: $(TARGET)

# 生成可执行文件，不保留中间文件
$(TARGET): $(SRCS) $(HDRS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRCS)

# 清理生成的文件
clean:
	rm -f $(TARGET)

# 伪目标，不会与文件名冲突
.PHONY: all clean
