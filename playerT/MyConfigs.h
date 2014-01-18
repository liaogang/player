#include "MySerialize.h"
#include "customMsg.h"

class MyConfigs:public SerializeObj<MyConfigs>
{
public:
	MyConfigs():bResumeOnReboot(1),playersVolume(100),playorder(0)
		,playlistIndex(-1),trackIndex(-1),playingStatus(status_stopped)
	{

	}
public:
	FILE& operator<<(FILE& f);
	FILE& operator>>(FILE& f);
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
	trackPosInfo pos;    //音轨的播放进度


	//播放器的音量
	int playersVolume;

	//播放次序
	int playorder;
};

MyConfigs* GetMyConfigs();