#ifndef __HIK_SDK_H__
#define __HIK_SDK_H__
#endif

#include <stdio.h>
#include "HCNetSDK.h"

typedef void (*PlaybackCallBack)(LONG lPlayHandle,DWORD dwDataType,BYTE  *pBuffer,DWORD  dwBufSize,DWORD  dwUser);
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
	int SaveFileByTime(NET_DVR_TIME startTime,NET_DVR_TIME stopTime,LONG chanl, char* fileName);

	/*
		Description:Save file by name
		paramS:
			[in] NVRFileName :file on NVR
			[in] destFileName :file saved on local
		return:
		    errcode 0:success else:failed
	*/
	int SaveFileByName( char* NVRFileName, char* destFileName );

	/*
		Description:Playback recoder by name
		paramS:
			[in] NVRFileName :file on NVR
			[in] callback: callback function realize by user
		return:
		    errcode 0:success else:failed
	*/
	int PlayBackByName(char* NVRFileName,PlaybackCallBack callback);

	/*
		Description:Playback recoder from nvr by time
		paramS:
			[in] startTime: 
			[in] stopTime
			[in] chan: channel number
			[in] callback 取流回调函数，由调用者实现此函数，可以对数据做编码、保存等操作
 		return:
		    errcode 0:success else:failed
	*/
	int PlayBackByTime(NET_DVR_TIME startTime,NET_DVR_TIME stopTime,LONG chanl,PlaybackCallBack callback);

	/*
		Description: Release Source
		paramS:
		return:
			errcode 0:success else:failed
	*/
	int Destory();

	/*
		Description search file   from nvr by time
		paramS:
			[in] startTime: 
			[in] stopTime
			[in] chan: channel number
			[out] file founded 
 		return:
		    errcode 0:success else:failed
	*/
	int FindFile(NET_DVR_TIME startTime,NET_DVR_TIME stopTime,LONG chanl, NET_DVR_FIND_DATA&  struFileData);

    /*
        Description get config from HIK DVR
        paramS:
            [in]command
            [in]group: channel group num,start at 0, each group contain 64 channel
            [out] device config
        return:
            errcode 0:success else:failed
    */

    int GetDVRConfig(DWORD command, LONG group, NET_DVR_IPPARACFG_V40 &deviceCfg);

    /*
        Description get Port from HIK DVR
        paramS:
            [in]command
            [out] device config
        return:
            errcode 0:success else:failed
    */
    int GetPort(DWORD command, NET_DVR_NAT_CFG &deviceCfg );
private:
	LONG m_UserID;

};
