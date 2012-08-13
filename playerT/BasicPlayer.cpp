#include "stdafx.h"
#include "BasicPlayer.h"
#include "MusicFile.h"
#include "PlayerThread.h"
#include "SpectrumAnalyser.h"
#include "Mp3File.h"
#include "WaveFileEx.h"
#include "mainfrm.h"
#include "PlayList.h"

#include <complex>
using namespace std;


static DWORD CALLBACK WaitPlayThread(LPVOID lpParameter)
{
	CBasicPlayer* p=(CBasicPlayer*)lpParameter;
	p->WaitPlay();
	return 0;
}

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
	 ,m_curVolume(50)
{
	m_eventSlowDown=::CreateEvent(0,TRUE,TRUE,0);
	//nosignaled state,to enter the slowdown
	//signaled,to leave the slowdown

	InitSlowDownVolBuffer();
	m_pPlayerThread=new CPlayerThread(this);
	m_pSpectrumAnalyser=new CSpectrumAnalyser;
}

CBasicPlayer :: ~CBasicPlayer(void)
{
	delete[] volBuffer;
	if(!m_pPlayerThread) delete m_pPlayerThread;
	if (!m_pSpectrumAnalyser) delete m_pSpectrumAnalyser; 
}

void CBasicPlayer::ResetFile()
{
	m_pFile->ResetFile();
}


//0 by silence
//100 by ,the max volume
void CBasicPlayer:: SetVolumeByEar(int vol)
{
	int index=vol/2;
	if(index==50)index=49;
	
	if (m_pPlayerThread && m_pPlayerThread->m_lpDSBuffer)
		m_pPlayerThread->m_lpDSBuffer->SetVolume(volBuffer[index]);
	
	m_curVolume=index;
}

void CBasicPlayer::OpenAfterSlowDown()
{
	::CreateThread(NULL,0,WaitPlayThread,(LPVOID)this,0,0);
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
		//MessageBox(m_pMainFrame->m_hWnd,_T("不支持的文件类型"),_T(""),MB_OK);
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

	m_pPlayerThread->m_lpDSBuffer->SetVolume(DSBVOLUME_MIN);

	if(!m_pPlayerThread->CleanDSBuffer())return;
	m_pPlayerThread->WriteDataToDSBuf();
	m_pPlayerThread->Init(FALSE);

	m_pPlayerThread->m_lpDSBuffer->Play( 0, 0, DSBPLAY_LOOPING);
	m_bStopped=FALSE;
	m_bPaused=FALSE;
	InitSlowDown(FALSE);

	::PostMessage(m_pMainFrame->m_hWnd,WM_NEW_TRACK_STARTED,NULL,NULL);
}


//这个函数放在线程里,有问题,声音出不来,???
//所以改用消息通知主线程启动
//等淡入淡出效果结果
void CBasicPlayer::WaitPlay()
{
	DWORD waitResult=::WaitForSingleObject(m_eventSlowDown,1000);
	if (waitResult==WAIT_TIMEOUT)
		return;
	else if (waitResult==WAIT_OBJECT_0)//slowdown ending sign
		::SendMessage(m_pMainFrame->m_hWnd,WM_PLAY_DIRECTLY,NULL,NULL);
}

void CALLBACK SlowDownVolFunc(UINT uTimerID,UINT uMsg,DWORD dwUser,DWORD dw1,DWORD dw2)
{
	CBasicPlayer *p=(CBasicPlayer*)dwUser;
	p->SlowDownVol();
}


void CBasicPlayer::InitSlowDown(BOOL bSlowDown,BOOL bCloseFile)
{
	ResetEvent(m_eventSlowDown);
	m_bSlowDown=bSlowDown;
	if (!m_bSlowDown)
	{
		indexA=0;
		indexB=m_curVolume;
		indexVec=1;
	}
	else
	{
		indexA=m_curVolume;
		indexB=0;
		indexVec=-1;
	}

	indexPoint=indexA;
	m_bCloseFileInSlowDown=bCloseFile;
	m_timerID=::timeSetEvent(12,100,SlowDownVolFunc,(DWORD)this,TIME_PERIODIC|TIME_CALLBACK_FUNCTION); 
}


void CBasicPlayer::InitSlowDownVolBuffer()
{
	maxTimerCount=50;

	//buffer 0    10000 
	//buffer max  0
	volBuffer=new double[maxTimerCount];
	double aa;

	//-----------------------------------------
	//使用对数函数
	//-----------------------------------------
	//aa 为底数
	//aa=10000的开maxtimercount次方
	//即10000的1/maxtimercoun乘方
	aa=pow((double)maxTimerCount,(double)1/(double)abs(DSBVOLUME_MIN));
	
	for (int i=1;i<maxTimerCount;i++)
		volBuffer[i]=log10((double)i)/log10(aa);

	volBuffer[0]=DSBVOLUME_MAX;
	//volBuffer[maxTimerCount-1]=abs(DSBVOLUME_MIN);

	//set behind 0
	for (int i=0;i<maxTimerCount;i++)
		volBuffer[i]=volBuffer[i]-abs(DSBVOLUME_MIN);

}

void CBasicPlayer::SlowDownVol()
{
	m_pPlayerThread->m_lpDSBuffer->SetVolume(volBuffer[indexPoint]);

	indexPoint+=indexVec;

	if ( indexPoint == indexB)
	{
		::timeKillEvent( m_timerID);
		if ( m_bSlowDown)
		{
			m_pPlayerThread->m_lpDSBuffer->Stop();
			if(m_bCloseFileInSlowDown)
			{
				::PostMessage(m_pMainFrame->m_hWnd,WM_TRACKPOS,0,0);
				m_pPlayerThread->Teminate();
				m_pFile->Close();
			}
			else
			{
				m_pPlayerThread->Suspend();
			}
		}

		::SetEvent(m_eventSlowDown);
	}
}

void CBasicPlayer::pause()
{
	if (m_bStopped )return;

	if(::WaitForSingleObject(m_eventSlowDown,0)!=WAIT_OBJECT_0)
		return;

	if (!m_bPaused)
	{  
		m_bPaused=TRUE;
		InitSlowDown();
	}
	else
	{
		m_bPaused=FALSE;
		m_pPlayerThread->Resume();
		m_pPlayerThread->m_lpDSBuffer->Play( 0, 0, DSBPLAY_LOOPING);
		InitSlowDown(FALSE);
	}
}



void CBasicPlayer::stop()
{
	if(::WaitForSingleObject(m_eventSlowDown,0)!=WAIT_OBJECT_0)
		return;

	if(!m_bStopped )
	{
		m_bStopped=TRUE;

		if (!m_bPaused)
		{
			InitSlowDown(TRUE,TRUE);
		}
		else
		{
			m_pPlayerThread->m_lpDSBuffer->Stop();
			m_pPlayerThread->Teminate();
			m_pFile->Close();
		}
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