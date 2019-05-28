#ifndef HTTP_CLIENT_H
#define HTTP_CLIENT_H

#include <string>
#include <string.h>
#include <curl/curl.h>
#include <errno.h>

//内核使用curl的httpclient
class httpDownloader
{
public:
	httpDownloader(int timeout=3){
		curl = NULL;
		init(timeout);
	};

	~httpDownloader()
	{ 
		if(NULL != curl)
		{
			curl_easy_cleanup(curl); 
			curl = NULL;
		}
	};


	int init(int _timeout=3)
	{
		curl = curl_easy_init();
		if( NULL == curl)
			return -1;
		//数据回调函数
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, html_write_cb);
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);//允许跳转
		curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 2);//跳转的层次
		curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);//

		if( _timeout > 0) {
			curl_easy_setopt(curl, CURLOPT_TIMEOUT, _timeout);
			curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, _timeout);
		}
		curl_easy_setopt(curl, CURLOPT_MAXCONNECTS, 32);
		//保存连接的个数(能够在下次的使用保存的连接)
		return 0;
	};

	int get(const char* url, std::string downloadPath, std::string& resultmsg)
	{
		CURLcode res;
		curl_easy_setopt(curl, CURLOPT_URL, url);

		FILE *fp = fopen(downloadPath.c_str(), "w+");
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);

		curl_easy_setopt(curl, CURLOPT_HTTPGET,1);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);

		char errmsg[CURL_ERROR_SIZE];
		curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errmsg);
		int retry=2;


		while(1)
		{
			retry--;
			res = curl_easy_perform(curl);
			if(res == CURLE_OK || retry == 0)
				break;
		}
		fclose(fp);

		if(res == CURLE_OK)
		{
			long code;
			curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &code);
				return code;
		}  
		else
		{
			resultmsg = errmsg;
			return -1;
		}
	}

	static size_t html_write_cb(void *ptr,size_t size,size_t nmemb,void *stream)
	{
		size_t written = fwrite(ptr, size, nmemb, (FILE *)stream);  
		return written;  
	}

private:
	CURL *curl;
	
};


#endif
