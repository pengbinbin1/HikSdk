#include<string.h>
#include<unistd.h>
#include "HIK_SDK.h"

void PlayBackCallback(LONG lPlayHandle,DWORD dwDataType,BYTE  *pBuffer,DWORD  dwBufSize,DWORD  dwUser)
{
	if (dwBufSize > 0)
	{
		printf("dataType = %d,datasize = %d\n",dwDataType,dwBufSize);
	}else
	{
		printf("datasize = 0\n");
	}
}
int main()
{
	int ret =0;
	NET_DVR_USER_LOGIN_INFO login;
	strcpy(login.sDeviceAddress,"10.58.123.42");
	strcpy(login.sUserName,"admin");
	strcpy(login.sPassword,"abc123456");
	login.wPort = 8000;
	login.bUseAsynLogin = FALSE;

	NET_DVR_DEVICEINFO_V40 devinfo = {0};
	ret = HIK_SDK::GetHikSdkInstance().Init(login,devinfo);
	if (ret!= 0)
	{
		printf("Init failed,ret = %d\n",ret);
		return ret;
	}

	NET_DVR_TIME startTime;
	startTime.dwYear = 2018;
	startTime.dwMonth = 9;
	startTime.dwDay = 26;
	startTime.dwHour = 10;
	startTime.dwMinute = 0;
	startTime.dwSecond = 0;
	
	NET_DVR_TIME stopTime;
	stopTime.dwYear = 2018;
	stopTime.dwMonth = 9;
	stopTime.dwDay = 26;
	stopTime.dwHour =10;
	stopTime.dwMinute = 3;
	stopTime.dwSecond =0;
	LONG chanl = 33;
	char* fileName = "./test.mp4";
//#define SAVEFILE_BY_TIME
#define PLAYBACK_BY_NAME

#ifdef SAVEFILE_BY_TIME
	ret = HIK_SDK::GetHikSdkInstance().SaveFileByTime(startTime,stopTime,chanl,fileName);
	if (ret !=0)
	{
		printf("SavefileByTime failed,ret = %d\n",ret);
		return ret;
	}
#endif

#ifdef PLAYBACK_BY_TIME
	ret = HIK_SDK::GetHikSdkInstance().PlayBackByTime(startTime,stopTime,chanl,PlayBackCallback);
	if (ret!=0)
	{
		printf("Play back by time failed,err = %d\n",ret);
		return ret;
	}
#endif

	NET_DVR_FIND_DATA file_data;
	ret = HIK_SDK::GetHikSdkInstance().FindFile(startTime,stopTime,chanl,file_data);
	if (ret != 0)
	{
		printf("@@@ Find file filed\n");
	}else{
		printf("@@@ find file success\n");
		if (file_data.dwFileSize>0)
		{
			printf("filename = %s, filesize = %d\n",file_data.sFileName,file_data.dwFileSize);
		}
	}

#ifdef SAVEFILE_BY_NAME
	ret = HIK_SDK::GetHikSdkInstance().SaveFileByName(file_data.sFileName,"./peng.mp4");
	if (ret !=0)
	{
		printf("@@@@ savefile by name failed,err = %d\n",ret);
	}
#endif

#ifdef PLAYBACK_BY_NAME
	ret = HIK_SDK::GetHikSdkInstance().PlayBackByName(file_data.sFileName,PlayBackCallback);
	if (ret!=0)
	{
		printf("@@@@ play back by name failed,err = %d\n",ret);
	}
	sleep(10);
#endif

	ret = HIK_SDK::GetHikSdkInstance().Destory();
	if (ret!= 0)
	{
		printf("destroy failed,err = %d\n",ret);
		return ret;
	}

	return 0;
}