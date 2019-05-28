CC = g++
LIBS1 = -Wl,-rpath ../lib
LIBS2 = -Wl,-rpath ../lib/HCNetSDKCom

%.o:%.cc
	$(CC) -std=c++11 -I /usr/local/include -pthread -c $< -o $@
all: server client

server: HIK_SDK.grpc.pb.o HIK_SDK.pb.o server.o
	$(CC) $^ -L /usr/local/lib `pkg-config --libs grpc++ grpc`-Wl,--no-as-needed -lgrpc++_reflection -Wl,--as-needed -lprotobuf -L ../lib  -lHIK_SDK $(LIBS1) $(LIBS2) -lpthread -ldl -lssl -o $@

client:  HIK_SDK.grpc.pb.o HIK_SDK.pb.o client.o
	$(CC) $^ -L /usr/local/lib `pkg-config --libs grpc++ grpc`-Wl,--no-as-needed -lgrpc++_reflection -Wl,--as-needed -lprotobuf -lpthread -ldl -lssl -o $@

.PRECIOUS: %.pb.cc
%.pb.cc: %.proto
	protoc --cpp_out=./ $<

clean:
	rm -rf *.o 

