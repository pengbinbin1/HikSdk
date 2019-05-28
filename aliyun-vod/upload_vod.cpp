// upload_vod.cpp : Defines the exported functions for the DLL application.
//

//#include "stdafx.h"
#include <stdio.h>
#include <string.h>
#include <list>
#include <jsoncpp/json/json.h>
#include <map>

#include "upload_vod.h"
int  InitVodClient(Access access,VodCredential& AuthInfo){
	
	if (AuthInfo.accessKeyId== "" || AuthInfo.accessKeySecret == ""){
		AuthInfo.accessKeyId = access.AcessKey;
		AuthInfo.accessKeySecret = access.AcessKeySecret;
		AuthInfo.securityToken = access.SecurityToken;
		AuthInfo.regionId = access.RegionId;
		return 0;
	} else{
		printf("client has been init \n");
	}
	return 0;
}

int UploadLocalVideo(Access access,File file,std::string & VideoId){	
	
	//创建AuthInfo
	VodCredential AuthInfo;
	AuthInfo.accessKeyId = access.AcessKey;
//	printf("keyId=%s\n",access.AcessKey.c_str());
	AuthInfo.accessKeySecret = access.AcessKeySecret;
	AuthInfo.securityToken = access.SecurityToken;
	AuthInfo.regionId = access.RegionId;

	//创建request
	CreateUploadVideoRequest request;
	request.fileName = file.FileName;
	request.title = file.Title;
	request.coverURL = file.CoverUrl;
	request.tags = file.Tags;

	//设置回调函数

	//创建uploadOptions
	UploadOptions uploadOptions;
	VodApiResponse result = uploadLocalVideo(AuthInfo,request,file.FileName,uploadOptions);
	if (result.httpCode != 200)
	{
		printf("uploadLocalVideo failed,%s\n",result.result.c_str());
		return -1;
	}

	//printf("line:%d,httpcode:%d,httpbody:%s\n",__LINE__,result.httpCode,result.result.c_str());
	//解析返回值得到VideoId
	Json::Reader reader;
	Json::Value root;
	if (reader.parse(result.result,root))
	{
		VideoId = root["VideoId"].asString();
		printf("VideoId is %s\n",VideoId.c_str());
	}else{
		printf("parse result failed:%s\n",result.result.c_str());
		return -1;
	}
	

	return 0;
}

int GetPlayURL(Access access,std::string VideoId,std::string & PlayURL){

	VodCredential AuthInfo;
	AuthInfo.accessKeyId = access.AcessKey;
	AuthInfo.accessKeySecret = access.AcessKeySecret;
	AuthInfo.securityToken = access.SecurityToken;
	AuthInfo.regionId = access.RegionId;

	std::map<std::string,std::string> args;
	args["VideoId"] = VideoId;
	std::string apiName = "GetPlayInfo";

	VodApiResponse result = getAcsResponse(AuthInfo,apiName,args);
	if (result.httpCode != 200)
	{
		printf("get playinfo failed,%s\n",result.result.c_str());
		return result.httpCode;
	}
	//printf("get playinfo response:%s\n",result.result.c_str());

	Json::Reader reader;
	Json::Value  root;
	
	if (reader.parse(result.result,root))
	{
		Json::Value PlayInfoListRoot; 
		//std::string PlayInfoList = root["PlayInfoList"].asString();
		std::string PlayInfoList = root["PlayInfoList"].toStyledString();
		//printf("PlayInfoList:%s\n",PlayInfoList.c_str());
		if (reader.parse(PlayInfoList,PlayInfoListRoot))
		{
			Json::Value PlayInfoRoot;
			//std::string PlayInfo = PlayInfoListRoot["PlayInfo"].asString();
			std::string PlayInfo = PlayInfoListRoot["PlayInfo"].toStyledString();
		  //	printf("PlayInfo :%s\n",PlayInfo.c_str());
			if (reader.parse(PlayInfo,PlayInfoRoot))
			{
				PlayURL= PlayInfoRoot[0]["PlayURL"].asString();
				printf("line:%d play URL is %s \n",__LINE__,PlayURL.c_str());
			}else{
				printf("line:%d parse result failed:%s\n",__LINE__,result.result.c_str());
				return -1;
			}

		}else{
			printf("line:%d parse result failed:%s\n",__LINE__,result.result.c_str());
			return -1;
		}
	}else{
		printf("line:%d parse result failed:%s\n",__LINE__,result.result.c_str());
		return -1;
	}
	
	return 0;
}
