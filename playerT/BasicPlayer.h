#pragma once

class CPlayerThread;
class CSpectrumAnalyser;
class MusicFile;
class CMainFrame;
class CPlayerController;
class CBasicPlayer
{
public:
	CPlayerController *m_pPlayerController;
	BOOL m_bFileEnd;
	CMainFrame *m_pMainFrame;
	CPlayerThread* m_pPlayerThread;
	CSpectrumAnalyser* m_pSpectrumAnalyser;
	MusicFile *m_pFile;
public:
	volatile BOOL m_bStopped;
	BOOL m_bPaused;
public:
	CBasicPlayer(void);
public:
	static CBasicPlayer* shared();

	~CBasicPlayer(void);
	void play();
	void pause();
	void stop();
	BOOL open( LPTSTR filepath );
};
