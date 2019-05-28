#include "src/upload.h"
#include "src/openApiUtil.h"

// define a global authinfo
//VodCredential AuthInfo = {"","","",""};
//memset(&AuthInfo,0,sizeof(VodCredential));

//void testCallback

struct Access{
	std::string AcessKey;
	std::string AcessKeySecret;
	std::string RegionId ;
	std::string SecurityToken;
};

struct File{
	std::string FileName;				//ԭ��Ƶ�ļ���(����)
	std::string Title;					//�ϴ������Ƶ����(����)
	std::string CoverUrl;				//������Ƭ
	std::string Description;			//��Ƶ����
	std::string Tags;					//��Ƶ��ǩ

};

int InitVodClient(Access access); //��ʼ��
int UploadLocalVideo(Access access,File file,std::string & url); // �ϴ���Ƶ
int GetPlayURL(Access access,std::string VideoId,std::string & PlayURL );	// ��ȡ���ŵ�ַ
//int UploadLocalVideo(File file,std::string & url); // �ϴ���Ƶ