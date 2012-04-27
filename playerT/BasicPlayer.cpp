#include "stdafx.h"
#include "BasicPlayer.h"
#include "MusicFile.h"
#include "PlayerThread.h"
#include "SpectrumAnalyser.h"
#include "Mp3File.h"
#include "WaveFileEx.h"
#include "mainfrm.h"
#include "PlayList.h"



 //-----------------------------------------

 CPlayerController::CPlayerController(CPlayerThread *_playerThread)//:CThread(FALSE)
 {
	 m_pPlayerThread=_playerThread;
	 decayEvent=CreateEvent(NULL,FALSE,FALSE,NULL);
 }

 void CPlayerController::Excute()
 {
	 while(1)
	 {
		 ::WaitForSingleObject(decayEvent,INFINITE);
		 const static int indecayLen=13;
		 static double indecay[indecayLen]={ -150 , -170,  -200,  -500,  -800, -1000, -2000, -3000, -4000, -5000, -6000, -8000, -9000};

		 if (!m_pPlayerThread->m_pPlayer->bDecay)    
		 {

			 for (int i=0;i<indecayLen;i++)
			 {
				 m_pPlayerThread->m_lpDSBuffer->SetVolume(indecay[i]);
				 Sleep(70);
			 }

			 m_pPlayerThread->m_lpDSBuffer->Stop();
			 m_pPlayerThread->Suspend();
		 }
		 else
		 {
			 m_pPlayerThread->Resume();
			 m_pPlayerThread->m_lpDSBuffer->Play(0,0,DSBPLAY_LOOPING);


			 for (int i=indecayLen-1;i>=0;--i)
			 {
				 m_pPlayerThread->m_lpDSBuffer->SetVolume(indecay[i]);
				 Sleep(70);
			 }
			 
		 }
	 }
 }


 //-----------------------------------------
 //
 CBasicPlayer* CBasicPlayer::shared()
 {
	 static CBasicPlayer *pGlobalBasePlayer=NULL;
	 if(!pGlobalBasePlayer)
		 pGlobalBasePlayer=new CBasicPlayer;
	 return pGlobalBasePlayer;
 }


 CBasicPlayer :: CBasicPlayer(void):
 m_bStopped(TRUE),m_bPaused(TRUE),
	 m_pFile(NULL),m_bFileEnd(FALSE)
{
	m_hWStartEvent=::CreateEvent(NULL,TRUE,FALSE,NULL);	

	m_pPlayerThread=new CPlayerThread(this);
	m_pSpectrumAnalyser=new CSpectrumAnalyser;

	ctl=new CPlayerController(m_pPlayerThread);
	ctl->Resume();
}

CBasicPlayer :: ~CBasicPlayer(void)
{
	if(!m_pPlayerThread) delete m_pPlayerThread;
	if (!m_pSpectrumAnalyser) delete m_pSpectrumAnalyser; 
}

const TCHAR* CBasicPlayer::playNextPlaylistItem()
{
	PlayListItem *item=MyLib::GetPlayListObj().GetNextTrackByOrder();
	stop();
	if(item  && open((LPTSTR) item->url.c_str()) )
	{
		play();
		return item->title.c_str();
	}
	return NULL;
}

BOOL CBasicPlayer::open( LPTSTR filepath )
{
	if(!m_bStopped)return FALSE;

	int len=_tcslen(filepath);
	TCHAR* p=filepath+len;
	while (p--)
		if ((TCHAR)(*p)=='.')
		{
			p++;
			break;
		}
	
	if(m_pFile)
		delete m_pFile;
	m_bFileEnd=FALSE;
	
	if (_tcscmp(p,_T("wav"))==0)
		m_pFile=new WaveFile();
	if (_tcscmp(p,_T("mp3"))==0)
		m_pFile=new Mp3File();
	if (_tcscmp(p,_T("wma"))==0)
		m_pFile=new Mp3File();

	return m_pFile->OpenAndReadID3Info(filepath);
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
	m_pPlayerThread->WriteDataToDSBuf();
	
	m_pPlayerThread->m_lpDSBuffer->SetVolume(0);
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
		m_bPaused=FALSE;
		bDecay=FALSE;
		::SetEvent(ctl->decayEvent);
	}
	else                        //pause       
	{
		m_bPaused=TRUE;
		bDecay=TRUE;
		::SetEvent(ctl->decayEvent);
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
		m_cs.Enter();
		m_bStopped=TRUE;
		ResetEvent(m_hWStartEvent);
		m_pPlayerThread->m_lpDSBuffer->Stop();
		m_cs.Leave();
		
		if (m_bPaused)
		{
			m_pPlayerThread->Resume();
			m_bPaused=FALSE;
		}
	}
}