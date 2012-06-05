#pragma once

#include "Thread.h"
class CBasicPlayer;
class CPlayerThread : public CThread
{
public:
	BOOL  m_bSleep;
	DWORD m_dwSizeRead;
	DWORD m_dwSizeToWrite;
	BOOL m_bNewTrack;
	CBasicPlayer *m_pPlayer;
	LPDIRECTSOUNDBUFFER m_lpDSBuffer;
	LPDIRECTSOUND m_lpDsound;
	DWORD m_dwTime;
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