#pragma once
#include "stdafx.h"
#include "Thread.h"
#include "CriticalSection.h"

struct trackPosInfo;
class CBasicPlayer;
class MusicFile;

class CPlayerThread : public CThread
{
public:
	CCriticalSection *m_cs;
	LPDIRECTSOUNDBUFFER m_lpDSBuffer;

	BOOL  m_bKeepPlaying;
	void  SetFlagToExit(){m_bKeepPlaying=FALSE;}

	DWORD m_dwSizeRead;
	DWORD m_dwSizeToWrite;

	BOOL *m_pStopped;

	MusicFile *m_pFile;
	WORD m_wBitsPerSample;
	DWORD m_nAvgBytesPerSec;
	//BOOL m_bFileEnded;
	int m_iBytePerFrame;
	int m_iTotalFrames;


	DWORD m_dwTime;
	/*retain 2 times len of */
	/*buffer for fft sample parse*/
	signed char * pBufFFT1;//for ds sound

	signed char *pBufFft; //for anayser
	int   fftBufLen;
	int   playPosInFFt; //play pos in bufFft

	BYTE *m_ReduceBuffer;
public:
	CPlayerThread::CPlayerThread(void * pFile,CCriticalSection *cs,BOOL *pStop);

	CPlayerThread::CPlayerThread(CCriticalSection *cs,BOOL *pStop);

	~CPlayerThread();

	void Excute(); 
	void Reset();

	DWORD DSoundBufferWrite(void* pBuf , int len);

	BOOL CleanDSBuffer();
	
	void WriteDataToDSBuf();


	//get time offset not played yet after file.
	double GetOffsetSeconds();

	//return TRUE if file reached end .
	BOOL ReadFileReduceVol(BOOL bReduce=TRUE);
	
	
	BOOL BeginChangeTrackPos();
	BOOL EndChangeTrackPos();


	DWORD m_dwCurWritePos;
};