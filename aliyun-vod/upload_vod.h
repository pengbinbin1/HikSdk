#include "src/upload.h"
#include "src/openApiUtil.h"

// define a global authinfo
//VodCredential AuthInfo = {"","","",""};
//memset(&AuthInfo,0,sizeof(VodCredential));

//void testCallback

struct Access{
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

int InitVodClient(Access access); //初始化
int UploadLocalVideo(Access access,File file,std::string & url); // 上传视频
int GetPlayURL(Access access,std::string VideoId,std::string & PlayURL );	// 获取播放地址
//int UploadLocalVideo(File file,std::string & url); // 上传视频