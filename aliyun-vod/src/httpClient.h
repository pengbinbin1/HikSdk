#ifndef HTTP_CLIENT_H
#define HTTP_CLIENT_H

#include <string>
#include <string.h>
#include <curl/curl.h>
#include <errno.h>

//内核使用curl的httpclient
class CHttpclient
{
public:
	CHttpclient(int timeout=3){
		curl = NULL;
		init(timeout);
	};

	~CHttpclient()
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

	int get(const char* url, std::string& html)
	{
		CURLcode res;
		curl_easy_setopt(curl, CURLOPT_URL, url);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &html);

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

		if(res == CURLE_OK)
		{
			long code = -1;
			curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &code);
			return code;
		}  
		else
		{
			html = errmsg;
			return -1;
		}
	}

	int post_proxy(const char* url,const char *post_data,std::string &html,const char *bak_url,
		const char *proxy=NULL,const char *auth=NULL)
	{
		//1.不用代理

		if( 0 == post(url,post_data,html) )
		{
			return 0;
		}
		printf("[post_proxy]no use proxy post:%s ,data:%s ",url,post_data);
		//2.使用bak_url
		if( (NULL != bak_url) && (0 == post(bak_url,post_data,html)) )
		{
			return 0;
		}
		else
			printf("[post_proxy]no use proxy post_bak_url:%s ,data:%s ",url,post_data);

		//3.使用代理
		if( NULL != proxy)
		{
			if( 0 == post(url,post_data,html,proxy,auth) )
			{
				return 0;
			}
			printf("[post_proxy]use proxy[%s,%s] post:%s ,data:%s ",proxy,auth,url,post_data);
			//2.使用bak_url
			if( (NULL != bak_url) && (0 == post(bak_url,post_data,html,proxy,auth)) )
			{
				return 0;
			}
			else
				printf("[post_proxy]use proxy[%s,%s] post_bak_url:%s ,data:%s ",
					proxy,auth,url,post_data);
		}
		return -1;
	}

	int post(const char* url,const char *post_data,std::string &html,
		const char *proxy=NULL,const char *auth=NULL)
	{
		CURLcode res;

		curl_easy_setopt(curl, CURLOPT_URL, url);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &html); 

		curl_easy_setopt(curl, CURLOPT_PROXY, proxy);
		curl_easy_setopt(curl, CURLOPT_PROXYUSERPWD, auth);


		//设置post的参数
		curl_easy_setopt(curl, CURLOPT_POST, 8);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_data);

		res = curl_easy_perform(curl);

		if(CURLE_OK == res)
		{
			long code;
			CURLcode res1;
			res1 = curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &code);
			printf("[http_client::post]url:%s,post_data:%s,retcode=%ld",
				url,post_data,code);
			if( (res1 == CURLE_OK ) && (code == 200) )
				return 0;

		}
		else
		{
			printf("[http_client::post]url:%s,post:%s ,[%s,%s]curl code=%s ,error..=%d,retry!",
				url,post_data,proxy,auth,
				curl_easy_strerror(res),errno);
			res = curl_easy_perform(curl);
			if(CURLE_OK == res)
			{
				long code;
				CURLcode res1;
				res1 = curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &code);

				printf("[http_client::post]url:%s,post_data:%s,retcode=%ld"
					,url,post_data,code);
				if( (res1 == CURLE_OK ) && (code == 200) )
					return 0;
			}
			else
			{
				printf("[http_client::post]url:%s,post:%s ,[%s,%s]curl code=%s error..=%d!",
					url,post_data,proxy,auth,
					curl_easy_strerror(res),errno);
				return -1;
			}
		}
		printf("[http_client::post]url:%s,post:%s ,[%s,%s]curl code=%s error..=%d!",
			url,post_data,proxy,auth,curl_easy_strerror(res),errno);
		return -1;
	}
	static size_t html_write_cb(void *ptr,size_t size,size_t nmemb,void *stream)
	{
		char *buff = new char[size*nmemb + 1]; 
		std::string* html = (std::string*)stream;
		memcpy(buff, ptr, size * nmemb);
		buff[size * nmemb] = 0;
		html->append(buff, nmemb);
		delete[] buff;
		return nmemb * size;
	}

private:
	CURL *curl;
};


#endif
