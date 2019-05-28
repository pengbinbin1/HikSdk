// upload_vod.cpp : Defines the exported functions for the DLL application.
//

//#include "stdafx.h"
#include <stdio.h>
#include <string.h>
#include <list>
#include <jsoncpp/json/json.h>

#include "src/upload.h"

// define a global authinfo
VodCredential AuthInfo;
memset(&AuthInfo,0,sizeof(VodCredential));

//void testCallback

struct Acess{
	std::string AcessKey;
	std::string AcessKeySecret;
	std::string RegionId ;
	std::string SecurityToken;
};

struct File{
	std::string FileName;				//原视频文件名(必须)
	std::string Title;					//上传后的视频标题(必须)
	std::string CoverUrl;				//封面照片
	std::string Description;			//视频描述
	std::string Tags;					//视频标签
		  

};


int  InitVodClient(Acess access){
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

int UploadLocalVideo(File file,std::string & url){	
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

	//等待回调，设置播放url
	url = "";

	return 0;
}

