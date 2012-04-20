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
public:
	CBasicPlayer(void);
public:
	static CBasicPlayer* shared();

	~CBasicPlayer(void);
	void play();
	void pause();
	void stop();
	BOOL stoped();
	BOOL open( LPTSTR filepath );
};





#ifdef _DEBUG

static TCHAR *debug_str=new TCHAR[1024*100];
static int  debug_pt=0;
//static CCriticalSection debug_s;
#define TRACE_11(tmp) \
	_tcscpy(debug_str+debug_pt,tmp);\
	debug_pt+=_tcslen(tmp);

#endif