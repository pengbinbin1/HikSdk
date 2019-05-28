LIBPATH = ./lib
TARGET = libHIK_SDK.so
OBJ = $(patsubst %.cpp, %.o, $(wildcard *.cpp ))

all:$(OBJ)
	g++  -shared -fPIC -o libHIK_SDK.so $^  -L ../lib -Wl,-rpath=./:./HCNetSDKCom:../lib -lhcnetsdk
	cp libHIK_SDK.so ./lib    
.cpp.o:
	g++  -g -fPIC -std=c++11 -O0 -c $< -o $@

.PHONY:clean
clean:
	rm -rf *.o
	rm -rf *.so
