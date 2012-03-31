#pragma once

#include "Thread.h"
#include "CriticalSection.h"
class CBasicPlayer;
class CPlayerThread : public CThread
{
public:
	BOOL m_bNewTrack;
	CCriticalSection* m_cs;
	CBasicPlayer *m_pPlayer;
	LPDIRECTSOUNDBUFFER m_lpDSBuffer;
	LPDIRECTSOUND m_lpDsound;
public:
	CPlayerThread(CBasicPlayer *pPlayer);
	~CPlayerThread();

	void Excute(); 
	void reset();

	DWORD DSoundBufferWrite(void* pBuf , int len);
	void CleanDSBufferByTrackBegin();
	void WriteDataToDSBuf();
	DWORD m_dwCurWritePos;
	DWORD m_dwSilencedBytes;
};