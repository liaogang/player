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
	int      volA;
	int      volB;
	int      volDownSpec;
	BOOL     m_bSlowDown,m_bCloseFileInSlowDown;
	int      timerCount;
	int      maxTimerCount;
	MMRESULT m_timerID;

	INT m_curVolume;
	BOOL m_bStopped;
	BOOL m_bPaused;
	BOOL m_bFileEnd;

	MusicFile *m_pFile;
	CCriticalSection m_cs;
	CMainFrame *m_pMainFrame;
	CPlayerThread* m_pPlayerThread;
	CSpectrumAnalyser* m_pSpectrumAnalyser;


	void InitSlowDown(BOOL bSlowDown=TRUE,BOOL bCloseFile=FALSE);
	void SlowDownVol();
	void SetVolume(double vol);
	BOOL open( PlayListItem *track);
	void ResetFile();
	void SetPos(int cur,int max);

protected:
	void play();
	void pause();//pause or resume play
	void stop();
	BOOL stoped(){return m_bStopped;}
	BOOL open( LPCTSTR filepath );
};