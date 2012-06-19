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
	INT m_curVolume;
	BOOL m_bStopped;
	BOOL m_bPaused;
	BOOL m_bFileEnd;

	MusicFile *m_pFile;
	CCriticalSection m_cs;
	CMainFrame *m_pMainFrame;
	CPlayerThread* m_pPlayerThread;
	CSpectrumAnalyser* m_pSpectrumAnalyser;


	void SetVolume(double vol);
	BOOL open( PlayListItem *track);
	void ResetFile();
	void SetPos(int cur,int max);

protected:
	void play();
	void pause();
	void stop();
	BOOL stoped(){return m_bStopped;}
	BOOL open( LPCTSTR filepath );
};