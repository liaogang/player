#include "stdafx.h"
#include "BasicPlayer.h"
#include "MusicFile.h"
#include "PlayerThread.h"
#include "SpectrumAnalyser.h"
#include "Mp3File.h"
#include "WaveFileEx.h"
#include "mainfrm.h"
#include "PlayList.h"

 CBasicPlayer* CBasicPlayer::shared()
 {
	 static CBasicPlayer *pGlobalBasePlayer=NULL;
	 if(!pGlobalBasePlayer)
		 pGlobalBasePlayer=new CBasicPlayer;
	 return pGlobalBasePlayer;
 }

 CBasicPlayer :: CBasicPlayer(void):
 m_bStopped(TRUE),m_bPaused(TRUE),
	 m_pFile(NULL),m_bFileEnd(TRUE)
{
	m_pPlayerThread=new CPlayerThread(this);
	m_pSpectrumAnalyser=new CSpectrumAnalyser;
}

CBasicPlayer :: ~CBasicPlayer(void)
{
	if(!m_pPlayerThread) delete m_pPlayerThread;
	if (!m_pSpectrumAnalyser) delete m_pSpectrumAnalyser; 
}

void CBasicPlayer::ResetFile()
{
	m_pFile->ResetFile();
}

void CBasicPlayer:: SetVolume(double vol)
{
	if (m_pPlayerThread && m_pPlayerThread->m_lpDSBuffer)
		m_pPlayerThread->m_lpDSBuffer->SetVolume(vol);
	m_curVolume=vol;
}

BOOL CBasicPlayer::open( LPCTSTR filepath )
{
	if(!m_bStopped)return FALSE;

	int len=_tcslen(filepath);
	TCHAR* p=(TCHAR*)filepath+len;
	while (p--)
		if ((TCHAR)(*p)=='.'){
			p++;
			break;
		}
	
	if(m_pFile)
		delete m_pFile;
	m_bFileEnd=FALSE;
	
	if (_tcscmp(p,_T("wav"))==0 || _tcscmp(p,_T("WAV"))==0)
		m_pFile=new WaveFile();
	else if (_tcscmp(p,_T("mp3"))==0 || _tcscmp(p,_T("MP3"))==0)
		m_pFile=new Mp3File();
	else if (_tcscmp(p,_T("wma"))==0 || _tcscmp(p,_T("WMA"))==0)
		m_pFile=new Mp3File();
	else{
		MessageBox(m_pMainFrame->m_hWnd,_T("不支持的文件类型"),_T(""),MB_OK);
		return -1;
	}

	return m_pFile->Open(filepath);
}


void CBasicPlayer::play()
{
	if (m_bFileEnd)	return;
	
	if (!m_bStopped)
		if (m_bPaused){      //未停止且暂停
			pause();        //继续
			return;
		}
		else               //正在播放
			stop();        //先停止

	m_pFile->ResetFile();
	m_pPlayerThread->Reset();
	m_pPlayerThread->CleanDSBuffer();
	m_pPlayerThread->WriteDataToDSBuf();
	m_pPlayerThread->m_lpDSBuffer->Play( 0, 0, DSBPLAY_LOOPING);
	
	m_pPlayerThread->Init(FALSE);

	//if(m_pPlayerThread->Suspended())
	//	m_pPlayerThread->Resume();

	m_pPlayerThread->m_lpDSBuffer->SetVolume(DSBVOLUME_MAX);

	::PostMessage(m_pMainFrame->m_hWnd,WM_NEW_TRACK_STARTED,NULL,NULL);


	m_bStopped=FALSE;
	m_bPaused=FALSE;
}

void CBasicPlayer::pause()
{
	if (m_bStopped)return;

	if (m_bPaused){               
		m_bPaused=FALSE;
		m_pPlayerThread->Suspend();
	}
	else{                             
		m_bPaused=TRUE;
		m_pPlayerThread->Resume();
	}
}



void CBasicPlayer::stop()
{
	if(!m_bStopped){
		m_pPlayerThread->m_lpDSBuffer->Stop();
		
		//m_cs.Enter();
		m_pPlayerThread->Teminate();
		m_bStopped=TRUE;
		//m_cs.Leave();
	}
}

BOOL CBasicPlayer::open( PlayListItem *track)
{
	return open(track->url.c_str());
}

void CBasicPlayer:: SetPos(int cur,int max)
{
	if (!m_bStopped)
		m_pFile->SetPos(cur,max);
}