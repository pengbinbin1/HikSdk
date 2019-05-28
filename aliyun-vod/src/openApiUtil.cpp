#include "httpClient.h"
#include "signatureUtil.h"
#include "openApiUtil.h"

using namespace std;

string getUTCtime()
{
	time_t t = time(NULL);
	struct tm gmt;
	gmtime_r(&t, &gmt);

	char tmp[256] = {0};
	strftime(tmp, sizeof(tmp)-1, "%Y-%m-%dT%H:%M:%SZ", &gmt);
	return tmp;
}

string getUUID()
{
    uuid_t uuid;
    char str[36+1] = {0};
    uuid_generate_time_safe(uuid);
    uuid_unparse(uuid, str);
    return str;
}

int httpGet(string url, string &result, int timeout)
{
	CHttpclient httpclient(timeout);
	int httpCode = httpclient.get(url.c_str(), result);
	return httpCode;
}

string replaceAll(string str, string oldvalue, string newvalue)
{
	while(str.find(oldvalue) != string::npos)
	{
		str = str.replace(str.find(oldvalue), oldvalue.size(), newvalue);
	}
	return str;
}

string percentEncode(string str)
{
	str = UrlEncode(str);
	str = replaceAll(str, "+", "%20");
	str = replaceAll(str, "*", "%2A");
	str = replaceAll(str, "%7E", "~");
	return str;
	
}

string hmacSHA1Signature(string accessKeySecret, string strToSign)
{
	unsigned int digest_len = 0;
	accessKeySecret += "&";
	unsigned char digest[EVP_MAX_MD_SIZE + 1] = {0};
	HMAC(EVP_sha1(), accessKeySecret.c_str(), accessKeySecret.size(), (const unsigned char*)strToSign.c_str(), strToSign.size(), digest, &digest_len);
	string result;
	//注意可能中间有\0字符,必须使用append
	result.append((char *)digest, digest_len);	
	result = base64_encode(result.c_str(), result.length());
	return result;
}

string getSignature(string method, string getQueryStr, string accessKeySecret)
{
	string strToSign = method + "&%2F&" + percentEncode(getQueryStr);
	string signature = hmacSHA1Signature(accessKeySecret, strToSign);
	return signature;
}

string getQueryStr(map<string, string> args)
{
	string getArgsStr;
	for(map<string, string>::iterator itr = args.begin(); itr != args.end(); itr++)
	{
		string key = percentEncode(itr->first);
		string value = percentEncode(itr->second);
		if(getArgsStr.size() != 0)
		{
			getArgsStr += "&";
		}
		getArgsStr += key + "=" + value;
	}
	return getArgsStr;
}

string getRequestUrl(VodCredential vodCredential, string apiName, map<string, string> requestArgs)
{
	string domain = "https://vod." + vodCredential.regionId + ".aliyuncs.com?";

	map<string, string> allRequestArgs;
	allRequestArgs["Format"] = "JSON"; 
	allRequestArgs["Version"] = "2017-03-21"; 
	allRequestArgs["AccessKeyId"] = vodCredential.accessKeyId; 
	allRequestArgs["SignatureMethod"] = "HMAC-SHA1"; 
	allRequestArgs["Timestamp"] = getUTCtime(); 
	allRequestArgs["SignatureVersion"] = "1.0"; 
	allRequestArgs["SignatureNonce"] = getUUID(); 
	if(!vodCredential.securityToken.empty()) {
		allRequestArgs["SecurityToken"] = vodCredential.securityToken;
	}
	allRequestArgs["Action"] = apiName;
	for(map<string, string>::iterator itr = requestArgs.begin(); itr != requestArgs.end(); itr++)
	{
		allRequestArgs[itr->first] = itr->second;	
	}
	string queryStr = getQueryStr(allRequestArgs);
	string signature = getSignature("GET", queryStr, vodCredential.accessKeySecret);
	string finalUrl = domain + queryStr +  "&" + percentEncode("Signature") + "=" + percentEncode(signature);
	return finalUrl;
}

VodApiResponse getAcsResponse(VodCredential vodCredential, std::string apiName, std::map<std::string, std::string> args)
{
	VodApiResponse result;
	string url = getRequestUrl(vodCredential, apiName, args);
	string html;
	int httpCode = httpGet(url, html);
	result.httpCode = httpCode;
	result.result = html;
	return result;
}

