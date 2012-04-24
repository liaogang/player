#pragma once
#include "CriticalSection.h"
class CPlayerThread;
class CSpectrumAnalyser;
class MusicFile;
class CMainFrame;
class CPlayerController;
class CBasicPlayer
{
public:
	BOOL m_bFileEnd;
	CMainFrame *m_pMainFrame;
	CPlayerThread* m_pPlayerThread;
	CSpectrumAnalyser* m_pSpectrumAnalyser;
	MusicFile *m_pFile;
public:
	volatile BOOL m_bStopped;
	BOOL m_bPaused;
public:
	HANDLE m_hWStartEvent;//work thread
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