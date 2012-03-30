#pragma once

class CPlayerThread;
class CSpectrumAnalyser;
class MusicFile;
class CMainFrame;
class CBasicPlayer
{
public:
	CMainFrame *m_pMainFrame;
	CPlayerThread* m_pPlayerThread;
	CSpectrumAnalyser* m_pSpectrumAnalyser;
	MusicFile *m_pFile;
public:
	BOOL m_bStopped,m_bPaused;
public:
	CBasicPlayer(void);
public:
	static CBasicPlayer* shared();

	~CBasicPlayer(void);
	void start();
	void pause();
	void stop();
	void open( LPTSTR filepath );
};
