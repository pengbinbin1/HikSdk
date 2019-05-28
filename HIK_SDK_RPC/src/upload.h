/**
 * UPLOAD_H 
 *
 * Aliyun VoD's Upload Video head file, which wraps parameters to upload a video into VoD.
 * Users could pass parameters to this function, including File Path,Title,etc.
 * For more details, please check out the VoD API document: https://help.aliyun.com/document_detail/55407.html
 */

#ifndef UPLOAD_H
#define UPLOAD_H 

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <list>
#include <stdio.h>
#include <sys/stat.h>

#include "openApiUtil.h"

typedef struct {
	std::string endpoint;
	std::string bucket;
	std::string fileName;
	std::string accessKeyId;
	std::string accessKeySecret;
	std::string securityToken;
	std::string videoId;
} UploadInfo;

typedef struct {
	std::string title;
	std::string fileName;
	std::string fileSize;
	std::string description;
	std::string coverURL;
	std::string cateId;
	std::string tags;
	std::string templateGroupId;
	std::string workflowId;
	std::string userData;
	std::string storageLocation;
} CreateUploadVideoRequest;

typedef struct {
	std::string imageType;
	std::string imageExt;
	std::string title;
	std::string tags;
	std::string cateId;
	std::string description;
	std::string userData;
	std::string storageLocation;
} CreateUploadImageRequest;

typedef struct {
	std::string businessType;
	std::string mediaExt;
	std::string title;
	std::string cateId;
	std::string fileName;
	std::string fileSize;
	std::string tags;
	std::string description;
	std::string userData;
	std::string storageLocation;
} CreateUploadAttachedMediaRequest;

void percentage(int64_t consumed_bytes, int64_t total_bytes);

typedef struct UploadOptions {
	UploadOptions() {
		uploadProgressCallback = percentage;
		multipartUploadLimit = 10 * 1024 * 1024;
		multipartUploadOnceSize = 10 * 1024 * 1024;
		tmpDir = "/tmp/";
	}
	void (*uploadProgressCallback) (int64_t, int64_t);
	std::string ecsRegionId;
	int multipartUploadLimit;
	int multipartUploadOnceSize;
	std::string tmpDir;
} UploadOptions;

VodApiResponse uploadLocalVideo(VodCredential authInfo, CreateUploadVideoRequest request, std::string localPath, UploadOptions &uploadOptions);
VodApiResponse uploadWebVideo(VodCredential authInfo, CreateUploadVideoRequest request, std::string webPath, UploadOptions &uploadOptions);
VodApiResponse uploadLocalM3u8(VodCredential authInfo, CreateUploadVideoRequest request, std::string m3u8Path, std::list<std::string> tsPathList, UploadOptions &uploadOptions);
VodApiResponse uploadWebM3u8(VodCredential authInfo, CreateUploadVideoRequest request, std::string webPath, std::list<std::string> tsWebPathList, UploadOptions &uploadOptions);
VodApiResponse uploadLocalImage(VodCredential authInfo, CreateUploadImageRequest request, std::string localPath, UploadOptions &uploadOptions);
VodApiResponse uploadWebImage(VodCredential authInfo, CreateUploadImageRequest request, std::string webPath, UploadOptions &uploadOptions);
VodApiResponse uploadLocalAttachedMedia(VodCredential authInfo, CreateUploadAttachedMediaRequest request, std::string localPath, UploadOptions &uploadOptions);
VodApiResponse uploadWebAttachedMedia(VodCredential authInfo, CreateUploadAttachedMediaRequest request, std::string webPath, UploadOptions &uploadOptions);

#endif
