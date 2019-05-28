/**
 * OPENAPI_UTIL
 *
 * Aliyun VoD's Request Head File, which wraps parameters and request to VoD.
 * Users could pass the vodCredential、apiName and api parameters to getAcsResponse function 
 * For more details, please check out the VoD API document: https://help.aliyun.com/document_detail/60574.html
 */
#ifndef OPENAPI_UTIL
#define OPENAPI_UTIL

#include <stdio.h>
#include <string>
#include <map>
#include <errno.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <uuid/uuid.h>
#include <openssl/hmac.h> 

typedef struct {
    int httpCode;
    std::string result;
} VodApiResponse;

typedef struct {
	std::string regionId;
	std::string accessKeyId;
	std::string accessKeySecret;
	std::string securityToken;
} VodCredential;

VodApiResponse getAcsResponse(VodCredential authInfo, std::string apiName, std::map<std::string, std::string> args);

int httpGet(std::string url, std::string &result,int timeout =5);

std::string getRequestUrl(VodCredential authInfo, std::string apiName, std::map<std::string, std::string> requestArgs);

#endif

