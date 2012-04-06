#include "stdafx.h"
#include "BasicPlayer.h"
#include "MusicFile.h"
#include "PlayerThread.h"
#include "SpectrumAnalyser.h"
#include "Mp3File.h"
#include "WaveFileEx.h"

 CBasicPlayer* CBasicPlayer::shared()
 {
 	static CBasicPlayer *pGlobalBasePlayer=NULL;
 	if(!pGlobalBasePlayer)
 		pGlobalBasePlayer=new CBasicPlayer;
 	return pGlobalBasePlayer;
 }


 CBasicPlayer :: CBasicPlayer(void):
 m_bStopped(TRUE),m_bPaused(TRUE),m_pFile(NULL),m_bFileEnd(FALSE)
{
	m_pPlayerThread=new CPlayerThread(this);
	m_pSpectrumAnalyser=new CSpectrumAnalyser;
	//m_pPlayerController=new CPlayerController(m_pPlayerThread);
}

CBasicPlayer :: ~CBasicPlayer(void)
{
	if(!m_pPlayerThread) delete m_pPlayerThread;
	if (!m_pSpectrumAnalyser) delete m_pSpectrumAnalyser; 
	//if (!m_pPlayerController) delete m_pPlayerController;
}

BOOL CBasicPlayer::open( LPTSTR filepath )
{
	int len=_tcslen(filepath);
	TCHAR* p=filepath+len;
	while (p--)
	{
		if ((TCHAR)(*p)=='.')
		{
			p++;
			break;
		}
	}

	if(m_pFile)
		delete m_pFile;
	m_bFileEnd=FALSE;

	if (_tcscmp(p,_T("wav"))==0)
	{
		m_pFile=new WaveFile();
	}
	if (_tcscmp(p,_T("mp3"))==0)
	{
		m_pFile=new Mp3File();
	}
	if (_tcscmp(p,_T("wma"))==0)
	{
		m_pFile=new Mp3File();
	}

	INT result=m_pFile->OpenAndReadID3Info(filepath);
	if (result==FALSE)
	{
		return FALSE;
	}

	mpg123_id3v1 *id3v1=m_pFile->m_pMpg123_id3v1;
}


void CBasicPlayer::play()
{
	if (m_bFileEnd)
		return;
	
	if (!m_bStopped)
		if (m_bPaused)      //Î´Í£Ö¹ÇÒÔÝÍ£
		{
			pause();        //¼ÌÐø
			return;
		}
		else               //ÕýÔÚ²¥·Å
			stop();        //ÏÈÍ£Ö¹

	///set file to Init
// 	static BOOL bFirst=TRUE;
// 	if(!bFirst)
// 	::WaitForSingleObject(m_pPlayerController->m_hStopEvent,INFINITE);
// 	bFirst=FALSE;




	m_pFile->ResetFile();
	m_pPlayerThread->reset();
	m_pPlayerThread->CleanDSBuffer();

	
	//::SetEvent(m_pPlayerController->m_hStartEvent);
	m_pPlayerThread->WriteDataToDSBuf();
	m_pPlayerThread->m_lpDSBuffer->Play( 0, 0, DSBPLAY_LOOPING);
	m_bStopped=FALSE;
	m_bPaused=FALSE;
	m_pPlayerThread->Resume();
}

void CBasicPlayer::pause()
{
	if (m_bStopped)
		return;

	if (m_bPaused)    //resume
	{
		m_pPlayerThread->m_lpDSBuffer->Play( 0, 0, DSBPLAY_LOOPING);
		m_pPlayerThread->Resume();
		m_bPaused=FALSE;
	}
	else               //pause
	{
		m_pPlayerThread->m_lpDSBuffer->Stop();
		m_pPlayerThread->Suspend();
		m_bPaused=TRUE;
	}
}

void CBasicPlayer::stop()
{
	if(!m_bStopped)
	{
		m_pPlayerThread->m_lpDSBuffer->Stop();


		m_bStopped=TRUE;

		m_bPaused=TRUE;
		
		//m_pPlayerThread->m_cs->Enter();
		m_pPlayerThread->Suspend();
		//m_pPlayerThread->m_cs->Leave();

		m_pPlayerThread->Renew();
	}
}