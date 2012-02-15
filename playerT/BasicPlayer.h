#pragma once

class CPlayerThread;
class CSpectrumAnalyser;
class MusicFile;

class CBasicPlayer
{
public:
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
