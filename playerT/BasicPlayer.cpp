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


CBasicPlayer :: CBasicPlayer(void)
{
	m_pPlayerThread=new CPlayerThread(this);
	m_pSpectrumAnalyser=new CSpectrumAnalyser;
}

CBasicPlayer :: ~CBasicPlayer(void)
{
	if(!m_pPlayerThread) delete m_pPlayerThread;
	if (!m_pSpectrumAnalyser) delete m_pSpectrumAnalyser; 
}



void CBasicPlayer::start()
{
	m_bStopped=FALSE;
	m_pPlayerThread->Resume();
}


void CBasicPlayer::pause()
{
	m_pPlayerThread->Suspend();
}

void CBasicPlayer::stop()
{

}


void CBasicPlayer::open( LPTSTR filepath )
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

	if (_tcscmp(p,_T("wav"))==0)
	{
		m_pFile=new WaveFile();
	}
	if (_tcscmp(p,_T("mp3"))==0)
	{
		m_pFile=new Mp3File();
	}

	m_pFile->OpenAndReadID3Info(filepath);


	start();
	//m_pwfx=m_pFile->GetFormat();
	//m_dwTotalSamples=m_pFile->GetSize()*m_pwfx->nBlockAlign;
}