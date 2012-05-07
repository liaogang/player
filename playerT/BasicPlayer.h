#include "Thread.h"
#include "CriticalSection.h"


#pragma once

//-----------------------------------------
//
class CPlayerThread;
class CSpectrumAnalyser;
class MusicFile;
class CMainFrame;
class CPlayerController;
class PlayListItem;


//-----------------------------------------
//
class CPlayerController :public CThread
{
public:
	HANDLE decayEvent;
	CPlayerThread* m_pPlayerThread;
public:
	CPlayerController(CPlayerThread *_playerThread);
	~CPlayerController(void){}
	void Excute();
};


//-----------------------------------------
//
class CBasicPlayer
{
	friend class MyLib;
public:
	CBasicPlayer(void);
	~CBasicPlayer(void);
	static CBasicPlayer* shared();


public:
	CCriticalSection m_cs;
	BOOL bDecay;
	
	BOOL m_bFileEnd;
	CMainFrame *m_pMainFrame;

	CPlayerController *ctl;
	CPlayerThread* m_pPlayerThread;
	CSpectrumAnalyser* m_pSpectrumAnalyser;
	MusicFile *m_pFile;
	
	

	volatile BOOL m_bStopped;
	BOOL m_bPaused;

	HANDLE m_hWStartEvent;

public:
	void SetVolume(double vol);
	
	BOOL open( PlayListItem *track);
	void ResetFile();
	void SetPos(int cur,int max);
	//const TCHAR* playNextPlaylistItem();
protected:
	void play();
	void pause();
	void stop();
	BOOL stoped();
	BOOL open( LPCTSTR filepath );
	//void decayPlay(BOOL bDecay=TRUE);
};