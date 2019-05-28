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
	std::string FileName;				//ԭ��Ƶ�ļ���(����)
	std::string Title;					//�ϴ������Ƶ����(����)
	std::string CoverUrl;				//������Ƭ
	std::string Description;			//��Ƶ����
	std::string Tags;					//��Ƶ��ǩ
		  

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
	//����request
	CreateUploadVideoRequest request;
	request.fileName = file.FileName;
	request.title = file.Title;
	request.coverURL = file.CoverUrl;
	request.tags = file.Tags;

	//���ûص�����

	//����uploadOptions
	UploadOptions uploadOptions;
	VodApiResponse result = uploadLocalVideo(AuthInfo,request,file.FileName,uploadOptions);
	if (result.httpCode != 200)
	{
		printf("uploadLocalVideo failed,%s\n",result.result.c_str());
		return -1;
	}

	//�ȴ��ص������ò���url
	url = "";

	return 0;
}

