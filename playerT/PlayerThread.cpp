#include "stdafx.h"
#include "PlayerThread.h"
#include "Utils.h"
#include "BasicPlayer.h"
#include "MusicFile.h"
#include "mainfrm.h"



CPlayerThread::CPlayerThread(CBasicPlayer *pPlayer):CThread(TRUE),
m_lpDSBuffer(NULL),m_lpDsound(NULL),m_dwCurWritePos(-1),m_bKeepPlaying(TRUE),pPosInfo(NULL)
{
	m_pPlayer=pPlayer;
	m_lpDsound=DSoundDeviceCreate();

	pBufFFT1=new signed char[gDefaultBufferSize];
	memset(pBufFFT1,0,gDefaultBufferSize);

	pPosInfo=new trackPosInfo;
} 

CPlayerThread::~CPlayerThread(void)
{
	delete[] pBufFFT1;
	delete pPosInfo;
}

void CPlayerThread::Reset()
{
	static DWORD lastAvgBytesPerSec=0;
	WAVEFORMATEX *pwfx=0;
	pwfx=m_pPlayer->m_pFile->GetFormat();
	if (pwfx && pwfx->nAvgBytesPerSec!=lastAvgBytesPerSec){
		m_lpDSBuffer=DSoundBufferCreate(m_lpDsound,pwfx);
		lastAvgBytesPerSec=pwfx->nAvgBytesPerSec;


		//anayser
		//-----------------------------------------
		fftBufLen=g_dwMaxDSBufferLen;
		pBufFft=new signed char[fftBufLen];
		playPosInFFt=0;
	}
	
	m_bNewTrack=TRUE;
	m_dwCurWritePos=0;
}




BOOL CPlayerThread::BeginChangeTrackPos(DWORD &dwWrite,int writed)
{
	DWORD dwPlay;
	m_lpDSBuffer->GetCurrentPosition(&dwPlay,&dwWrite);

	int fileOffset=m_dwCurWritePos-dwWrite;
	if(fileOffset<0)
		fileOffset+=g_dwMaxDSBufferLen;

	WAVEFORMATEX* format=m_pPlayer->m_pFile->GetFormat();
	

	int bytePF= format->wBitsPerSample/8 * 1152 *format->nChannels ;
	int frameOffset=fileOffset /(double) bytePF;
	m_pPlayer->m_pFile->seek_frame(0-frameOffset ,SEEK_CUR);

	m_dwCurWritePos=dwWrite;


	
	int  fileBufferLen=4800 ;
	int times=16;
	int   totalBufferLen=fileBufferLen * times;
	BYTE *fileBuffer=(BYTE*)malloc(totalBufferLen);

	double vol=1;
	
	double step=1/(double)times;
	int   bufferOffset=0;
	while(times--)
	{
		vol-=step;
		if(vol<0) vol=0;
		m_pPlayer->m_pFile->SetOutVolume(vol);

		if(!m_pPlayer->m_pFile->Read(fileBuffer + bufferOffset,fileBufferLen,&m_dwSizeRead) ||
			m_dwSizeRead==0 )
		{
			return 0;
		}
		else
		{
			bufferOffset+=m_dwSizeRead;
		}
	}

	DSoundBufferWrite(fileBuffer,totalBufferLen);

	free(fileBuffer);

	return dwWrite;
}


BOOL CPlayerThread::EndChangeTrackPos(DWORD dwWrite,int writed)
{
	int  fileBufferLen=4800 ;
	int times=16;
	int   totalBufferLen=fileBufferLen * times;
	BYTE *fileBuffer=(BYTE*)malloc(totalBufferLen);

	double vol=0;

	double step=1/(double)times;
	int   bufferOffset=0;
	while(times--)
	{
		vol+=step;
		if(vol>1)vol=1;
		m_pPlayer->m_pFile->SetOutVolume(vol);

		if(!m_pPlayer->m_pFile->Read(fileBuffer + bufferOffset,fileBufferLen,&m_dwSizeRead) ||
			m_dwSizeRead==0 )
		{
			return 0;
		}
		else
		{
			bufferOffset+=m_dwSizeRead;
		}
	}

	DSoundBufferWrite(fileBuffer,totalBufferLen);

	free(fileBuffer);



	m_pPlayer->m_pFile->SetOutVolume(1);

	return TRUE;
}



BOOL CPlayerThread::CleanDSBuffer()
{
	BOOL bRet=TRUE;
	LPVOID buffer;
	DWORD   bufferLen;

	HRESULT result= m_lpDSBuffer->Lock(0,0,&buffer,&bufferLen,NULL,NULL,DSBLOCK_ENTIREBUFFER);
	if (result==DS_OK){
		WAVEFORMATEX* format=m_pPlayer->m_pFile->GetFormat();
		if (format){
			int emptyByte=(format->wBitsPerSample == 8)?128:0;
			memset(buffer, emptyByte, bufferLen);
			m_lpDSBuffer->Unlock(buffer,bufferLen,NULL,NULL);
		}else bRet=FALSE;
	}

	m_lpDSBuffer->SetCurrentPosition(0);

	return bRet;
}


void CPlayerThread::Excute()
{
	while(m_bKeepPlaying)
		WriteDataToDSBuf();
	m_bKeepPlaying=TRUE;
}

void CPlayerThread::WriteDataToDSBuf()
{
	DWORD playCursor;
	if (FAILED(m_lpDSBuffer->GetCurrentPosition(&playCursor,NULL))) return;
	DWORD available=DS_GetAvailable(g_dwMaxDSBufferLen,playCursor,m_dwCurWritePos);

	Sleep2WaitReadCursor(available);



	m_pPlayer->m_cs.Enter();


	char *pFileBuffer=(char*)pBufFFT1;
	if(!m_pPlayer->m_pFile->Read(pBufFFT1,gDefaultBufferSize,&m_dwSizeRead) ||
		m_dwSizeRead==0 )
	{
		m_pPlayer->m_cs.Leave();
		if(m_pPlayer->m_bStopped)
		{			
			pPosInfo->used=0;
			pPosInfo->left=100;
			NotifyMsg(WM_TRACKPOS,(WPARAM)pPosInfo,0);
		}
		else
		{		
			NotifyMsg(WM_TRACK_REACH_END);
		}

		m_lpDSBuffer->Stop();
		m_pPlayer->m_bFileEnd=TRUE;
		m_pPlayer->m_bStopped=TRUE;
		m_bKeepPlaying=FALSE;
		return;
	}
	

	/*
	//send data to spectrum analyser buffer
	if (fftBufLen-playPosInFFt<m_dwSizeRead)
	{
		memcpy(pBufFft+playPosInFFt,pBufFFT1,fftBufLen-playPosInFFt);
		memcpy(pBufFft,pBufFFT1+fftBufLen-playPosInFFt,m_dwSizeRead-(fftBufLen-playPosInFFt) );
		playPosInFFt+=m_dwSizeRead-fftBufLen;
	}
	else
	{
		memcpy(pBufFft+playPosInFFt,pBufFFT1,m_dwSizeRead);
		playPosInFFt+=m_dwSizeRead;
	}
	*/


	//send data to direct sound sys buffer
	DWORD written;
	m_dwSizeToWrite=m_dwSizeRead;
	while(m_dwSizeToWrite > 0)
	{
		written= DSoundBufferWrite(pFileBuffer, m_dwSizeToWrite);
		if (written==-1){
			m_pPlayer->m_bStopped=TRUE;
			break;
		}

		m_dwSizeToWrite-=written;
		pFileBuffer=pFileBuffer+written;
	}

	m_pPlayer->m_cs.Leave();
}

DWORD CPlayerThread::DSoundBufferWrite(void* pBuf , int len)
{
	HRESULT result;
	LPVOID buffer1   ,buffer2;
	DWORD  buffer1Len,buffer2Len;
	result=m_lpDSBuffer->Lock(m_dwCurWritePos,len ,&buffer1,&buffer1Len,&buffer2,&buffer2Len,NULL );
	if (result==DS_OK){
		memcpy(buffer1,pBuf,buffer1Len);
		if (buffer2!=NULL)
			memcpy(buffer2,(char*)pBuf+buffer1Len,buffer2Len);
	}
	else if (result==DSERR_BUFFERLOST)
		return -1;
	else 
		return -1;


	m_lpDSBuffer->Unlock(buffer1,buffer1Len,buffer2,buffer2Len);

	m_dwCurWritePos+=buffer1Len+buffer2Len;
	if (m_dwCurWritePos>=g_dwMaxDSBufferLen)
		m_dwCurWritePos-=g_dwMaxDSBufferLen;
	return buffer1Len+buffer2Len;
}
