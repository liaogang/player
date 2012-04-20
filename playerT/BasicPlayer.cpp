#include "stdafx.h"
#include "BasicPlayer.h"
#include "MusicFile.h"
#include "PlayerThread.h"
#include "SpectrumAnalyser.h"
#include "Mp3File.h"
#include "WaveFileEx.h"
#include "mainfrm.h"

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
	m_hWStartEvent=CreateEvent(NULL,TRUE,FALSE,NULL);
	m_pPlayerThread=new CPlayerThread(this);
	m_pPlayerThread->Resume();
#ifdef _DEBUG
	m_pPlayerThread->m_pStrDebug=debug_str;
	m_pPlayerThread->m_pDebugStrPt=&debug_pt;
#endif
	m_pSpectrumAnalyser=new CSpectrumAnalyser;
}


CBasicPlayer :: ~CBasicPlayer(void)
{
	if(!m_pPlayerThread) delete m_pPlayerThread;
	if (!m_pSpectrumAnalyser) delete m_pSpectrumAnalyser; 
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
	
	return TRUE;
}


void CBasicPlayer::play()
{
	if (m_bFileEnd)	return;
	
	if (!m_bStopped)
		if (m_bPaused)      //Î´Í£Ö¹ÇÒÔÝÍ£
		{
			pause();        //¼ÌÐø
			return;
		}
		else               //ÕýÔÚ²¥·Å
			stop();        //ÏÈÍ£Ö¹


	m_pFile->ResetFile();
	m_pPlayerThread->reset();
	m_pPlayerThread->CleanDSBuffer();
	m_pPlayerThread->WriteDataToDSBuf();
	m_pPlayerThread->m_lpDSBuffer->Play( 0, 0, DSBPLAY_LOOPING);
	
	::SetEvent(m_hWStartEvent);

	m_bStopped=FALSE;
	m_bPaused=FALSE;
}

void CBasicPlayer::pause()
{
	if (m_bStopped)return;

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

BOOL CBasicPlayer::stoped()
{
	return m_bStopped;
}
void CBasicPlayer::stop()
{
	if(!m_bStopped)
	{
		m_pPlayerThread->m_cs->Enter();
		::ResetEvent(m_hWStartEvent);
		m_pPlayerThread->m_lpDSBuffer->Stop();
		m_pPlayerThread->m_cs->Leave();

		m_bStopped=TRUE;
		m_bPaused=TRUE;

#ifdef _DEBUG
		TRACE_11 ( _T("stoped  ") );
#endif
		
		::PostMessage(this->m_pMainFrame->m_hWnd,WM_USER+22,0,100);
	}
}