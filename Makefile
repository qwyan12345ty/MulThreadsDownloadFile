# 使用的编译器
CXX=g++
# 预处理参数

# 头文件文件路径
CPPLFAGS=-I./inc/					        \
		 -I/usr/local/curl/include/

# 编译选项
CXXFLAGS=-std=c++17 -g

# 库的头文件路径
LDFLAGS=-L/usr/local/curl/lib/

# 链接的库
LIBS=-lcurl	    \
	 -pthread


SRC = $(wildcard ./src/*.cpp)
OBJ = $(patsubst ./src/%.cpp, ./bin/%.o, $(SRC))

TARGET=main
ALL:$(TARGET)

$(TARGET):
	$(CXX) $(SRC) $(CPPLFAGS) $(LDFLAGS) $(LIBS)  -o $@ $(CXXFLAGS)

clean:
	-rm -rf $(OBJ) $(TARGET)

# 声明伪文件
.PHONY:clean ALL