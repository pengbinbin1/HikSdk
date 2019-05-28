// HIK_SDK.cpp : Defines the entry point for the console application.


#include "HIK_SDK.h"
#include <unistd.h>
static int init_num =0;
static int uninit_num =0;

HIK_SDK::HIK_SDK(){
	m_UserID = -1;

}

HIK_SDK::~HIK_SDK(){
	m_UserID = -1;
}

int HIK_SDK::Init(NET_DVR_USER_LOGIN_INFO loginInfo,NET_DVR_DEVICEINFO_V40& deviceInfo)
{
	int err =0;
    init_num +=1;
    printf("has call Init %d times\n",init_num);
	// Init the environment 
	if (!NET_DVR_Init())
	{
		err = NET_DVR_GetLastError();
		printf("@@@log out fialed,err= %d\n",err);
		return err;
	}

	//Print the version info
	unsigned int uiVersion = NET_DVR_GetSDKBuildVersion();
	char strTemp[1024] = {0};
	sprintf(strTemp, "@@@@HCNetSDK V%d.%d.%d.%d\n", \
		(0xff000000 & uiVersion)>>24, \
		(0x00ff0000 & uiVersion)>>16, \
		(0x0000ff00 & uiVersion)>>8, \
		(0x000000ff & uiVersion));
	printf(strTemp);

	// set log file
	NET_DVR_SetLogToFile(3, "./sdkLog");

	//set time when connect
	bool setTime = NET_DVR_SetConnectTime(1000,5);
	if (setTime)
	{
            printf("set Time success\n");
	}
	else
	{
            printf("set time failed,err = %d,line = %d\n",NET_DVR_GetLastError(),__LINE__);
            return -1;
	}

	deviceInfo = {0};
	m_UserID = NET_DVR_Login_V40(&loginInfo,&deviceInfo);
	if (m_UserID<0)
	{
		err= NET_DVR_GetLastError();
                printf("@@@login failed err = %d,line=%d\n ",err,__LINE__);
		return err;
	}

	return 0;
}

int HIK_SDK::SaveFileByTime(NET_DVR_TIME startTime,NET_DVR_TIME stopTime,LONG chanl,char* fileName)
{
	int err = 0;
	if (m_UserID <0)
	{
		printf("@@@ m_UserID is invalied\n");
		return -1;
	}
	int hdPlayBack = NET_DVR_GetFileByTime(m_UserID,chanl,&startTime,&stopTime,fileName);
	if (hdPlayBack<0)
	{
		err = NET_DVR_GetLastError();
		printf("line = %d,GetFileByTime Failed,err = %d\n",__LINE__,err);
		return err;
	}

	if(!NET_DVR_PlayBackControl(hdPlayBack, NET_DVR_PLAYSTART, 0, NULL))
	{
		err = NET_DVR_GetLastError();
		printf("@@@play back control failed err = %d\n",err);
		return err;
	}

	int pos = 0;
	while (pos >=0 && pos < 100)
	{
		pos = NET_DVR_GetDownloadPos(hdPlayBack);
		printf("progress = %d\n",pos);
		sleep(3);
	}

	if (!NET_DVR_StopGetFile(hdPlayBack))
	{
		err = NET_DVR_GetLastError();
		printf("@@@stop get file failed err = %d\n",err);
		return err;
	}

	if (pos<0 || pos>100)
	{
		err = NET_DVR_GetLastError();
		printf("@@@get pos  failed err = %d\n",err);
		return err;
	}
	return 0;
}

int HIK_SDK::PlayBackByTime(NET_DVR_TIME startTime,NET_DVR_TIME stopTime,LONG chanl,PlaybackCallBack callback)
{
	int err = 0;
	if (m_UserID < 0)
	{
		printf("@@@userID is invalid\n");
		return -1;
	}

	int hdPlayBack =  NET_DVR_PlayBackByTime(m_UserID,chanl,&startTime,&stopTime,NULL);
	if (hdPlayBack<0)
	{
		err = NET_DVR_GetLastError();
		printf("@@@PlayBackByTime Failed,err = %d\n",err);
		return err;
	}

	if (!NET_DVR_PlayBackControl(hdPlayBack,NET_DVR_PLAYSTART,0,NULL))
	{
		err = NET_DVR_GetLastError();
		printf("@@@PlayBackControl Failed,err = %d\n",err);
		return err;
	}else
	{
		printf("@@@ start playback success\n");
	}

	//set call back function
	if(!NET_DVR_SetPlayDataCallBack(hdPlayBack,callback,1))
	{
		err = NET_DVR_GetLastError();
		printf("@@@NET_DVR_SetPlayDataCallBack failed,err = %d\n",err);
		return err;
	}else{
		printf("@@@set playback callback success\n");
	}


	return 0;
}

int HIK_SDK::Destory(){
        int err =0;
        uninit_num +=1;
        printf("has call Uninit %d times\n",uninit_num);
	if (!NET_DVR_Logout_V30(m_UserID))
	{
                err = NET_DVR_GetLastError();
                printf("@@@log out failed,err= %d,line = %d\n",err,__LINE__);
		return err;
	}

        if(!NET_DVR_Cleanup())
        {
            err = NET_DVR_GetLastError();
            printf("@@@clean up  fialed,err= %d,line = %d\n",err,__LINE__);
            return err;
        }

	return 0;
}
int HIK_SDK::FindFile(NET_DVR_TIME startTime,NET_DVR_TIME stopTime,LONG chanl, NET_DVR_FIND_DATA& struFileData)
{
	int err = 0;
	// start first find,default is all type file,0xff
	int lFindHandle = NET_DVR_FindFile(m_UserID, chanl, 0xff, &startTime, &stopTime);
	if(lFindHandle < 0)
	{
		err = NET_DVR_GetLastError();
		printf("@@@find file failed,last error %d\n",err);
		return err;
	}


	while(true)
	{
		int result = NET_DVR_FindNextFile(lFindHandle, &struFileData);
		if(result == NET_DVR_ISFINDING)
		{
			//nsleep(0, 20000000L); // sleep for 2/100 second;
			printf("@@@@Is Finding the video file,please wait\n");
			continue;
		}
		else if(result == NET_DVR_FILE_SUCCESS)
		{
			//char strFileName[256] = {0};
			//sprintf(strFileName, "./%s", struFileData.sFileName);
// 			printf("%s, %d, %d, %d,%d,%d-----%d,%d,%d,%d,%d.", struFileData.sFileName, 
// 				struFileData.struStartTime.dwYear, struFileData.struStartTime.dwMonth,  struFileData.struStartTime.dwDay,struStartTime.dwHour,struStartTime.dwMinute,
// 				struFileData.struStopTime.dwYear, struFileData.struStopTime.dwMonth,  struFileData.struStopTime.dwDay,struStopTime.dwHour,struStopTime.dwMinute);
// 			//saveFile(lUserID, struFileData.sFileName,  strFileName);
			printf("@@@Find file success\n");
			break;
		}
		else if(result == NET_DVR_FILE_NOFIND || result == NET_DVR_NOMOREFILE)
		{	
			printf("@@@can not find file\n");
			break;
		}
		else
		{
			printf("@@@find file fail for illegal get file state\n");
			return -1;
		}

	}

	NET_DVR_FindClose(lFindHandle);
	return 0;
}

int HIK_SDK::SaveFileByName( char* NVRFileName, char* destFileName )
{
	int err = 0;
	int hdPlayback = NET_DVR_GetFileByName(m_UserID,NVRFileName,destFileName);
	if (hdPlayback <0)
	{
		err = NET_DVR_GetLastError();
		printf("@@@Get file by name failed,err=%d\n",err);
		return err;
	}

	//set format to PS/TS/RTP
// 	int format = 3;
// 	if (!NET_DVR_PlayBackControl_V40(hdPlayback,NET_DVR_SET_TRANS_TYPE,&format,sizeof(format),NULL,NULL));
// 	{
// 		err = NET_DVR_GetLastError();
// 		printf("@@@Set format failed,err = %d\n",err);
// 		return err;
// 	}

	//start download file from nvr
	if (!NET_DVR_PlayBackControl(hdPlayback,NET_DVR_PLAYSTART,0,NULL))
	{
		err = NET_DVR_GetLastError();
		printf("@@@Start download filed,err=%d\n",err);
		return err;
	}

	//wait download succ
	int pos = 0;
	while (pos >=0 && pos < 100)
	{
		pos = NET_DVR_GetDownloadPos(hdPlayback);
		printf("progress = %d\n",pos);
		sleep(3);
	}

	if (!NET_DVR_StopGetFile(hdPlayback))
	{
		err = NET_DVR_GetLastError();
		printf("@@@Stop download filed,err=%d\n",err);
		return err;
	}
	return 0;
}

int HIK_SDK::PlayBackByName(char* NVRFileName,PlaybackCallBack callback)
{
	int err= 0;
	HWND hWnd = {0};
	int hdPlayBack = NET_DVR_PlayBackByName(m_UserID,NVRFileName,hWnd);
	if (hdPlayBack<0)
	{
		err = NET_DVR_GetLastError();
		printf("@@@Play back by name failed,err =%d\n",err);
		return err;
	}

	//start play back by name 
	if (!NET_DVR_PlayBackControl(hdPlayBack,NET_DVR_PLAYSTART,0,NULL))
	{
		err = NET_DVR_GetLastError();
		printf("@@@Start play back by name failed,err =%d\n",err);
		return err;
	}

	//set call back function
	if (!NET_DVR_SetPlayDataCallBack(hdPlayBack,callback,1))
	{
		err = NET_DVR_GetLastError();
		printf("@@@Set call back failed,err =%d\n",err);
		return err;
	}
	return 0;
}

int HIK_SDK::GetDVRConfig(DWORD command, LONG group, NET_DVR_IPPARACFG_V40 &deviceCfg){
    int ret =0;
    DWORD returned = 0;
    if(!NET_DVR_GetDVRConfig(m_UserID,command,group,&deviceCfg,sizeof (NET_DVR_IPPARACFG_V40),&returned))
    {
        ret = NET_DVR_GetLastError();
        printf("@@@get cfg from dvr failed.err= %d,line = %d\n",ret,__LINE__);
        return ret;
    }

    return  ret;

}

int HIK_SDK::GetPort(DWORD command, NET_DVR_NAT_CFG &deviceCfg){
    int ret = 0;
    DWORD returned = 0;
    if (!NET_DVR_GetDVRConfig(m_UserID,command,0,&deviceCfg,sizeof (NET_DVR_NAT_CFG),&returned))
    {
        ret = NET_DVR_GetLastError();
        printf("@@@get cfg from dvr failed.err= %d,line = %d\n",ret,__LINE__);
        return ret;
    }

    return ret;
}
