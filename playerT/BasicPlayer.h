#include "Thread.h"
#include "CriticalSection.h"

#pragma once

class CPlayerThread;
class CSpectrumAnalyser;
class MusicFile;
class CMainFrame;
class CPlayerController;
class PlayListItem;


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
	int indexPoint;
	int indexVec;
	double *volBuffer;
	BOOL     m_bSlowDown;           //down or up
	BOOL     m_bCloseFileInSlowDown;
	int      maxTimerCount;
	MMRESULT m_timerID;

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


	void InitSlowDown(BOOL bSlowDown=TRUE,BOOL bCloseFile=FALSE);
	void SlowDownVol();
	void SetVolumeByEar(int vol);
	BOOL open( PlayListItem *track);
	void ResetFile();
	void SetPos(int cur,int max);
	void WaitPlay();
	void OpenAfterSlowDown();
	inline BOOL stoped(){return m_bStopped;}
protected:
	void play();
	void pause();//pause or resume play
	void stop();
	BOOL open( LPCTSTR filepath );

private:
	void InitSlowDownVolBuffer();
};