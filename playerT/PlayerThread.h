#pragma once
struct trackPosInfo;
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

	trackPosInfo *pPosInfo;
	/*retain 2 times len of */
	/*buffer for fft sample parse*/
	signed char * pBufFFT1;//for ds sound

	signed char *pBufFft; //for anayser
	int   fftBufLen;
	int   playPosInFFt; //play pos in bufFft

public:
	CPlayerThread(CBasicPlayer *pPlayer);
	~CPlayerThread();

	void Excute(); 
	void Reset();

	DWORD DSoundBufferWrite(void* pBuf , int len);
	BOOL CleanDSBuffer();
	void WriteDataToDSBuf();



	DWORD m_dwReadPos;
	BOOL BeginChangeTrackPos(DWORD &dwWrite,int writed);
	BOOL EndChangeTrackPos(DWORD dwWrite,int writed);


	DWORD m_dwCurWritePos;
	DWORD m_dwSilencedBytes;
};