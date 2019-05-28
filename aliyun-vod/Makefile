LIBPATH = ./lib
TARGET = libUploadVOD.so
OBJ = $(patsubst %.cpp, %.o, $(wildcard *.cpp ))

all:$(OBJ)
	g++  -shared -fPIC -o $(TARGET) $^ -I /src -lvod_sdk  -loss_c_sdk  -ljsoncpp  -lmxml -lcurl -lapr-1 -laprutil-1	
	cp libUploadVOD.so /usr/local/lib
	cp libUploadVOD.so /usr/lib
.cpp.o:
	g++  -g -fPIC -std=c++11 -O0 -c $< -o $@


clean:
	rm -f $(TARGET) upload_vod.o
