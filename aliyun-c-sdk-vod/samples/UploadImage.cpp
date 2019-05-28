#include <stdio.h>
#include <string>

#include "vod_sdk/upload.h"
#include <jsoncpp/json/json.h>

using namespace std;

void testCallback(int64_t consumed_bytes, int64_t total_bytes)
{
	printf("total :%ld, %ld\n", consumed_bytes, total_bytes);
}

VodApiResponse testUploadLocalImage(VodCredential authInfo)
{
	CreateUploadImageRequest request;
	request.imageType = "default";
	request.title = "testUploadLocalImage";
    UploadOptions uploadOptions;
	//设置上传脚本部署的ECS区域(如有)，如与点播存储同一区域会自动启用内网上传
	//uploadOptions.ecsRegionId = "cn-shanghai";
    //设置自定义回调函数,否则设置为默认,设置为NULL不回调
    //uploadOptions.uploadProgressCallback = testCallback;
	VodApiResponse result = uploadLocalImage(authInfo, request, "./test.png", uploadOptions);
	return result;
}

VodApiResponse testUploadWebImage(VodCredential authInfo) {
	CreateUploadImageRequest request;
	request.imageType = "default";
	request.title = "testUploadWebImage";
    UploadOptions uploadOptions;
	//设置上传脚本部署的ECS区域(如有)，如与点播存储同一区域会自动启用内网上传
	//uploadOptions.ecsRegionId = "cn-shanghai";
	//设置自定义回调函数,否则设置为默认,设置为NULL不回调
	//uploadOptions.uploadProgressCallback = testCallback;
	//设置下载临时目录,默认为/tmp/
	//uploadOptions.tmpDir = "/tmp/";
	VodApiResponse result = uploadWebImage(authInfo, request, "<Your Download Url>", uploadOptions);
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
	VodCredential authInfo = initVodClient("<Your AccessKeyId>", "<Your AccessKeySecret>");
	VodApiResponse response;

	response = testUploadLocalImage(authInfo);
	//response = testUploadWebImage(authInfo);
	printf("httpCode: %d, result: %s\n", response.httpCode, response.result.c_str());
}
