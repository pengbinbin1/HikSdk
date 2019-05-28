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
using HIK_SDK_P::InitInfo;
using HIK_SDK_P::Port;
using HIK_SDK_P::Empty;
using HIK_SDK_P::ErrCode;
using HIK_SDK_P::UplodParam;
using HIK_SDK_P::VideoId;
using HIK_SDK_P::Access;
using HIK_SDK_P::VideoIdParam;
using HIK_SDK_P::VideoURL;
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
    InitInfo reply;

    // Context for the client. It could be used to convey extra information to
    // the server and/or tweak certain RPC behaviors.
    ClientContext context;

    // The actual RPC.
    Status status = stub_->Init(&context, request, &reply);

    // Act upon its status.
    if (status.ok()) {
        std::cout<< "reply.errcode = "<<reply.err()<<std::endl;
        std::cout<<" serial number = "<<reply.devicesn()<<",device type = "<<reply.devicetype()<<std::endl;
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

          if (deviceList.devicelists_size() >0 && deviceList.err()==0)
          {
              printf("device size = %d\n",deviceList.devicelists_size());
              printf("device[0]IP = %s, port=%d, channel = %d\n",deviceList.devicelists(0).ip().c_str(),deviceList.devicelists(0).port(),deviceList.devicelists(0).channel());
              int len = deviceList.devicelists_size();
              for(int i=0;i<len ;i++)
              {
                  printf("device[%d] ip =%s,port = %d,channel = %d\n",i,deviceList.devicelists(i).ip().c_str(),deviceList.devicelists(i).port(),deviceList.devicelists(i).channel());
              }
          }else {
              printf("device size = %d,ret = %d\n",deviceList.devicelists_size(),deviceList.err());
          }
          return  "ok";
      }else {
          return "grpc failed";
      }

  }

  std::string GetPort( int  command, int group){
      ConfParam request;
      request.set_command(command);
      request.set_group(group);

      Port devicePort;
      ClientContext context;

      Status status = stub_->GetPort(&context, request,&devicePort);

      if(status.ok()){
          printf("getcport success\n");

          if ( devicePort.err()==0)
          {
              printf("rtsp port = %d,http port = %d,cmd port = %d,https port = %d\n",devicePort.rtsp(),devicePort.http(),devicePort.cmd(),devicePort.https());
          }else {
              printf("Get port failed,ret  = %d\n",devicePort.err());
          }
          return  "ok";
      }else {
          return "grpc failed";
      }

  }

  std::string UploadVideo(std::string accessKey,std::string accessKeySecret,std::string regionId,std::string fileName,std::string & video){
        UplodParam request;
        Access acce;
        acce.set_accesskey(accessKey);
        acce.set_accesskeysecret(accessKeySecret);
        acce.set_regionid(regionId);


        VideoId  videoId;
        request.set_allocated_access(&acce);
        request.set_filename(fileName);
        request.set_videotitle(fileName);

        ClientContext contex;
        Status status = stub_->UploadLocalFile(&contex,request,&videoId);
        if(status.ok()){
            if(videoId.err()!=0){
                printf("upload video failed,err = %d\n",videoId.err());

            }else{
                video = videoId.videoid();
                printf("video id :%s\n",video.c_str());
                printf("before uploadvideo return\n");
            }
            return  "ok";
        }else {
            return "grpc failed";
        }

  }

  std::string GetURL(std::string accessKey,std::string accessKeySecret,std::string regionId,std::string videoId,std::string & URL){
      VideoIdParam request;
      Access acce;
      acce.set_accesskey(accessKey);
      acce.set_accesskeysecret(accessKeySecret);
      acce.set_regionid(regionId);

      request.set_allocated_access(&acce);
      request.set_videoid(videoId);
      VideoURL videoURL;

      ClientContext context;
      Status status = stub_->GetVideoURL(&context,request,&videoURL);
      if(status.ok()){
          if(videoURL.err() != 0){
              printf("upload video failed,err = %d\n",videoURL.err());

          }else {
              std::string url = videoURL.url();
              printf("url is %s\n",url.c_str());
          }
          return "ok";
      }else {
          return  "grpc failed";
      }
  }

  std::string UnInit(){
      ClientContext context;
      Empty empty;
      ErrCode response;
      Status status = stub_->Uninit(&context,empty,&response);
      if (response.errcode() ==0)
      {
          printf("destory success\n");
          return "ok";
      }else{
          printf("destory failed,errcode = %d",response.errcode());
          return  "grpc failed";
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
      "127.0.0.1:8099", grpc::InsecureChannelCredentials()));

  /*
  std::string user("admin");
  std::string passwd("abc123456");
  std::string IP("10.58.123.42");
  unsigned int port = 8000;
  std::string reply = greeter.Init(IP,user,passwd,port);
  std::cout << "Greeter received: " << reply << std::endl;

  std::string confg = greeter.GetConfig(1062,0);
  std::cout << "Getconfg received: " << confg << std::endl;

  std::string Devport = greeter.GetPort(6111,0);
  std::cout <<  "GetPort received: " << Devport << std::endl;

  std::string destory = greeter.UnInit();*/

  std::string acceKey = "LTAIbIXzJ2U4K3gj";
  std::string acceKeySecret = "WDHcRuAqU1wrU2V1fMrTGX7CKXY399";
  std::string regionId = "cn-shanghai";

  std::string filename = "test.mp4";

  std::string videoId;
  std::string result = greeter.UploadVideo(acceKey,acceKeySecret,regionId,filename,videoId);


  /*
  std::string videoId = "6aece81540f347a7ad0a2150abbd26be";
  std::string url;
  std::string result = greeter.GetURL(acceKey,acceKeySecret,regionId,videoId,url);*/

   printf("before return\n");
  return 0;
}
