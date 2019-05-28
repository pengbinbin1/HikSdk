/*
 *
 *
 */

#include <iostream>
#include <memory>
#include <string>

#include <grpcpp/grpcpp.h>

#include "HIK_SDK.grpc.pb.h"
#include "HIK_SDK.pb.h"
#include "HIK_SDK.h"
#include "upload_vod.h"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;
using HIK_SDK_P::HIKSDK;
using HIK_SDK_P::LoginInfo;
using HIK_SDK_P::ErrCode;
using HIK_SDK_P::Empty;
using HIK_SDK_P::SaveFileParam;
using HIK_SDK_P::SaveFileParam1;
using HIK_SDK_P::Time;
using HIK_SDK_P::FileData;
using HIK_SDK_P::ConfParam;
using HIK_SDK_P::DeviceInfo;
using HIK_SDK_P::DeviceList;
using HIK_SDK_P::InitInfo;
using HIK_SDK_P::Port;
using HIK_SDK_P::UplodParam;
using HIK_SDK_P::VideoId;
using HIK_SDK_P::VideoURL;
using HIK_SDK_P::VideoIdParam;
// Logic and data behind the server's behavior.
class HIKSDKServiceImpl final : public HIKSDK::Service {
public:
	Status Init(ServerContext* context, const LoginInfo* request,
                  InitInfo* reply) override {
		int ret =0;
		NET_DVR_USER_LOGIN_INFO login;
		strcpy(login.sDeviceAddress,request->ip().c_str());
 		strcpy(login.sUserName,request->user().c_str());
 		strcpy(login.sPassword,request->passwd().c_str());
		login.wPort = request->port();
		login.bUseAsynLogin = request->useasynlogin();
		
        //std::cout<< "IP = "<<request->ip()<<std::endl;
        //std::cout << "port = "<<request->port()<<std::endl;

		NET_DVR_DEVICEINFO_V40 devinfo = {0};
		ret = HIK_SDK::GetHikSdkInstance().Init(login,devinfo);
		if (ret!= 0)
        {
            reply->set_devicesn("failed");
            reply->set_devicetype(-1);
			printf("Init failed,ret = %d\n",ret);
		}else{
            reply->set_devicesn((char*)devinfo.struDeviceV30.sSerialNumber);
            reply->set_devicetype(devinfo.struDeviceV30.wDevType);
		}
        reply->set_err(ret);
		return Status::OK;
  }

	Status Uninit(ServerContext* context, const Empty* request, 
		::HIK_SDK_P::ErrCode* response)
	{
		int ret = 0;
		ret = HIK_SDK::GetHikSdkInstance().Destory();
		if (ret != 0)
		{
			printf("Uninit failed\n");
		}else{
			printf("Uninit success\n");
		}
		response->set_errcode(ret);
		return Status::OK;
	}

	Status SaveFileByTime(::grpc::ServerContext* context, const SaveFileParam* request, 
		::HIK_SDK_P::ErrCode* response){
			int ret = 0;
			NET_DVR_TIME startTime;
			NET_DVR_TIME stopTime;
			startTime.dwYear = request->starttime().dwyear();
			startTime.dwMonth = request->starttime().dwmonth();
			startTime.dwDay = request->starttime().dwday();
			startTime.dwHour = request->starttime().dwhour();
			startTime.dwMinute = request->starttime().dwminute();
			startTime.dwSecond = request->starttime().dwsecond();

			stopTime.dwYear = request->stoptime().dwyear();
			stopTime.dwMonth = request->stoptime().dwmonth();
			stopTime.dwDay = request->stoptime().dwday();
			stopTime.dwHour = request->stoptime().dwhour();
			stopTime.dwMinute = request->stoptime().dwminute();
			stopTime.dwSecond = request->stoptime().dwsecond();
			
			LONG channel = request->channel();
			std::string file  =  request->filename();

			ret = HIK_SDK::GetHikSdkInstance().SaveFileByTime(startTime,stopTime,channel,(char *)(file.c_str()));
			if (ret != 0)
			{
				printf("save file by name failed\n");
			}else{
				printf("save file by name success\n");
			}
			response->set_errcode(ret);
			return Status::OK;
		}
    Status UploadLocalFile(::grpc::ServerContext* context,const UplodParam* request,VideoId* response){
        int ret = 0;
        Access acce;
        acce.AcessKey = request->access().accesskey();
        acce.AcessKeySecret = request->access().accesskeysecret();
        acce.RegionId = request->access().regionid();

        File file;
        file.FileName= request->filename();
        file.Title = request->videotitle();
        std::string videoId;
        ret = UploadLocalVideo(acce,file,videoId);
        if(ret != 0)
        {
            printf("@@@upload local video failed,err = %d,line=%d\n",ret,__LINE__);
            response->set_err(ret);
        }else {
             printf("@@@upload local video success,line=%d\n",__LINE__);
             response->set_videoid(videoId);
             response->set_err(0);
        }
        return Status::OK;
    }

    Status GetVideoURL(::grpc::ServerContext* context,const VideoIdParam* request, VideoURL* response ){
        int ret = 0;
        std::string videoId = request->videoid();

        Access acce;
        acce.AcessKey = request->access().accesskey();
        acce.AcessKeySecret = request->access().accesskeysecret();
        acce.RegionId = request->access().regionid();
        std::string URL;
        ret = GetPlayURL(acce,videoId,URL);
        if(ret!= 0){
            printf("@@@get url failed,err = %d,line=%d\n",ret,__LINE__);
            response->set_err(ret);
        }else{
             printf("@@@get url  success,line=%d\n",__LINE__);
             response->set_err(0);
             response->set_url(URL);
        }
        return  Status::OK;
    }
    Status GetDVRConfig(::grpc::ServerContext* context, const ConfParam * request,
                        DeviceList* response){
        int ret = 0;
        DWORD command = request->command();
        LONG  group = request->group();
        NET_DVR_IPPARACFG_V40 deviceCfg;
        ret = HIK_SDK::GetHikSdkInstance().GetDVRConfig(command,group, deviceCfg);
        if(ret != 0)
        {
            printf("@@@get config failed,err = %d, line=%d\n",ret,__LINE__);
        }else{
            printf("@@@get config success\n");
            //set response
            DWORD dChanNum = deviceCfg.dwDChanNum;
            DWORD dStartChan = deviceCfg.dwStartDChan;
            for(int i=0;i<dChanNum;i++)
            {
                // check is enable?
                if (deviceCfg.struIPDevInfo[i].byEnable)
                {
                    DeviceInfo* deviceInfo = response->add_devicelists();
                    deviceInfo->set_port(deviceCfg.struIPDevInfo[i].wDVRPort);
                    deviceInfo->set_user((char*)deviceCfg.struIPDevInfo[i].sUserName);
                    deviceInfo->set_password((char*)deviceCfg.struIPDevInfo[i].sPassword);
                    deviceInfo->set_ip((char*)deviceCfg.struIPDevInfo[i].struIP.sIpV4);
                    deviceInfo->set_channel(i+dStartChan);

                }
            }
        }
        response->set_err(ret);

        return Status::OK;
    }

    Status GetPort(::grpc::ServerContext* context, const ConfParam * request,
                   Port* response){
        int ret =0;
        DWORD command = request->command();
        NET_DVR_NAT_CFG deviceCfg;
        ret = HIK_SDK::GetHikSdkInstance().GetPort(command,deviceCfg);
        if (ret !=0)
        {
            printf("@@@get config failed,err = %d, line=%d\n",ret,__LINE__);
        }else {
            printf("@@@get config success,line = %d\n",__LINE__);
            if(deviceCfg.struRtspPort.wEnable) // check is rtsp enable
            {
                response->set_rtsp(deviceCfg.struRtspPort.wExtPort);
            }

            if(deviceCfg.struHttpPort.wEnable) // check is http enable
            {
                response->set_http(deviceCfg.struHttpPort.wExtPort);
            }

            if(deviceCfg.struHttpsPort.wEnable) //check is https enable
            {
                response->set_https(deviceCfg.struHttpsPort.wExtPort);
            }

            if (deviceCfg.struCmdPort.wEnable) //check is cmd port enable
            {
                response->set_cmd(deviceCfg.struCmdPort.wExtPort);
            }
        }
        response->set_err(ret);
        return Status::OK;
    }

	Status SaveFileByName(::grpc::ServerContext* context, const SaveFileParam1* request, ::HIK_SDK_P::ErrCode* response)
	{
		int ret = 0;
		ret = HIK_SDK::GetHikSdkInstance().SaveFileByName((char *)request->nvrfile().c_str(),(char *)request->desfile().c_str());
		if (ret != 0)
		{
			printf("Save file by name failed\n");
		}else{
			printf("Save file by name success\n");
		}
		response->set_errcode(ret);
		return Status::OK;
	}

	Status FindFileByTime(::grpc::ServerContext* context, const ::HIK_SDK_P::FindFileParam* request, FileData* response)
	{
		int ret = 0;
		NET_DVR_TIME startTime;
		NET_DVR_TIME stopTime;
		startTime.dwYear = request->starttime().dwyear();
		startTime.dwMonth = request->starttime().dwmonth();
		startTime.dwDay = request->starttime().dwday();
		startTime.dwHour = request->starttime().dwhour();
		startTime.dwMinute = request->starttime().dwminute();
		startTime.dwSecond = request->starttime().dwsecond();

		stopTime.dwYear = request->stoptime().dwyear();
		stopTime.dwMonth = request->stoptime().dwmonth();
		stopTime.dwDay = request->stoptime().dwday();
		stopTime.dwHour = request->stoptime().dwhour();
		stopTime.dwMinute = request->stoptime().dwminute();
		stopTime.dwSecond = request->stoptime().dwsecond();
		
		NET_DVR_FIND_DATA fileData={0};
		ret = HIK_SDK::GetHikSdkInstance().FindFile(startTime,stopTime,request->channel(),fileData);
		if (ret!=0 )
		{
			printf("Find file failed\n");
		}else{
			printf("Find file success\n");
		}
		response->set_filename(fileData.sFileName);
		response->set_filesize(fileData.dwFileSize);
		response->set_err(ret);
		return Status::OK	; 
	}
};

void RunServer() {
	std::string server_address("0.0.0.0:8099");
	HIKSDKServiceImpl service;

	ServerBuilder builder;
	// Listen on the given address without any authentication mechanism.
	builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
	// Register "service" as the instance through which we'll communicate with
	// clients. In this case it corresponds to an *synchronous* service.
	builder.RegisterService(&service);
	// Finally assemble the server.
	std::unique_ptr<Server> server(builder.BuildAndStart());
	std::cout << "Server listening on " << server_address << std::endl;

	// Wait for the server to shutdown. Note that some other thread must be
	// responsible for shutting down the server for this call to ever return.
	server->Wait();
}

int main(int argc, char** argv) {
  RunServer();

  return 0;
}
