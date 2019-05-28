LIBS1 =  -Wl,-rpath ../lib
LIBS2 = -Wl,-rpath ../lib/HCNetSDKCom

all:
	g++ -g -std=c++11 -O0 -o  main main.cpp -I ../ -L ../ -lHIK_SDK $(LIBS1) -Wl,-rpath ../lib/HCNetSDKCom
clean:
	rm -f main
