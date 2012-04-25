#pragma once
#include "CriticalSection.h"
class CPlayerThread;
class CSpectrumAnalyser;
class MusicFile;
class CMainFrame;
class CPlayerController;
#include "Thread.h"
class CPlayerController :public CThread
{
public:
	HANDLE decayEvent;
	CPlayerThread* m_pPlayerThread;

	CPlayerController(CPlayerThread *_playerThread);


	~CPlayerController(void)
	{
	}

	void Excute();
};
class CBasicPlayer
{
public:
	BOOL bDecay;CPlayerController *ctl;

	BOOL m_bFileEnd;
	CMainFrame *m_pMainFrame;
	CPlayerThread* m_pPlayerThread;
	CSpectrumAnalyser* m_pSpectrumAnalyser;
	MusicFile *m_pFile;
public:
	volatile BOOL m_bStopped;
	BOOL m_bPaused;
public:
	HANDLE m_hWStartEvent,m_hPauseEvent,m_hPausedEvent;
	CBasicPlayer(void);
public:
	static CBasicPlayer* shared();
	~CBasicPlayer(void);
	void play();
	void pause();
	void stop();
	BOOL stoped();
	BOOL open( LPTSTR filepath );
	const TCHAR* playNextPlaylistItem();
private:
	void decayPlay(BOOL bDecay=TRUE);
};