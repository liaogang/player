#include "stdafx.h"
#include "config.h"
#include "PlayerThread.h"
#include "Utils.h"
#include "BasicPlayer.h"
#include "MusicFile.h"
#include "mainfrm.h"

CPlayerThread::CPlayerThread(CBasicPlayer *pPlayer):CThread(TRUE),
m_lpDSBuffer(NULL),m_lpDsound(NULL),m_dwCurWritePos(-1)
{
	m_pPlayer=pPlayer;
	m_lpDsound=DSoundDeviceCreate();
	m_cs=new CCriticalSection;
}

CPlayerThread::~CPlayerThread(void)
{
	if(m_cs)
		delete m_cs;
}

void CPlayerThread::reset()
{
	static DWORD lastAvgBytesPerSec;
	WAVEFORMATEX *pwfx;
	pwfx=m_pPlayer->m_pFile->GetFormat();
	if (pwfx->nAvgBytesPerSec!=lastAvgBytesPerSec)
	{
		m_lpDSBuffer=DSoundBufferCreate(m_lpDsound,pwfx);
		lastAvgBytesPerSec=pwfx->nAvgBytesPerSec;
	}
	
	m_bNewTrack=TRUE;
	m_dwCurWritePos=0;
}

void CPlayerThread::CleanDSBufferByTrackBegin()
{
	HRESULT result;
	LPVOID buffer1;
	DWORD buffer1Len;

	result= m_lpDSBuffer->Lock(0,0,&buffer1,&buffer1Len,NULL,NULL,DSBLOCK_ENTIREBUFFER);
	
	if (result==DS_OK)
	{
		int emptyByte=(m_pPlayer->m_pFile->GetFormat()->wBitsPerSample == 8)?128:0;
		memset(buffer1, emptyByte, buffer1Len);
		m_lpDSBuffer->Unlock(buffer1,buffer1Len,NULL,NULL);
	}
}

void CPlayerThread::Excute()
{
	while(!m_pPlayer->m_bStopped)
	{
		WriteDataToDSBuf();
	}
}

void CPlayerThread::WriteDataToDSBuf()
{
	CONST DWORD buffersize = DEFAULTBUFFERSIZE;
	LPVOID pFileBuffer;
	DWORD dwSizeToRead,dwSizeRead;
	dwSizeToRead=buffersize;

	pFileBuffer=new BYTE[dwSizeToRead];

	m_pPlayer->m_pFile->Read(pFileBuffer,dwSizeToRead,&dwSizeRead);
	if (dwSizeRead<=0)
	{
		m_lpDSBuffer->Stop();
		m_pPlayer->m_bFileEnd=TRUE;
		m_pPlayer->m_bStopped=TRUE;
	}

	DOUBLE used,lefted;
	m_pPlayer->m_pFile->GetPos(&used,&lefted);
	::SendMessage(m_pPlayer->m_pMainFrame->m_hWnd,WM_USER+22,used,lefted);

	DWORD sizeToWrite,written;
	sizeToWrite=dwSizeRead;
	while(sizeToWrite > 0)
	{
		written= DSoundBufferWrite(pFileBuffer, sizeToWrite);
		if (written==-1)
		{
			m_pPlayer->m_bStopped=TRUE;
			break;
		}
		sizeToWrite-=written;
		pFileBuffer=(char*)pFileBuffer+written;
	}
}


DWORD CPlayerThread::DSoundBufferWrite(void* pBuf , int len)
{
	HRESULT result;
	DWORD playCursor;

	result= m_lpDSBuffer->GetCurrentPosition(&playCursor,NULL);
	if (FAILED(result)) return -1;

	DWORD available=DS_GetAvailable(g_dwMaxDSBufferLen,playCursor,m_dwCurWritePos);
	if (available>g_dwMaxDSBufferLen/2)
		Sleep(g_dwSleepTime);

	if (len>available)
		len=available;

	LPVOID buffer1,buffer2;
	DWORD buffer1Len,buffer2Len;
	result=m_lpDSBuffer->Lock(m_dwCurWritePos,len ,&buffer1,&buffer1Len,&buffer2,&buffer2Len,NULL );
	if (result==DS_OK)
	{
		memcpy(buffer1,pBuf,buffer1Len);
		if (buffer2!=NULL)
			memcpy(buffer2,(char*)pBuf+buffer1Len,buffer2Len);
	}
	else if (result==DSERR_BUFFERLOST)
	{
		return -1;
	}
	else 
	{ 
		return -1;
	}

	m_lpDSBuffer->Unlock(buffer1,buffer1Len,buffer2,buffer2Len);

	m_dwCurWritePos+=buffer1Len+buffer2Len;
	if (m_dwCurWritePos>=g_dwMaxDSBufferLen)
		m_dwCurWritePos-=g_dwMaxDSBufferLen;

	return buffer1Len+buffer2Len;
}