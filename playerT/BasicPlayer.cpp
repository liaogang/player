#include "stdafx.h"

#include "BasicPlayer.h"
#include "MusicFile.h"
#include "PlayerThread.h"
#include "SpectrumAnalyser.h"
#include "Mp3File.h"
#include "WaveFileEx.h"
#include "PlayList.h"
#include "forwardMsg.h"
#include "globalStuffs.h"
#include <complex>
using namespace std;

static TrackFormatInfo *gTrackFormatInfo;
void SetTrackFormatInfo(int type,int rate,int samplesps,int mode)
{
	if(gTrackFormatInfo==nullptr)
		gTrackFormatInfo=new TrackFormatInfo;

	gTrackFormatInfo->type=type;
	gTrackFormatInfo->rate=rate;
	gTrackFormatInfo->nSamplesPerSec=samplesps;
	gTrackFormatInfo->mode=mode;
}


TrackFormatInfo* GetTrackFormatInfo()
{
	mpg123_frameinfo info;
	CBasicPlayer::shared()->m_pFile->GetFrameInfo(&info);
	SetTrackFormatInfo(1,info.bitrate,info.rate,info.mode);

	return gTrackFormatInfo;
}


static trackPosInfo curPosInfo;
trackPosInfo *getTrackPosInfo()
{
	if( !CBasicPlayer::shared()->stoped() && CBasicPlayer::shared()->m_pFile)
		CBasicPlayer::shared()->m_pFile->GetPos(&(curPosInfo.used),&(curPosInfo.left));

	double secPlayed=CBasicPlayer::shared()->m_pPlayerThread->GetPlayedSeconds();

	curPosInfo.used=secPlayed;
	//curPosInfo.left-=secPlayed;

	return &curPosInfo;
}

void setTrackPosInfo(trackPosInfo info)
{
	curPosInfo=info;
}



///////////////////////////////////////
 static CBasicPlayer *pGlobalBasePlayer=NULL;
  CBasicPlayer* const CBasicPlayer::shared()
 {
	 if(!pGlobalBasePlayer)
		 pGlobalBasePlayer=new CBasicPlayer();
	 return pGlobalBasePlayer;
 }

 CBasicPlayer :: CBasicPlayer(void):
 m_bStopped(TRUE),m_bPaused(TRUE),
	 m_pFile(NULL),m_bFileEnd(TRUE)
	 ,m_curVolume(50)
{
	//m_eventSlowDown=::CreateEvent(0,TRUE,TRUE,0);
	//nosignaled state,to enter the slowdown
	//signaled,to leave the slowdown

	InitSlowDownVolBuffer();
	m_pPlayerThread=new CPlayerThread(this);
	m_pSpectrumAnalyser=new CSpectrumAnalyser(this);

	
}

CBasicPlayer :: ~CBasicPlayer(void)
{
	stop(TRUE);
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
	if(vol>100)return;

	m_curVolume=vol;

	int index=vol/2;
	if(index==50)index=49;
	m_curDSBVolume=volBuffer[index];
	
	if(!m_bStopped && !m_bPaused)
		m_pPlayerThread->m_lpDSBuffer->SetVolume(m_curDSBVolume);
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
	else
		{return FALSE;}
 
	BOOL result=m_pFile->Open(filepath);
	if(result==FALSE)
		delete m_pFile;
	return result;
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

	m_bStopped=FALSE;
	m_bPaused=FALSE;

	if(curPosInfo.used==-1)
		m_pFile->ResetFile();
	else
		m_pFile->SetPos(curPosInfo.used-7>0?curPosInfo.used-7:curPosInfo.used,curPosInfo.used+curPosInfo.left);
	

	m_pPlayerThread->Reset();

	m_pPlayerThread->CleanDSBuffer();

	m_pPlayerThread->Init(FALSE);
	
	m_pPlayerThread->m_lpDSBuffer->SetVolume(DSBVOLUME_MIN);
	TimerVolGrowUp();
	m_pPlayerThread->m_lpDSBuffer->Play( 0, 0, DSBPLAY_LOOPING);

	NotifyMsg(WM_NEW_TRACK_STARTED,(WPARAM)NULL,0);
}




void CALLBACK SlowDownVolFunc(UINT uTimerID,UINT uMsg,DWORD dwUser,DWORD dw1,DWORD dw2)
{
	CBasicPlayer *p=(CBasicPlayer*)dwUser;
	p->SlowDownVol();
}

void CALLBACK GrowUpVolFunc(UINT uTimerID,UINT uMsg,DWORD dwUser,DWORD dw1,DWORD dw2)
{
	CBasicPlayer *p=(CBasicPlayer*)dwUser;
	p->GrowUpVol();
}



void CBasicPlayer::SlowDownVol()
{
	--curVolDown;
	m_pPlayerThread->m_lpDSBuffer->SetVolume(volBuffer[curVolDown]);

	if(curVolDown==0)
	{
		timeKillEvent(m_timerDownID);
		m_pPlayerThread->m_lpDSBuffer->Stop();
		if(m_bCloseFileInSlowDown)
		{
			NotifyMsg(WM_TRACKSTOPPED);
			m_pPlayerThread->Teminate();
			m_pFile->Close();	
		}
		else
		{
			m_cs.Enter();
			NotifyMsg(WM_PAUSED);
			m_pPlayerThread->Suspend();
			m_cs.Leave();
		}
	}
}

void CBasicPlayer::GrowUpVol()
{
	curVolUp+=2;

	m_pPlayerThread->m_lpDSBuffer->SetVolume(volBuffer[curVolUp]);

	if(curVolUp>=m_curVolume/2)
	{
		timeKillEvent(m_timerUpID);
	}
}

void CBasicPlayer::TimerVolSlowDown()
{	
	 curVolDown=m_curVolume/2;
	 if(curVolDown==50)curVolDown=49;
	m_timerDownID=::timeSetEvent(12,100,SlowDownVolFunc,(DWORD)this,TIME_PERIODIC|TIME_CALLBACK_FUNCTION); 
}

void CBasicPlayer::TimerVolGrowUp()
{
	curVolUp=0;
	m_timerUpID=::timeSetEvent(12,100,GrowUpVolFunc,(DWORD)this,TIME_PERIODIC|TIME_CALLBACK_FUNCTION); 
}





void CBasicPlayer::InitSlowDownVolBuffer()
{
	const int maxTimerCount=50;

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



void CBasicPlayer::pause()
{
	if (m_bStopped )return;

	if (!m_bPaused)
	{  
		m_bPaused=TRUE;
		m_bCloseFileInSlowDown=FALSE;
		TimerVolSlowDown();
	}
	else
	{
		m_bPaused=FALSE;
		m_pPlayerThread->Resume();
		m_pPlayerThread->m_lpDSBuffer->Play( 0, 0, DSBPLAY_LOOPING);
		
		trackPosInfo *posInfo=getTrackPosInfo();
		m_pFile->GetPos(&(posInfo->used),&(posInfo->left));
		NotifyMsg(WM_PAUSE_START);
		TimerVolGrowUp();
	}
}



void CBasicPlayer::stop(BOOL bDestroy)
{
	if(!m_bStopped )
	{
		m_cs.Enter();
		m_bStopped=TRUE;
		
		m_pPlayerThread->m_lpDSBuffer->Stop();

		if(bDestroy)//cllocect info
		{
			getTrackPosInfo();
		}
		else  //clear info
			curPosInfo.used=-1;

		NotifyMsg(WM_TRACKSTOPPED);

		m_pPlayerThread->Teminate();
		m_pFile->Close();


		m_cs.Leave();
	}
}

BOOL CBasicPlayer::open( FileTrack *track)
{
	return open(track->url.c_str());
}

//set pos in playing
void CBasicPlayer::SetPos(int cur,int max)
{
	if (!m_bStopped)
	{
		m_cs.Enter();
	
		m_pPlayerThread->BeginChangeTrackPos();
	
		m_pFile->SetPos(cur,max);
		
		NotifyMsg(WM_TRACK_POS_CHANGED,TRUE);

		m_pPlayerThread->EndChangeTrackPos();
		m_cs.Leave();
	}
}

void CBasicPlayer::GetPos(int *cur,int *max)
{
	*cur=m_pPlayerThread->pPosInfo->left;
	
	if(max)
		*max=*cur+m_pPlayerThread->pPosInfo->used;
}