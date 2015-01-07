#include "MySerialize.h"
#include "globalStuffs.h"


#include <vector>

class MyConfigs:public SerializeObj<MyConfigs>
{
public:
	MyConfigs():bResumeOnReboot(1),playersVolume(100),playorder(0)
		, playlistIndex(-1), trackIndex(-1), playingStatus(status_stopped), m_iListFontHeight(18) , m_iLyricsFontHeight(20)
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
	//重启后恢复播放
	int bResumeOnReboot;
	//用来恢复播放所需的信息
	
	int playlistIndex;     //第几个播放列表
	int trackIndex;                //列表中的第几个音轨
	
	PlayingStatus playingStatus;
	int msecPos;    //音轨的播放进度


	//播放器的音量
	int playersVolume;

	//播放次序
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
	void offsetAllFontHeight(int offset)
	{
		offsetListFontHeight(offset);
		offsetLyricsFontHeight(offset);
	}

	void enlargeAllFontHeight()
	{
		offsetAllFontHeight(1);
	}

	void reduceAllFontHeight()
	{
		offsetAllFontHeight(-1);
	}

	void offsetListFontHeight(int off){ m_iListFontHeight += off; }
	void setListFontHeight(int h){ m_iListFontHeight = h; }
	int getListFontHeight(){ return m_iListFontHeight; }

	void offsetLyricsFontHeight(int off){ m_iLyricsFontHeight += off; }
	void setLyricsFontHeight(int h){m_iLyricsFontHeight = h;}
	int getLyricsFontHeight(){return m_iLyricsFontHeight;}
public:
	int  m_iListFontHeight;
	int  m_iLyricsFontHeight;
};

MyConfigs* GetMyConfigs();
