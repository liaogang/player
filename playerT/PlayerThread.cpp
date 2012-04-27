#include "stdafx.h"
#include "PlayerThread.h"
#include "Utils.h"
#include "BasicPlayer.h"
#include "MusicFile.h"
#include "mainfrm.h"


CPlayerThread::CPlayerThread(CBasicPlayer *pPlayer):CThread(FALSE),
m_lpDSBuffer(NULL),m_lpDsound(NULL),m_dwCurWritePos(-1)
{
	m_pPlayer=pPlayer;
	m_lpDsound=DSoundDeviceCreate();
}

CPlayerThread::~CPlayerThread(void)
{
}

void CPlayerThread::reset()
{
	static DWORD lastAvgBytesPerSec=0;
	WAVEFORMATEX *pwfx=0;
	pwfx=m_pPlayer->m_pFile->GetFormat();
	if (pwfx->nAvgBytesPerSec!=lastAvgBytesPerSec)
	{
		m_lpDSBuffer=DSoundBufferCreate(m_lpDsound,pwfx);
		lastAvgBytesPerSec=pwfx->nAvgBytesPerSec;
	}
	
	m_bNewTrack=TRUE;
	m_dwCurWritePos=0;
}

void CPlayerThread::CleanDSBuffer()
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

	m_lpDSBuffer->SetCurrentPosition(0);
}

void CPlayerThread::Excute()
{
	while(TRUE)
	{
		m_pPlayer->m_cs.Enter();
		::WaitForSingleObject(m_pPlayer->m_hWStartEvent,INFINITE);
		WriteDataToDSBuf();
		m_pPlayer->m_cs.Leave();
	}
}

void CPlayerThread::WriteDataToDSBuf()
{
	CONST DWORD buffersize = DEFAULTBUFFERSIZE;
	DWORD dwSizeToRead;
	dwSizeToRead=buffersize;
	BOOL bEndOfInput;

	char fileBuffer[DEFAULTBUFFERSIZE];
	char *pFileBuffer=fileBuffer;

	 bEndOfInput=!m_pPlayer->m_pFile->Read(pFileBuffer,dwSizeToRead,&m_dwSizeRead);
	if (bEndOfInput)
	{
		::PostMessage(m_pPlayer->m_pMainFrame->m_hWnd,WM_TRACKPOS,0,100);
		::PostMessage(m_pPlayer->m_pMainFrame->m_hWnd,WM_TRACKSTOPPED,0,0);
		m_lpDSBuffer->Stop();
		m_pPlayer->m_bFileEnd=TRUE;
		m_pPlayer->m_bStopped=TRUE;
		::ResetEvent(m_pPlayer->m_hWStartEvent);
		return; 
	}

	DOUBLE used,lefted;
	m_pPlayer->m_pFile->GetPos(&used,&lefted);
	::PostMessage(m_pPlayer->m_pMainFrame->m_hWnd,WM_TRACKPOS,used,lefted);
	

	DWORD playCursor;
	if (FAILED(m_lpDSBuffer->GetCurrentPosition(&playCursor,NULL))) return;
	DWORD available=DS_GetAvailable(g_dwMaxDSBufferLen,playCursor,m_dwCurWritePos);

	if (available<DEFAULTBUFFERSIZE*2)
		::Sleep(g_dwSleepTime);
	

	DWORD written;
	m_dwSizeToWrite=m_dwSizeRead;

	while(m_dwSizeToWrite > 0)
	{
		written= DSoundBufferWrite(pFileBuffer, m_dwSizeToWrite);
		if (written==-1)
		{
			m_pPlayer->m_bStopped=TRUE;
			break;
		}
		
		m_dwSizeToWrite-=written;
		pFileBuffer=pFileBuffer+written;
	}
}


DWORD CPlayerThread::DSoundBufferWrite(void* pBuf , int len)
{
	HRESULT result;
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
		return -1;
	else 
		return -1;


	m_lpDSBuffer->Unlock(buffer1,buffer1Len,buffer2,buffer2Len);

	m_dwCurWritePos+=buffer1Len+buffer2Len;
	if (m_dwCurWritePos>=g_dwMaxDSBufferLen)
		m_dwCurWritePos-=g_dwMaxDSBufferLen;
	return buffer1Len+buffer2Len;
}
