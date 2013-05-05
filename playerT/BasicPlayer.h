#include "Thread.h"
#include "CriticalSection.h"

#pragma once

class CPlayerThread;
class CSpectrumAnalyser;
class MusicFile;
class CMainFrame;
class CPlayerController;
class FileTrack;

class CBasicPlayer
{
	friend class MyLib;
public:
	CBasicPlayer(void);
	~CBasicPlayer(void);
	static CBasicPlayer* shared();

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
	BOOL m_bStopped;
	BOOL m_bPaused;
	BOOL m_bFileEnd;
	BOOL m_bStartPlay;
	MusicFile *m_pFile;
	CCriticalSection m_cs;
	CMainFrame *m_pMainFrame;
	CPlayerThread* m_pPlayerThread;
	CSpectrumAnalyser* m_pSpectrumAnalyser;

	FileTrack *itemWaitPlay;
	//void InitSlowDown(BOOL bSlowDown=TRUE,BOOL bCloseFile=FALSE);
	void SlowDownVol();
	void GrowUpVol();
	void TimerVolSlowDown();

	void TimerVolGrowUp();

	void SetVolumeByEar(int vol);
	BOOL open(FileTrack * track);
	void ResetFile();
	void SetPos(int cur,int max);
	void WaitPlay();
	void OpenAfterSlowDown(FileTrack * item);
	inline BOOL stoped(){return m_bStopped;}
protected:
	void play();
	void pause();//pause or resume play
	void stop();
	BOOL open( LPCTSTR filepath );

private:
	void InitSlowDownVolBuffer();
};