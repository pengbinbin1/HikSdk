#ifndef __HIK_SDK_H__
#define __HIK_SDK_H__
#endif

#include <stdio.h>
#include "HCNetSDK.h"
class HIK_SDK{

private:	
	HIK_SDK();
	~HIK_SDK();
public:
	static HIK_SDK &GetHikSdkInstance(){
		static HIK_SDK  instance ;
		return instance;
	}
public:
	/*
		Description: Get recoder file from nvr by time
		paramS:
			[in] loginInfo,ip,port,username,password... 
			[out] deviceInfo
		return:
		    errcode 0:success else:failed
	*/
	int Init(NET_DVR_USER_LOGIN_INFO loginInfo,NET_DVR_DEVICEINFO_V40& deviceInfo);

	/*
		Description: Get recoder file from nvr by time
		paramS:
			[in] startTime: 
			[in] stopTime
			[in] chan: channel number
			[in] fileName: filename include the path
		return:
		    errcode 0:success else:failed
	*/
	int SaveFileByTime(NET_DVR_TIME StartTime,NET_DVR_TIME StopTime,LONG chanl,char* fileName);

	/*
		Description:Playback recoder from nvr by time
		paramS:
			[in] startTime: 
			[in] stopTime
			[in] chan: channel number
		return:
		    errcode 0:success else:failed
	*/
	int PlayBackByTime(NET_DVR_TIME StartTime,NET_DVR_TIME StopTime,LONG chanl);

	/*
		Description: Release Source
		paramS:
		return:
			errcode 0:success else:failed
	*/
	int Destory();
private:
	LONG m_UserID;

};