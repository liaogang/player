#include "stdafx.h"
#include "config.h"
#include "PlayerThread.h"
#include "Utils.h"
#include "BasicPlayer.h"
#include "MusicFile.h"

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

		DWORD sizeToWrite;
		sizeToWrite=dwSizeRead;
		while(sizeToWrite > 0)
		{
			DWORD written= DSoundBufferWrite(lpDSBuffer, pFileBuffer, dwSizeRead);
			sizeToWrite-=written;
		}	
	}
}