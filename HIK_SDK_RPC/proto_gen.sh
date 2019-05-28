protoc  --cpp_out=./  HIK_SDK.proto
protoc --grpc_out=./ --plugin=protoc-gen-grpc=`which grpc_cpp_plugin` HIK_SDK.proto
