#include "MySerialize.h"
#include "globalStuffs.h"


#include <vector>

class MyConfigs:public SerializeObj<MyConfigs>
{
public:
	MyConfigs():bResumeOnReboot(1),playersVolume(100),playorder(0)
		, playlistIndex(-1), trackIndex(-1), playingStatus(status_stopped), m_iListFontHeight(18)
	{

	}
public:
	FILE& operator<<(FILE& f);
	FILE& operator>>(FILE& f) const ;
public:
	
	inline int getResumeOnReboot(){return bResumeOnReboot;}
	inline int getPlayersVolume(){return playersVolume;}

	inline void setResumeOnReboot(int bResumeOnReboot){this->bResumeOnReboot=bResumeOnReboot;}
	inline void setPlayersVolume(int playersVolume){ this->playersVolume=playersVolume;}

public:
	//������ָ�����
	int bResumeOnReboot;
	//�����ָ������������Ϣ
	
	int playlistIndex;     //�ڼ��������б�
	int trackIndex;                //�б��еĵڼ�������
	
	PlayingStatus playingStatus;
	int msecPos;    //����Ĳ��Ž���


	//������������
	int playersVolume;

	//���Ŵ���
	int playorder;

	void setTimeForUpdateMediaLib(FILETIME &fileTime)
	{
		fileTimeUpdateMediaLib=fileTime;
	}

	FILETIME fileTimeForUpdateMediaLib()
	{
		return fileTimeUpdateMediaLib;
	}

private:
	FILETIME fileTimeUpdateMediaLib;

public:
	void setListFontHeight(int h){ m_iListFontHeight = h; }
	int getListFontHeight(){ return m_iListFontHeight; }

public:
	int  m_iListFontHeight;
};

MyConfigs* GetMyConfigs();