/*
 *
 */

#include <iostream>
#include <memory>
#include <string>

#include <grpcpp/grpcpp.h>
#include "HIK_SDK.grpc.pb.h"


using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using HIK_SDK_P::HIKSDK;
using HIK_SDK_P::LoginInfo;
using HIK_SDK_P::ErrCode;
using HIK_SDK_P::DeviceList;
using HIK_SDK_P::ConfParam;
class HIKSDKClient {
 public:
  HIKSDKClient(std::shared_ptr<Channel> channel)
      : stub_(HIKSDK::NewStub(channel)) {}

  // Assembles the client's payload, sends it and presents the response back
  // from the server.
  std::string Init(const std::string IP,const std::string user,const std::string passwd,unsigned int port) {
    // Data we are sending to the server.
    LoginInfo request;
    request.set_ip(IP);
	request.set_user(user);
	request.set_passwd(passwd);
	request.set_port(port);

    // Container for the data we expect from the server.
    ErrCode reply;

    // Context for the client. It could be used to convey extra information to
    // the server and/or tweak certain RPC behaviors.
    ClientContext context;

    // The actual RPC.
    Status status = stub_->Init(&context, request, &reply);

    // Act upon its status.
    if (status.ok()) {
		std::cout<< "reply.errcode = "<<reply.errcode()<<std::endl;
      return "ok";
    } else {
      std::cout << status.error_code() << ": " << status.error_message()
                << std::endl;
      return "RPC failed";
    }
  }

  std::string GetConfig( int  command, int group){
      ConfParam request;
      request.set_command(command);
      request.set_group(group);

      DeviceList deviceList ;
      ClientContext context;

      Status status = stub_->GetDVRConfig(&context, request,&deviceList);

      if(status.ok()){
          printf("getconfig success\n");
          if (deviceList.devicelists_size() >0)
          {
              printf("device size = %d\n",deviceList.devicelists_size());
              printf("device[0]IP = %s, port=%d, channel = %d\n",deviceList.devicelists(0).ip().c_str(),deviceList.devicelists(0).port(),deviceList.devicelists(0).channel());
          }else {
              printf("device size = 0\n");
          }
          return  "ok";
      }else {
          return "grpc failed";
      }

  }

 private:
  std::unique_ptr<HIKSDK::Stub> stub_;
};

int main(int argc, char** argv) {
  // Instantiate the client. It requires a channel, out of which the actual RPCs
  // are created. This channel models a connection to an endpoint (in this case,
  // localhost at port 50051). We indicate that the channel isn't authenticated
  // (use of InsecureChannelCredentials()).
  HIKSDKClient greeter(grpc::CreateChannel(
      "localhost:8099", grpc::InsecureChannelCredentials()));
  std::string user("admin");
  std::string passwd("abc123456");
  std::string IP("10.58.12.42");
  unsigned int port = 8000;
  std::string reply = greeter.Init(IP,user,passwd,port);
  std::cout << "Greeter received: " << reply << std::endl;

  std::string confg = greeter.GetConfig(1062,0);
  std::cout << "Getconfg received: " << confg << std::endl;

  return 0;
}
