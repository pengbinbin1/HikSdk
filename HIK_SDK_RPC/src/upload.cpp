#include <stdio.h>
#include<sys/ioctl.h>
#include<unistd.h>
#include<termios.h>
#include <fstream>
#include <sstream>
#include <jsoncpp/json/json.h>

#include "oss_c_sdk/oss_api.h"
#include "oss_c_sdk/aos_http_io.h"
#include "upload.h"
#include "httpDownloader.h"
#include "signatureUtil.h"

using namespace std;

template<typename T> string toString(const T& t) {
    ostringstream outstr;
    outstr<<t;
    return outstr.str();  
}

int getscreenwidth()
{
  struct winsize size;
    ioctl(STDIN_FILENO, TIOCGWINSZ,&size);
	return size.ws_col;
}

void percentage(int64_t consumed_bytes, int64_t total_bytes) 
{
    assert(total_bytes >= consumed_bytes);
	int width = getscreenwidth();
	string linestr;
	for(int i = 0; i< (width - 51) * consumed_bytes /total_bytes; i++) {
		linestr += "=";
	}
	linestr += ">";
	printf("\r\033[K\r");
	char buf[10] = {0};
	snprintf(buf, sizeof(buf), "%2ld%%", consumed_bytes * 100 / total_bytes);
	printf("%-4s[%-*s] total: %ld, remain: %ld", buf, width-50, linestr.c_str(), total_bytes, total_bytes-consumed_bytes);
	fflush(stdout);
}

void init_options(oss_request_options_t *options, UploadInfo &uploadInfo)
{
    options->config = oss_config_create(options->pool);
    /* 用char*类型的字符串初始化aos_string_t类型。*/
    aos_str_set(&options->config->endpoint, uploadInfo.endpoint.c_str());
    aos_str_set(&options->config->access_key_id, uploadInfo.accessKeyId.c_str());
    aos_str_set(&options->config->access_key_secret, uploadInfo.accessKeySecret.c_str());
    aos_str_set(&options->config->sts_token, uploadInfo.securityToken.c_str());
    /* 是否使用了CNAME。0表示不使用。*/
    options->config->is_cname = 0;
    /* 设置网络相关参数，比如超时时间等。*/
    options->ctl = aos_http_controller_create(options->pool, 0);
}

void refresh_options(oss_request_options_t *options, UploadInfo &uploadInfo)
{
    /* 用char*类型的字符串初始化aos_string_t类型。*/
    aos_str_set(&options->config->endpoint, uploadInfo.endpoint.c_str());
    aos_str_set(&options->config->access_key_id, uploadInfo.accessKeyId.c_str());
    aos_str_set(&options->config->access_key_secret, uploadInfo.accessKeySecret.c_str());
    aos_str_set(&options->config->sts_token, uploadInfo.securityToken.c_str());
}

int getUploadInfo(string createUploadResult, UploadInfo &uploadInfo, string ecsRegionId = "")
{
	Json::Reader reader;
	Json::Value root;
	if (reader.parse(createUploadResult, root))
	{
		Json::Value uploadAddrRoot, uploadAuthRoot;
		std::string uploadAddr = root["UploadAddress"].asString();
		uploadAddr =  base64_decode(uploadAddr.c_str(), uploadAddr.size());
		if(reader.parse(uploadAddr, uploadAddrRoot)) {
			string endpoint = uploadAddrRoot["Endpoint"].asString();
			if(!ecsRegionId.empty() && endpoint.find(ecsRegionId) != string::npos) {
				endpoint = endpoint.replace(endpoint.find(ecsRegionId), ecsRegionId.size(), ecsRegionId + "-internal");
			}
			uploadInfo.endpoint = endpoint;
			uploadInfo.bucket = uploadAddrRoot["Bucket"].asString();
			uploadInfo.fileName = uploadAddrRoot["FileName"].asString();
		}
		else {
			return -1;
		}
		uploadInfo.videoId = root["VideoId"].asString();
		std::string uploadAuth = root["UploadAuth"].asString();
		uploadAuth =  base64_decode(uploadAuth.c_str(), uploadAuth.size());
		if(reader.parse(uploadAuth, uploadAuthRoot))
		{
			uploadInfo.accessKeyId = uploadAuthRoot["AccessKeyId"].asString();
			uploadInfo.accessKeySecret = uploadAuthRoot["AccessKeySecret"].asString();
			uploadInfo.securityToken = uploadAuthRoot["SecurityToken"].asString();
			return 0;
		}
	}
	return -1;
}

//上传进度汇报
VodApiResponse reportUploadProgress(VodCredential authInfo, string uploadAddress, string uploadId, string fileName, int fileSize, int donePartsCount, int partSize) {
	string secretKey = "dGfgZ@BfxKDwE9jQ";
    char hostname[129]={0} ;
    gethostname(hostname,sizeof(hostname));
	string hostName = hostname;
	map<string, string> args;
	args["Source"] = "CSDK";
	args["ClientId"] = hostname;
	args["BusinessType"] = "UploadVideo";
	args["TerminalType"] = "Linux";
	args["DeviceModel"] = "centOs";
	args["AppVersion"] = "0.0.2";
	string timestamp = toString(time(NULL));
	args["AuthTimestamp"] = timestamp;
	args["AuthInfo"] = getMD5(hostName + "|" + secretKey  + "|" + timestamp);
	args["FileName"] = fileName;
	args["FileSize"] = toString(fileSize);
	args["FileCreateTime"] = timestamp;
	args["FileHash"] = getMD5(toString(donePartsCount*partSize));
	args["UploadId"] = uploadId;
	args["DonePartsCount"] = toString(donePartsCount);
	args["PartSize"] = toString(partSize);
	args["UploadPoint"] = "{}";
	args["UploadAddress"] = uploadAddress;
	VodApiResponse result = getAcsResponse(authInfo, "ReportUploadProgress", args);
	return result;
}

int refreshUploadInfo(VodCredential authInfo, string videoId, UploadInfo &uploadInfo)
{
	map<string, string> args;
	args["VideoId"] = videoId;
	VodApiResponse result = getAcsResponse(authInfo, "RefreshUploadVideo", args);
	if(result.httpCode != 200) {
		return -1;
	}
	//使用原来的endpoint,防止内网endpoint被重新获取覆盖
	string originalEndpoint = uploadInfo.endpoint;
	if(getUploadInfo(result.result, uploadInfo)) {
		return -1;
	}
	uploadInfo.endpoint = originalEndpoint;
	return 0;
}

int buffer_upload(UploadInfo uploadInfo, string fileContent, void (*uploadProgressCallback) (int64_t, int64_t) = NULL) {
	int code = -1;
    /* 在程序入口调用aos_http_io_initialize方法来初始化网络、内存等全局资源。*/
    if (aos_http_io_initialize(NULL, 0) != AOSE_OK) {
		return code;
    }
    /* 用于内存管理的内存池（pool），等价于apr_pool_t。其实现代码在apr库中。*/
    aos_pool_t *pool;
    /* 重新创建一个内存池，第二个参数是NULL，表示没有继承其它内存池。*/
    aos_pool_create(&pool, NULL);
    /* 创建并初始化options，该参数包括endpoint、access_key_id、acces_key_secret、is_cname、curl等全局配置信息。*/
    oss_request_options_t *oss_client_options;
    /* 在内存池中分配内存给options。*/
    oss_client_options = oss_request_options_create(pool);
    /* 初始化Client的选项oss_client_options。*/
    init_options(oss_client_options, uploadInfo);
    /* 初始化参数。*/
    aos_string_t bucket;
    aos_string_t object;
    aos_list_t buffer;
    aos_buf_t *content = NULL;
    aos_table_t *headers = NULL;
    aos_table_t *resp_headers = NULL; 
    aos_status_t *resp_status = NULL; 
    aos_str_set(&bucket, uploadInfo.bucket.c_str());
    aos_str_set(&object, uploadInfo.fileName.c_str());
    aos_list_init(&buffer);
    content = aos_buf_pack(oss_client_options->pool, fileContent.c_str(), fileContent.size());
    aos_list_add_tail(&content->node, &buffer);
    /* 上传文件 */
    resp_status = oss_put_object_from_buffer(oss_client_options, &bucket, &object, &buffer, headers, &resp_headers);
    /* 判断上传是否成功 */
    if (aos_status_is_ok(resp_status)) 
		code = 0;
    /* 释放内存池，相当于释放了请求过程中各资源分配的内存。*/
    aos_pool_destroy(pool);
    /* 释放之前分配的全局资源。*/
    aos_http_io_deinitialize();
    return code;
}

int sample_upload(UploadInfo uploadInfo, string localfilepath, void (*uploadProgressCallback) (int64_t, int64_t) = NULL) {
	int code = -1;
    /* 在程序入口调用aos_http_io_initialize方法来初始化网络、内存等全局资源。*/
    if (aos_http_io_initialize(NULL, 0) != AOSE_OK) {
        return code;
    }
    /* 用于内存管理的内存池（pool），等价于apr_pool_t。其实现代码在apr库中。*/
    aos_pool_t *pool;
    /* 重新创建一个内存池，第二个参数是NULL，表示没有继承其它内存池。*/
    aos_pool_create(&pool, NULL);
    /* 创建并初始化options，该参数包括endpoint、access_key_id、acces_key_secret、is_cname、curl等全局配置信息。*/
    oss_request_options_t *oss_client_options;
    /* 在内存池中分配内存给options。*/
    oss_client_options = oss_request_options_create(pool);
    /* 初始化Client的选项oss_client_options。*/
    init_options(oss_client_options, uploadInfo);
    /* 初始化参数。*/
    aos_string_t bucket;
    aos_string_t object;
    aos_string_t file;
    //aos_table_t *headers = NULL;
    aos_table_t *resp_headers = NULL; 
    aos_status_t *resp_status = NULL; 
    aos_str_set(&bucket, uploadInfo.bucket.c_str());
    aos_str_set(&object, uploadInfo.fileName.c_str());
    aos_str_set(&file, localfilepath.c_str());
    /* 上传文件。*/
    //resp_status = oss_put_object_from_file(oss_client_options, &bucket, &object, &file, headers, &resp_headers);
	/*带进度条的上传*/
	aos_list_t resp_body;
	resp_status = oss_do_put_object_from_file(oss_client_options, &bucket, &object, &file, NULL, NULL, uploadProgressCallback, &resp_headers, &resp_body);
	printf("\n");
    /* 判断上传是否成功。*/
    if (aos_status_is_ok(resp_status)) {
		code = 0;
    }
    /* 释放内存池，相当于释放了请求过程中各资源分配的内存。*/
    aos_pool_destroy(pool);
    /* 释放之前分配的全局资源。*/
    aos_http_io_deinitialize();
    return code;
}

int64_t get_file_size(const char *file_path)
{
    int64_t filesize = -1;
    struct stat statbuff;
    if(stat(file_path, &statbuff) < 0){
        return filesize;
    } else {
        filesize = statbuff.st_size;
    }
    return filesize;
}

int multipart_upload(VodCredential authInfo, UploadInfo uploadInfo, string localfilepath, string videoId, string uploadAddress,
	int multipartUploadOnceSize, void (*uploadProgressCallback) (int64_t, int64_t) = NULL)
{
	int code = -1;
	if (multipartUploadOnceSize <= 0)
		multipartUploadOnceSize = 10*1024*1024;
    /* 在程序入口调用aos_http_io_initialize方法来初始化网络、内存等全局资源。*/
    if (aos_http_io_initialize(NULL, 0) != AOSE_OK) {
        return code;
    }
    /* 用于内存管理的内存池（pool），等价于apr_pool_t。其实现代码在apr库中。*/
    aos_pool_t *pool;
    /* 重新创建一个内存池，第二个参数是NULL，表示没有继承其它内存池。*/
    aos_pool_create(&pool, NULL);
    /* 创建并初始化options，该参数包括endpoint、access_key_id、acces_key_secret、is_cname、curl等全局配置信息。*/
    oss_request_options_t *oss_client_options;
    /* 在内存池中分配内存给options。*/
    oss_client_options = oss_request_options_create(pool);
    /* 初始化Client的选项oss_client_options。*/
	init_options(oss_client_options, uploadInfo);
    /* 初始化参数。*/
    aos_string_t bucket;
    aos_string_t object;
    oss_upload_file_t *upload_file = NULL;
    aos_string_t upload_id;   
    aos_table_t *headers = NULL;
    aos_table_t *complete_headers = NULL;
    aos_table_t *resp_headers = NULL;
    aos_status_t *resp_status = NULL; 
    aos_str_set(&bucket, uploadInfo.bucket.c_str());
    aos_str_set(&object, uploadInfo.fileName.c_str());
    aos_str_null(&upload_id);
    headers = aos_table_make(pool, 1);
    complete_headers = aos_table_make(pool, 1);
    int part_num = 1;
    /* 初始化分片上传，获取一个上传ID(upload_id)。*/
    resp_status = oss_init_multipart_upload(oss_client_options, &bucket, &object, &upload_id, headers, &resp_headers);
    /* 判断分片上传初始化是否成功。*/
    if (aos_status_is_ok(resp_status)) {
        printf("Init multipart upload succeeded, upload_id:%.*s\n", 
               upload_id.len, upload_id.data);
    } else {
        printf("Init multipart upload failed, upload_id:%.*s\n", 
               upload_id.len, upload_id.data);
    }
    /* 上传分片。*/
	//记录开始时间，防止凭证过期
	int64_t start_time = time(NULL);
    int64_t file_length = 0;
    int64_t pos = 0;
	int64_t donePartsCount = 0;
    file_length = get_file_size(localfilepath.c_str());
    while(pos < file_length) {
        upload_file = oss_create_upload_file(pool);
        aos_str_set(&upload_file->filename, localfilepath.c_str());
        upload_file->file_pos = pos;
        pos += multipartUploadOnceSize;
        upload_file->file_last = pos < file_length ? pos : file_length;
        resp_status = oss_upload_part_from_file(oss_client_options, &bucket, &object, &upload_id, part_num++, upload_file, &resp_headers);
		if (aos_status_is_ok(resp_status)) {
			donePartsCount++;
			if(uploadProgressCallback)
				uploadProgressCallback(upload_file->file_last, file_length);
			//上传进度汇报
			reportUploadProgress(authInfo, uploadAddress, upload_id.data, localfilepath, file_length, donePartsCount, multipartUploadOnceSize);
			//判断是否超时，刷新上传凭证
			if(time(NULL) - start_time >= 2500) {
				refreshUploadInfo(authInfo, videoId, uploadInfo);
				refresh_options(oss_client_options, uploadInfo);
				start_time = time(NULL);
			}
        } else {
            printf("Multipart upload part from file failed\n");
        }
    }
	printf("\n");
    oss_list_upload_part_params_t *params = NULL;
    aos_list_t complete_part_list;
    /* 获取已经上传的分片。*/
    params = oss_create_list_upload_part_params(pool);
    params->max_ret = 1000;
    aos_list_init(&complete_part_list);
    resp_status = oss_list_upload_part(oss_client_options, &bucket, &object, &upload_id, params, &resp_headers);
    /* 判断分片列表是否获取成功。*/
    if (aos_status_is_ok(resp_status)) {
    }
    oss_complete_part_content_t *complete_part_content = NULL;
    oss_list_part_content_t *part_content = NULL;
    aos_list_for_each_entry(oss_list_part_content_t, part_content, &params->part_list, node) {
		complete_part_content = oss_create_complete_part_content(pool);
		aos_str_set(&complete_part_content->part_number, part_content->part_number.data);
		aos_str_set(&complete_part_content->etag, part_content->etag.data);
		aos_list_add_tail(&complete_part_content->node, &complete_part_list);
    }
    /* 完成分片上传。*/
    resp_status = oss_complete_multipart_upload(oss_client_options, &bucket, &object, &upload_id,
            &complete_part_list, complete_headers, &resp_headers);
    /* 判断分片上传是否完成。*/
    if (aos_status_is_ok(resp_status)) {
        //printf("Complete multipart upload from file succeeded, upload_id:%.*s\n", 
        //       upload_id.len, upload_id.data);
		code = 0;
    } else {
        //printf("Complete multipart upload from file failed\n");
    }
    /* 释放内存池，相当于释放了请求过程中各资源分配的内存。*/
    aos_pool_destroy(pool);
    /* 释放之前分配的全局资源。*/
    aos_http_io_deinitialize();
    return code;
}

string getJsonValue(string createUploadResult, string key)
{
	string value;
	Json::Reader reader;
	Json::Value root;
	if (reader.parse(createUploadResult, root))
		return root[key].asString();
	return "";
}

string getFileName(string filePath) {
	string fileName;
	fileName = filePath.substr(filePath.find_last_of("/") + 1);
	fileName = fileName.substr(0, fileName.find_first_of("?"));
	return fileName;
}

string getFileDir(string filePath) {
    string fileDir;
    fileDir = filePath.substr(0, filePath.find_last_of("/") + 1);
    return fileDir;
}

bool parseM3u8File(const string& file_name, string& fileContent)
{
    std::ifstream fin(file_name.c_str());
    if(!fin) {
        return false;
    }

    string strline;
    while(std::getline(fin, strline)) {    
		if(strline.find(".ts?") != string::npos)
			strline = getFileName(strline);
		fileContent +=strline + '\n';
    }    

    if(fileContent.size() > 0)
        return true;
    else
        return false;
}

bool parseTsListFromM3u8File(const string& localPath, string prefix, list<string>& tsList, bool trimArgs = true) {
    std::ifstream fin(localPath.c_str());
    if(!fin) {
        return false;
    }
    string strline;
    while(std::getline(fin, strline)) {    
		if(strline.find(".ts") != string::npos) {
			if(trimArgs)
				strline = getFileName(strline);
			tsList.push_back(prefix + strline);
		}
    }    

    if(tsList.size() > 0)
        return true;
    else
        return false;
}

int downloadFile(string webPath, string tmpFile, string &errmsg) {
	httpDownloader downloader(5);
	int code = downloader.get(webPath.c_str(), tmpFile, errmsg);
	if(code != 200)
		printf("download %s failed,httpcode: %d\n", webPath.c_str(), code);
	else
		printf("download %s success\n", webPath.c_str());
	return code;
}

VodApiResponse uploadLocalVideo(VodCredential authInfo, CreateUploadVideoRequest request, string localPath, UploadOptions &uploadOptions)
{
	int64_t fileSize =  get_file_size(localPath.c_str());
	if( fileSize < 0) {
		VodApiResponse result;
		result.httpCode = -1;
		result.result = "the file " + localPath + " doesn't exist";
		return result;
	}

	map<string, string> args;
	if(!request.title.empty()) {
		args["Title"] = request.title;
	}
	if(!request.fileName.empty()) {
		args["FileName"] = request.fileName;
	}
	if(!request.fileSize.empty()) {
		args["FileSize"] = request.fileSize;
	}
	if(!request.description.empty()) {
		args["Description"] = request.description;
	}
	if(!request.coverURL.empty()) {
		args["CoverURL"] = request.coverURL;
	}
	if(!request.cateId.empty()) {
		args["CateId"] = request.cateId;
	}
	if(!request.tags.empty()) {
		args["Tags"] = request.tags;
	}
	if(!request.templateGroupId.empty()) {
		args["TemplateGroupId"] = request.templateGroupId;
	}
	if(!request.workflowId.empty()) {
		args["WorkflowId"] = request.workflowId;
	}
	if(!request.userData.empty()) {
		args["UserData"] = request.userData;
	}
	if(!request.storageLocation.empty()) {
		args["StorageLocation"] = request.storageLocation;
	}
	
	VodApiResponse result = getAcsResponse(authInfo, "CreateUploadVideo", args);
	if(result.httpCode != 200) {
		return result;
	}
	UploadInfo uploadInfo;
	if(getUploadInfo(result.result, uploadInfo, uploadOptions.ecsRegionId))
	{
		result.httpCode = -1;
		return result;
	}
	int uploadCode = -1;
	if(fileSize < uploadOptions.multipartUploadLimit) {
		uploadCode = sample_upload(uploadInfo, localPath, uploadOptions.uploadProgressCallback);
	}
	else {
		string videoId = getJsonValue(result.result, "VideoId");
		string uploadAddress = getJsonValue(result.result, "UploadAddress");
		uploadCode = multipart_upload(authInfo, uploadInfo, localPath, videoId, uploadAddress, uploadOptions.multipartUploadOnceSize, uploadOptions.uploadProgressCallback);
	}

	if(uploadCode != 0) {
		result.httpCode = uploadCode;
	}
	return result;
}

VodApiResponse uploadWebVideo(VodCredential authInfo, CreateUploadVideoRequest request, string webPath, UploadOptions &uploadOptions) 
{
	VodApiResponse result;
	string tmpFile = uploadOptions.tmpDir + "/" + getFileName(webPath);
	string errmsg;
    int code = downloadFile(webPath, tmpFile, errmsg);
	if(code != 200) {
		result.httpCode = code;
		result.result = "download " + webPath + " error, errmsg: " +errmsg;
	}
	else {
		result = uploadLocalVideo(authInfo, request, tmpFile, uploadOptions);
	}
	unlink(tmpFile.c_str());
	return result;
}

VodApiResponse uploadLocalM3u8(VodCredential authInfo, CreateUploadVideoRequest request, string m3u8Path, list<string> tsPathList, UploadOptions &uploadOptions)
{
	int64_t fileSize =  get_file_size(m3u8Path.c_str());
	if( fileSize < 0) {
		VodApiResponse result;
		result.httpCode = -1;
		result.result = "the file " + m3u8Path + " doesn't exist";
		return result;
	}

	map<string, string> args;
	if(!request.title.empty()) {
		args["Title"] = request.title;
	}
	if(!request.fileName.empty()) {
		args["FileName"] = request.fileName;
	}
	if(!request.fileSize.empty()) {
		args["FileSize"] = request.fileSize;
	}
	if(!request.description.empty()) {
		args["Description"] = request.description;
	}
	if(!request.coverURL.empty()) {
		args["CoverURL"] = request.coverURL;
	}
	if(!request.cateId.empty()) {
		args["CateId"] = request.cateId;
	}
	if(!request.tags.empty()) {
		args["Tags"] = request.tags;
	}
	if(!request.templateGroupId.empty()) {
		args["TemplateGroupId"] = request.templateGroupId;
	}
	if(!request.workflowId.empty()) {
		args["WorkflowId"] = request.workflowId;
	}
	if(!request.userData.empty()) {
		args["UserData"] = request.userData;
	}
	if(!request.storageLocation.empty()) {
		args["StorageLocation"] = request.storageLocation;
	}
	
	VodApiResponse result = getAcsResponse(authInfo, "CreateUploadVideo", args);
	if(result.httpCode != 200) {
		return result;
	}
	UploadInfo uploadInfo;
	if(getUploadInfo(result.result, uploadInfo, uploadOptions.ecsRegionId)) {
		result.httpCode = -1;
		return result;
	}
	string videoId = getJsonValue(result.result, "VideoId");
	/*注意:m3u8uploadInfo与ts的fileName不同*/
	string uploadDir = getFileDir(uploadInfo.fileName);
	if(tsPathList.size() == 0) {
		parseTsListFromM3u8File(m3u8Path, getFileDir(m3u8Path), tsPathList);
	}
	int start_time = time(NULL);
	//循环上传ts分片
	int uploadCode = -1;
	for(list<string>::iterator itr = tsPathList.begin(); itr != tsPathList.end(); itr++) {
		string tsFileName = uploadDir + getFileName(*itr);
		UploadInfo tsUploadInfo = uploadInfo;
		//ts文件采用相同路径,文件名不同
		tsUploadInfo.fileName = tsFileName;
		uploadCode = sample_upload(tsUploadInfo, *itr, uploadOptions.uploadProgressCallback);
		if (uploadCode != 0)
			break;
		//超时刷新上传凭证
		if(time(NULL) - start_time > 2500) {
			refreshUploadInfo(authInfo, videoId, uploadInfo);
			start_time = time(NULL);
		}
	}
	//ts上传失败则不进行m3u8上传
	if(uploadCode != 0) {
		result.httpCode = uploadCode;
		return result;
	}
	//重置uploadCode
	//uploadCode = sample_upload(uploadInfo, m3u8Path, uploadOptions.uploadProgressCallback);
	string m3u8FileContent;
	parseM3u8File(m3u8Path, m3u8FileContent);
	uploadCode = buffer_upload(uploadInfo, m3u8FileContent, uploadOptions.uploadProgressCallback);
	if(uploadCode != 0) {
		result.httpCode = uploadCode;
	}
	return result;
}

VodApiResponse uploadWebM3u8(VodCredential authInfo, CreateUploadVideoRequest request, string webPath, list<string> tsWebPathList, UploadOptions &uploadOptions)
{
	VodApiResponse result;
	list<string> tsLocalList;
	string tmpFile = uploadOptions.tmpDir + "/" + getFileName(webPath);
	string errmsg;
	int code = downloadFile(webPath, tmpFile, errmsg);
	if(code != 200) {
		result.httpCode = code;
		result.result = "download " + webPath + " error, errmsg: " + errmsg;
	}
	else {
		//循环下载ts文件
		if(tsWebPathList.size() == 0) {
			parseTsListFromM3u8File(tmpFile, getFileDir(webPath), tsWebPathList, false);
		}
		for(list<string>::iterator itr = tsWebPathList.begin(); itr != tsWebPathList.end(); itr++) {
			string tsTmpFile = uploadOptions.tmpDir + "/" + getFileName(*itr);
			code = downloadFile(*itr, tsTmpFile, errmsg);
			if(code == 200)
				tsLocalList.push_back(tsTmpFile);
			else { 
				result.httpCode = code;
				result.result = "download " + *itr + " error, errmsg: " + errmsg;
				break;
			}
		}
	}
	if(code == 200) {
		result = uploadLocalM3u8(authInfo, request, tmpFile, tsLocalList, uploadOptions);
	}
	for(list<string>::iterator itr = tsLocalList.begin(); itr != tsLocalList.end(); itr++) {
		unlink((*itr).c_str());
	}	
	unlink(tmpFile.c_str());
	return result;
}

VodApiResponse uploadLocalImage(VodCredential authInfo, CreateUploadImageRequest request, string localPath, UploadOptions &uploadOptions)
{
	int64_t fileSize =  get_file_size(localPath.c_str());
	if( fileSize < 0) {
		VodApiResponse result;
		result.httpCode = -1;
		result.result = "the file " + localPath + " doesn't exist";
		return result;
	}

	map<string, string> args;
	if(!request.imageType.empty()) {
		args["ImageType"] = request.imageType;
	}
	if(!request.imageExt.empty()) {
		args["ImageExt"] = request.imageExt;
	}
	if(!request.title.empty()) {
		args["Title"] = request.title;
	}
	if(!request.tags.empty()) {
		args["Tags"] = request.tags;
	}
	if(!request.cateId.empty()) {
		args["CateId"] = request.cateId;
	}
	if(!request.description.empty()) {
		args["Description"] = request.description;
	}
	if(!request.userData.empty()) {
		args["UserData"] = request.userData;
	}
	if(!request.storageLocation.empty()) {
		args["StorageLocation"] = request.storageLocation;
	}
	
	VodApiResponse result = getAcsResponse(authInfo, "CreateUploadImage", args);
	if(result.httpCode != 200) {
		return result;
	}
	UploadInfo uploadInfo;
	if(getUploadInfo(result.result, uploadInfo, uploadOptions.ecsRegionId))
	{
		result.httpCode = -1;
		return result;
	}
	string videoId = getJsonValue(result.result, "VideoId");
	int uploadCode = -1;
	uploadCode = sample_upload(uploadInfo, localPath, uploadOptions.uploadProgressCallback);

	if(uploadCode != 0) {
		result.httpCode = uploadCode;
	}
	return result;
}

VodApiResponse uploadWebImage(VodCredential authInfo, CreateUploadImageRequest request, string webPath, UploadOptions &uploadOptions) 
{
	VodApiResponse result;
	string tmpFile = uploadOptions.tmpDir + "/" + getFileName(webPath);
	string errmsg;
    int code = downloadFile(webPath, tmpFile, errmsg);
	if(code != 200) {
		result.httpCode = code;
		result.result = "download " + webPath + " error, errmsg: " +errmsg;
	}
	else {
		result = uploadLocalImage(authInfo, request, tmpFile, uploadOptions);
	}
	unlink(tmpFile.c_str());
	return result;
}

VodApiResponse uploadLocalAttachedMedia(VodCredential authInfo, CreateUploadAttachedMediaRequest request, string localPath, UploadOptions &uploadOptions)
{
	int64_t fileSize =  get_file_size(localPath.c_str());
	if( fileSize < 0) {
		VodApiResponse result;
		result.httpCode = -1;
		result.result = "the file " + localPath + " doesn't exist";
		return result;
	}

	map<string, string> args;
	if(!request.businessType.empty()) {
		args["BusinessType"] = request.businessType;
	}
	if(!request.mediaExt.empty()) {
		args["MediaExt"] = request.mediaExt;
	}
	if(!request.title.empty()) {
		args["Title"] = request.title;
	}
	if(!request.cateId.empty()) {
		args["CateId"] = request.cateId;
	}
	if(!request.fileName.empty()) {
		args["FileName"] = request.fileName;
	}
	if(!request.fileSize.empty()) {
		args["FileSize"] = request.fileSize;
	}
	if(!request.tags.empty()) {
		args["Tags"] = request.tags;
	}
	if(!request.description.empty()) {
		args["Description"] = request.description;
	}
	if(!request.userData.empty()) {
		args["UserData"] = request.userData;
	}
	if(!request.storageLocation.empty()) {
		args["StorageLocation"] = request.storageLocation;
	}
	
	VodApiResponse result = getAcsResponse(authInfo, "CreateUploadAttachedMedia", args);
	if(result.httpCode != 200) {
		return result;
	}
	UploadInfo uploadInfo;
	if(getUploadInfo(result.result, uploadInfo, uploadOptions.ecsRegionId))
	{
		result.httpCode = -1;
		return result;
	}
	string videoId = getJsonValue(result.result, "VideoId");
	int uploadCode = -1;
	uploadCode = sample_upload(uploadInfo, localPath, uploadOptions.uploadProgressCallback);

	if(uploadCode != 0) {
		result.httpCode = uploadCode;
	}
	return result;
}

VodApiResponse uploadWebAttachedMedia(VodCredential authInfo, CreateUploadAttachedMediaRequest request, string webPath, UploadOptions &uploadOptions) 
{
	VodApiResponse result;
	string tmpFile = uploadOptions.tmpDir + "/" + getFileName(webPath);
	string errmsg;
    int code = downloadFile(webPath, tmpFile, errmsg);
	if(code != 200) {
		result.httpCode = code;
		result.result = "download " + webPath + " error, errmsg: " +errmsg;
	}
	else {
		result = uploadLocalAttachedMedia(authInfo, request, tmpFile, uploadOptions);
	}
	unlink(tmpFile.c_str());
	return result;
}
