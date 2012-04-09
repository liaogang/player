#pragma once

#include "Thread.h"
#include "CriticalSection.h"
class CBasicPlayer;
class CPlayerThread : public CThread
{
public:
	BOOL  m_bSleep;
	DWORD m_dwSizeRead;
	DWORD m_dwSizeToWrite;
	BOOL m_bNewTrack;
	CCriticalSection* m_cs;
	CBasicPlayer *m_pPlayer;
	LPDIRECTSOUNDBUFFER m_lpDSBuffer;
	LPDIRECTSOUND m_lpDsound;


#ifdef _DEBUG
	TCHAR *m_pStrDebug;
	INT   *m_pDebugStrPt;
#endif

public:
	CPlayerThread(CBasicPlayer *pPlayer);
	~CPlayerThread();

	void Excute(); 
	void reset();

	DWORD DSoundBufferWrite(void* pBuf , int len);
	void CleanDSBuffer();
	void WriteDataToDSBuf();
	DWORD m_dwCurWritePos;
	DWORD m_dwSilencedBytes;
};



class CPlayerController //:public CThread
{
public:
	HANDLE m_hStartEvent;
	HANDLE m_hStartedEvent;
	HANDLE m_hStopEvent;
	CPlayerThread* m_pPlayerThread;

	CPlayerController(CPlayerThread *_playerThread);


	~CPlayerController(void)
	{
		if(m_hStartEvent)
			CloseHandle(m_hStartEvent);
	}

	void Excute();
};