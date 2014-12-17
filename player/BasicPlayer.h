#pragma once
#include "Thread.h"
#include "CriticalSection.h"
#include "globalStuffs.h"



class CPlayerThread;
class CSpectrumAnalyser;
class MusicFile;
class CMainFrame;
class CPlayerController;
class CPlayListItem;
struct trackPosInfo;

class CBasicPlayer
{
	friend class MyLib;
public:
	CBasicPlayer(void);
	~CBasicPlayer(void);
	static  CBasicPlayer* const shared();

public:
	HANDLE m_eventSlowDown;


	int indexA,indexB;
	volatile int indexPoint;
	int indexVec;
	double *volBuffer;
	BOOL     m_bSlowDown;           //down or up
	BOOL     m_bCloseFileInSlowDown;
	MMRESULT m_timerID;


	MMRESULT m_timerDownID;
	MMRESULT	m_timerUpID;
	volatile int curVolDown,curVolUp;

	INT m_curVolume;
	double m_curDSBVolume;
	BOOL m_bStopped;
	BOOL m_bPaused;
	//BOOL m_bFileEnd;
	//BOOL m_bStartPlay;
	MusicFile *m_pFile;
	CCriticalSection m_cs;
	CPlayerThread* m_pPlayerThread;
	//CSpectrumAnalyser* m_pSpectrumAnalyser;

	PlayingStatus m_lastStatus;
	int m_msecLastPos;
	double m_maxFilepos;
	int m_MillisecondsTotal;
	//void InitSlowDown(BOOL bSlowDown=TRUE,BOOL bCloseFile=FALSE);
	void SlowDownVol();
	void GrowUpVol();
	void TimerVolSlowDown();

	void TimerVolGrowUp();

	void SetVolumeByEar(int vol);
	
	void ResetFile();
	void SetPos(int cur);
	void SetFilePos(double cur);
	int  MillisecondsPlayed();
	int  MillisecondsTotal();
	BOOL stoped(){return m_bStopped;}
	BOOL paused(){return m_bPaused;}

	//save status and stop 
	void Destroy();
	//PlayingStatus GetLastPlayingStatus(){return m_lastStatus;}
	void stop();
protected:
	void play();
	void pause();//pause or resume play
	
	BOOL open( LPCTSTR filepath );

	
private:
	void InitSlowDownVolBuffer();
};