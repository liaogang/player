#pragma once

#include "Thread.h"
#include "CriticalSection.h"
class CBasicPlayer;
class CPlayerThread : public CThread
{
public:
	CCriticalSection m_cs;
	BOOL  m_bKeepPlaying;
	BOOL  m_bSleep;
	DWORD m_dwSizeRead;
	DWORD m_dwSizeToWrite;
	BOOL m_bNewTrack;
	CBasicPlayer *m_pPlayer;
	LPDIRECTSOUNDBUFFER m_lpDSBuffer;
	LPDIRECTSOUND m_lpDsound;
	DWORD m_dwTime;

	/*retain 2 times len of */
	/*buffer for fft sample parse*/
	BYTE * pBufFFT1,* pBufFFT2;
	DWORD m_dwSizeRead2;
	BOOL bFileEnd2;
	int   playPosInFFt;                        //play pos in bufFft
	 //first,第一次不要把bufttf2拷贝到buffttf1
	BOOL bFirst;                   

public:
	CPlayerThread(CBasicPlayer *pPlayer);
	~CPlayerThread();

	void Excute(); 
	void Reset();

	DWORD DSoundBufferWrite(void* pBuf , int len);
	BOOL CleanDSBuffer();
	void WriteDataToDSBuf();
	DWORD m_dwCurWritePos;
	DWORD m_dwSilencedBytes;
};