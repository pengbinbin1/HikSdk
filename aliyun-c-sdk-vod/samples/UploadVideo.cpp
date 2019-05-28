#include <stdio.h>
#include <string>
#include <list>

#include "../src/upload.h"
#include <jsoncpp/json/json.h>

using namespace std;

void testCallback(int64_t consumed_bytes, int64_t total_bytes)
{
	printf("total :%ld, %ld\n", consumed_bytes, total_bytes);
}

VodApiResponse testUploadLocalVideo(VodCredential authInfo) {
	CreateUploadVideoRequest request;
	request.fileName = "test.mp4";
	request.title = "testUploadLocalVideo";
	request.cateId = "1";
	request.coverURL = "http://xxxx.jpg";
	request.tags = "test1,test2";
	//request.templateGroupId = "6ae347b0140181ad371d197ebe289326";
	//request.storageLocation = "outin-xx.oss-cn-beijing.aliyuncs.com";
    Json::Value userData;
    Json::Value callbackUrl;
    callbackUrl["CallbackURL"] = "https://demo.sample.com/ProcessMessageCallback";
    userData["MessageCallback"] = callbackUrl;
    Json::Value extend;
    extend["localId"] = "xxx";
    extend["test"] = "www";
    userData["Extend"] = extend;
	request.userData = userData.toStyledString();
    UploadOptions uploadOptions;
    //设置上传脚本部署的ECS区域(如有)，如与点播存储同一区域会自动启用内网上传
    //uploadOptions.ecsRegionId = "cn-shanghai";
    //设置自定义回调函数,否则设置为默认,设置为NULL不回调
    //uploadOptions.uploadProgressCallback = testCallback;
	//uploadOptions.multipartUploadLimit = 20*1024*1024;//设置分片上传阈值
	//uploadOptions.multipartUploadOnceSize = 10*1024*1024;//设置分片上传分片大小
	VodApiResponse result = uploadLocalVideo(authInfo, request, "./test.mp4", uploadOptions);
	return result;
}

VodApiResponse testUploadWebVideo(VodCredential authInfo) {
	CreateUploadVideoRequest request;
	request.fileName = "testWeb.mp4";
	request.title = "testUploadWebVideo";
    UploadOptions uploadOptions;
    //设置上传脚本部署的ECS区域(如有)，如与点播存储同一区域会自动启用内网上传
    //uploadOptions.ecsRegionId = "cn-shanghai";
    //设置自定义回调函数,否则设置为默认,设置为NULL不回调
    //uploadOptions.uploadProgressCallback = testCallback;
	//uploadOptions.multipartUploadLimit = 20*1024*1024;//设置分片上传阈值
	//uploadOptions.multipartUploadOnceSize = 10*1024*1024;//设置分片上传分片大小
    //设置下载临时目录,默认为/tmp/
    //uploadOptions.tmpDir = "/tmp/";
	VodApiResponse result = uploadWebVideo(authInfo, request, "<Your Download Url>", uploadOptions);
	return result;
}


VodApiResponse testUploadLocalM3u8(VodCredential authInfo) {
	CreateUploadVideoRequest request;
	request.fileName = "testLocal.m3u8";
	request.title = "testUploadLocalM3u8";
	list<string> tsList;
	//注意:如果不添加则会自动解析
	//tsList.push_back("/tmp/1.ts");
	UploadOptions uploadOptions;
	VodApiResponse result = uploadLocalM3u8(authInfo, request, "./test.m3u8", tsList, uploadOptions);
	return result;
}

VodApiResponse testUploadWebM3u8(VodCredential authInfo) {
	CreateUploadVideoRequest request;
	request.fileName = "testWeb.m3u8";
	request.title = "testUploadWebM3u8";
	list<string> tsList;
	//注意:如果不添加则会自动解析
	//tsList.push_back("<Ts1 Download Url>");
	//tsList.push_back("<Ts2 Download Url>");
	UploadOptions uploadOptions;
	VodApiResponse result = uploadWebM3u8(authInfo, request, "<Your M3u8 Download Url>", tsList, uploadOptions);
	return result;
}

VodCredential initVodClient(std::string accessKeyId, std::string accessKeySecret) {
    VodCredential authInfo;
    authInfo.accessKeyId = accessKeyId;
    authInfo.accessKeySecret = accessKeySecret;
    authInfo.regionId = "cn-shanghai";
    return authInfo;
}


int main(int argc, char * argv[]) {
	VodCredential authInfo = initVodClient("LTAIojO1xZAIEnYC", "MToeljv2TWQThRzBbgVVI7hxIMAH6O");
	VodApiResponse response;

	response = testUploadLocalVideo(authInfo);
	//response = testUploadWebVideo(authInfo);
	//response = testUploadLocalM3u8(authInfo);
	//response = testUploadWebM3u8(authInfo);
	printf("httpCode: %d, result: %s\n", response.httpCode, response.result.c_str());

	return 0;
}
