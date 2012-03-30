#include "stdafx.h"
#include "config.h"
#include "PlayerThread.h"
#include "Utils.h"
#include "BasicPlayer.h"
#include "MusicFile.h"
#include "mainfrm.h"

CPlayerThread::CPlayerThread(CBasicPlayer *pPlayer):CThread(TRUE)
{
	m_pPlayer=pPlayer;
}

CPlayerThread::~CPlayerThread(void)
{

}

void CPlayerThread::Excute()
{
	MusicFile *pFile;
	WAVEFORMATEX *pwfx;
	if(! m_pPlayer ) return;

	pFile=m_pPlayer->m_pFile;
	pwfx=pFile->GetFormat();

	const DWORD buffersize = DEFAULTBUFFERSIZE;

	LPDIRECTSOUND lpDsound=DSoundDeviceCreate();
	LPDIRECTSOUNDBUFFER lpDSBuffer=DSoundBufferCreate(lpDsound,pwfx);

	lpDSBuffer->Play( 0, 0, DSBPLAY_LOOPING);
	while(!m_pPlayer->m_bStopped)
	{
		void* pFileBuffer;
		DWORD dwSizeToRead,dwSizeRead;
		dwSizeToRead=buffersize;
		
		pFileBuffer=new BYTE[dwSizeToRead];
		pFile->Read(pFileBuffer,dwSizeToRead,&dwSizeRead);
		if (dwSizeRead<=0)
		{
			m_pPlayer->m_bStopped=TRUE;
		}
		else
		{
			double used,lefted;
			m_pPlayer->m_pFile->GetPos(&used,&lefted);
			::SendMessage(m_pPlayer->m_pMainFrame->m_hWnd,WM_USER+22,used,lefted);
		}

		DWORD sizeToWrite;
		sizeToWrite=dwSizeRead;
		while(sizeToWrite > 0)
		{
			DWORD written= DSoundBufferWrite(lpDSBuffer, pFileBuffer, sizeToWrite);
			if (written==-1)
			{
				m_pPlayer->m_bStopped=TRUE;
				break;
			}
			sizeToWrite-=written;
			pFileBuffer=(char*)pFileBuffer+written;
		}	
	}
}