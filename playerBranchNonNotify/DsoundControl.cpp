
#include "StdAfx.h"
#include "DsoundControl.h"
#include "WaveFileEx.h"
#include "Mp3File.h"
#include "MainFrm.h"
//-------------------------------------------------------------
INT   intPeaks[90];
INT	  intPeaksDelay[90];
INT   intLastBarHeight[90];
INT   intLastPeaks[90];
#define    HATHEIGHT    8
#define  BARHEIGHT   6

#define WIDTH(rc) ((rc).right-(rc.left))
#define HEIGHT(rc) ((rc).bottom-(rc.top))
//-------------------------------------------------------------
void CALLBACK CALLBACK_TimerNotify(UINT uTimerID,UINT uMsg,DWORD dwUser,DWORD dw1,DWORD dw2)
{
	DsoundControl *pDsoundCtl=(DsoundControl*)dwUser;
	if (pDsoundCtl->m_bStatus==playing)
	{
		pDsoundCtl->TimerNotify();
		
		double used,lefted;
		pDsoundCtl->m_pfile->GetPos(&used,&lefted);
		::SendMessage(pDsoundCtl->m_pMainFrame->m_hWnd,WM_USER+22,used,lefted);
	}
}
//-------------------------------------------------------------
void CALLBACK CallBack_TimerConvertDataToFFT(UINT uTimerID,UINT uMsg,DWORD dwUser,DWORD dw1,DWORD dw2)
{
	DsoundControl *pDsoundCtrl=(DsoundControl*)dwUser;
	if (pDsoundCtrl->m_bStatus==playing)
		pDsoundCtrl->ConvertDataToFFT();
} 


//#define m_iSpectrum_Width		900
//#define m_iSpectrum_Hight		300
//#define m_iSpectrum_Decay		0.05
//#define m_iSpectrum_Bands		90
//#define m_iSpectrum_Delay		20



DsoundControl::DsoundControl(void):m_pAudioBuf(NULL),m_dwCircles1(0),m_dwCircles2(0)
								,m_ppDS(NULL),m_dsBuffer2(NULL),m_timerID(0)
								,m_pFFTBuffer(NULL),m_bStatus(stopped)
								,m_iSpectrum_Decay(0.05)
								,m_iSpectrum_Bands(60),m_iSpectrum_Delay(15),m_pWndShow(NULL)
{
	m_pHEvent[0] = CreateEvent(NULL, FALSE, FALSE, _T("Direct_Sound_Buffer_Notify_0"));
	m_pHEvent[1] = CreateEvent(NULL, FALSE, FALSE, _T("Direct_Sound_Buffer_Notify_1"));	
}

DsoundControl::~DsoundControl(void)
{
	if (NULL != m_pAudioBuf) 
	{
		delete []m_pAudioBuf;
		m_pAudioBuf = NULL;
	}









	// 	//Do not forget to clean up.
	// 	SelectObject(m_memDC, hpenOld);
	// 	DeleteObject(hpen);
	// 
	// 	SelectObject(m_memDC, hbrushOld);
	// 	DeleteObject(hbrush);
	// 
	// 	SelectObject(m_memDC, hbrushOld1);
	// 	DeleteObject(hbrush1);
	// 
	// 	SelectObject(m_memDC,hbrushOld2);
	// 	DeleteObject(hbrush2);
	// 
	// 
	// 	SelectObject(m_memDC,oldBitmap);
	// 	DeleteObject(m_bitmap);
	// 
	// 	Sleep(20);
}

//-------------------------------------------------------------
//timer to process notify event
void  DsoundControl::TimerNotify()
{
	LPVOID lpvAudio1 = NULL, lpvAudio2 = NULL;
	DWORD dwBytesAudio1 = 0, dwBytesAudio2 = 0;
	DWORD dwRetSamples = 0, dwRetBytes = 0;

	HRESULT hr=WaitForMultipleObjects(2,m_pHEvent,FALSE,0);
	if (hr==WAIT_OBJECT_0)
	{
		m_dwCircles1++;

		//Lock DirectSoundBuffer Second Part
		HRESULT hr=m_dsBuffer2->Lock(m_pwfx->nAvgBytesPerSec,m_pwfx->nAvgBytesPerSec,&lpvAudio1,&dwBytesAudio1,&lpvAudio2,&dwBytesAudio2,0);
		if(FAILED(hr)) return;  
	}
	else if (hr==WAIT_OBJECT_0+1)
	{
		m_dwCircles2++;

		//Lock DirectSoundBuffer Firest Part
		HRESULT hr=m_dsBuffer2->Lock(0,m_pwfx->nAvgBytesPerSec,&lpvAudio1,&dwBytesAudio1,&lpvAudio2,&dwBytesAudio2,0);
		if(FAILED(hr))return;

	}
	else
	{
		return;
	}

	//Get 1 Second Audio Buffer
	GetAudioSampleDatas(m_pAudioBuf,m_pwfx->nSamplesPerSec,dwRetSamples);
	dwRetBytes=dwRetSamples*m_pwfx->nBlockAlign;

	//if near the end of the audio data
	if (dwRetSamples<m_pwfx->nSamplesPerSec)
	{
		DWORD dwRetBytes=dwRetSamples*m_pwfx->nBlockAlign;
		memset(m_pAudioBuf+dwRetBytes,0,m_pwfx->nAvgBytesPerSec-dwRetBytes);
	}

	//Copy AudioBuffer to DirectSoundBuffer
	if (NULL==lpvAudio2)
	{
		memcpy(lpvAudio1,m_pAudioBuf,dwBytesAudio1);
		memcpy(m_pFFTBuffer,m_pAudioBuf,dwBytesAudio1);
	}
	else
	{
		memcpy(lpvAudio1,m_pAudioBuf,dwBytesAudio1);
		memcpy(lpvAudio2,m_pAudioBuf+dwBytesAudio1,dwBytesAudio2);
		memcpy(m_pFFTBuffer,m_pAudioBuf,dwBytesAudio1+dwBytesAudio2);
	}

	//unlock
	m_dsBuffer2->Unlock(lpvAudio1,dwBytesAudio1,lpvAudio2,dwBytesAudio2);
}


BOOL DsoundControl::InitDSound()
{
	//创建DirectSound设备

	if(FAILED(DirectSoundCreate(NULL,&m_ppDS,NULL) ))return -1;


	//可以设置主缓冲的播放模式的级别
	HWND hWnd=GetForegroundWindow();
	if (hWnd==NULL)
	{
		hWnd=GetDesktopWindow();
	}
	//m_pMainFrame->m_hWnd

	if(FAILED(m_ppDS->SetCooperativeLevel(hWnd,DSSCL_PRIORITY/*|DSSCL_WRITEPRIMARY*/ ) ))return -1;
	//m_ppDS.GetCaps();

	//创建主缓冲区
	DSBUFFERDESC dsBufferDesc;
	ZeroMemory(&dsBufferDesc,sizeof(dsBufferDesc) );
	dsBufferDesc.dwSize=sizeof(DSBUFFERDESC);
	dsBufferDesc.dwFlags=DSBCAPS_GETCURRENTPOSITION2|DSBCAPS_PRIMARYBUFFER|DSBCAPS_LOCSOFTWARE  /*|DSBCAPS_LOCDEFER*/|DSBCAPS_STICKYFOCUS|DSBCAPS_CTRLVOLUME ;
	dsBufferDesc.dwBufferBytes=0;
	dsBufferDesc.lpwfxFormat=NULL;
	LPDIRECTSOUNDBUFFER lpDSB=NULL;
	if(FAILED(m_ppDS->CreateSoundBuffer(&dsBufferDesc,&lpDSB,NULL)))return -1;

	//设置主缓冲区模式                      
	if (FAILED(lpDSB->SetFormat(m_pwfx))) return -1;

	//创建次缓冲区
	dsBufferDesc.dwFlags=DSBCAPS_GETCURRENTPOSITION2|DSBCAPS_CTRLPOSITIONNOTIFY|DSBCAPS_LOCSOFTWARE|DSBCAPS_GLOBALFOCUS/*|DSBCAPS_LOCDEFER*/|DSBCAPS_STICKYFOCUS ;//position回放 global全局声音
	dsBufferDesc.dwBufferBytes=2*m_pwfx->nAvgBytesPerSec;   //2秒缓冲
	dsBufferDesc.lpwfxFormat=m_pwfx;


	if (FAILED(m_ppDS->CreateSoundBuffer(&dsBufferDesc,&m_dsBuffer2,NULL))) return -1;

	//query directsound_Notify 接口
	LPDIRECTSOUNDNOTIFY lpDSBNotify;
	if(FAILED(m_dsBuffer2->QueryInterface(IID_IDirectSoundNotify,(LPVOID*)&lpDSBNotify))) return -1;

	//Set Direct Sound Buffer Notify Position
	DSBPOSITIONNOTIFY pPosNotify[2];
	pPosNotify[0].dwOffset=0;//m_pwfx->nAvgBytesPerSec/2-1;
	pPosNotify[1].dwOffset=m_pwfx->nAvgBytesPerSec;//3*m_pwfx->nAvgBytesPerSec/2-1;
	pPosNotify[0].hEventNotify=m_pHEvent[0];
	pPosNotify[1].hEventNotify=m_pHEvent[1];
	if (FAILED(lpDSBNotify->SetNotificationPositions(2,pPosNotify))) return -1;


	//New audio buffer
	if (m_pAudioBuf!=NULL)
	{
		delete []m_pAudioBuf;
		m_pAudioBuf=NULL;
	}
	m_pAudioBuf=new BYTE[m_pwfx->nAvgBytesPerSec];

	memset(m_pAudioBuf,0,m_pwfx->nAvgBytesPerSec);

	//new fft data buffer
	if (m_pFFTBuffer!=NULL)
	{
		delete []m_pFFTBuffer;
		m_pFFTBuffer=NULL;
	}
	
	m_pFFTBuffer=new BYTE[m_pwfx->nAvgBytesPerSec*2];
	
	memset(m_pFFTBuffer,0,m_pwfx->nAvgBytesPerSec*2);


	return 0;
}

HRESULT DsoundControl::GetAudioSampleDatas(LPBYTE lpDesBuf,const DWORD dwRequiredSamples,DWORD &dwRetSamples)
{
	DWORD dwRequiredBytes=0,dwRetBytes=0;
	WAVEFORMATEX *pWFX=m_pfile->GetFormat();

	dwRequiredBytes=dwRequiredSamples*pWFX->nBlockAlign;
	m_pfile->Read(lpDesBuf,dwRequiredBytes,&dwRetBytes);

	dwRetSamples=dwRetBytes/pWFX->nBlockAlign;
	return dwRetBytes;
}

void  DsoundControl::Play()
{
	if (m_bStatus==paused)
	{
		Pause();
		return;
	}
	else if (m_bStatus==playing)
	{
		Stop();
	}

	m_pfile->ResetFile();
	//获得数据
	DWORD dwRetSamples=0,dwRetBytes=0;

	GetAudioSampleDatas(m_pAudioBuf,dwRetSamples,dwRetBytes);

	//把数据拷至Direct sound缓冲区
	LPVOID lpvAudio1=NULL,lpvAudio2=NULL;
	DWORD dwBytesAudio1=0,dwBytesAudio2=0;
	HRESULT hr=m_dsBuffer2->Lock(0,m_pwfx->nAvgBytesPerSec,&lpvAudio1,&dwBytesAudio1,&lpvAudio2,&dwBytesAudio2,NULL);
	if(FAILED(hr)) return ;

	if (lpvAudio1!=NULL) memset(lpvAudio1,0,dwBytesAudio1);
	if (lpvAudio2!=NULL) memset(lpvAudio2,0,dwBytesAudio2);


	if(lpvAudio2==NULL)
	{
		memcpy(lpvAudio1,m_pAudioBuf,dwRetBytes);
		memcpy(m_pFFTBuffer,m_pAudioBuf,dwRetBytes);
	}
	else
	{
		memcpy(lpvAudio1,m_pAudioBuf,dwBytesAudio1);
		memcpy(lpvAudio2,m_pAudioBuf+dwBytesAudio1,dwBytesAudio2);
		memcpy(m_pFFTBuffer,m_pAudioBuf,dwBytesAudio1+dwBytesAudio2);
	}

	//Unlock DirectSoundBuffer
	m_dsBuffer2->Unlock(lpvAudio1,dwBytesAudio1,lpvAudio2,dwBytesAudio2);


	
	m_timerID=timeSetEvent(500,100,CALLBACK_TimerNotify,(DWORD)this,TIME_PERIODIC|TIME_CALLBACK_FUNCTION);
	
	m_timerFFT=timeSetEvent(80,0,CallBack_TimerConvertDataToFFT,(DWORD)this,TIME_PERIODIC|TIME_CALLBACK_FUNCTION);

	//播放
	m_dsBuffer2->Play(0,0,DSBPLAY_LOOPING);                    
	m_bStatus=playing;
}

void DsoundControl::OpenFile(LPTSTR filepath)
{
	//m_wavefile.Open(filepath,&m_pwfx,WAVEFILE_READ);
	//m_pwfx=*m_wavefile.GetFormat();
	//m_dwTotalSamples=m_wavefile.GetSize()*m_pwfx->nBlockAlign;
	//InitDSound();

	
	
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
		m_pfile=new WaveFile();
	}
	if (_tcscmp(p,_T("mp3"))==0)
	{
		m_pfile=new Mp3File();
	}

	m_pfile->OpenAndReadID3Info(filepath);
	m_pwfx=m_pfile->GetFormat();
	m_dwTotalSamples=m_pfile->GetSize()*m_pwfx->nBlockAlign;

	InitDSound();
}


void DsoundControl::Pause()
{
	if (NULL != m_dsBuffer2&&m_bStatus==playing)
	{
		m_dsBuffer2->Stop();
		timeKillEvent(m_timerFFT);
		m_bStatus=paused;
	}
	else if (m_bStatus==paused)
	{
		m_dsBuffer2->Play(0,0,DSBPLAY_LOOPING);
		m_timerFFT=timeSetEvent(200,0,CallBack_TimerConvertDataToFFT,(DWORD)this,TIME_PERIODIC|TIME_CALLBACK_FUNCTION);
		m_bStatus=playing;
	}
}

void DsoundControl::Stop()
{
	if (m_dsBuffer2!=NULL && m_bStatus!=stopped)
	{
		m_dsBuffer2->Stop();                   
		timeKillEvent(m_timerID);
		timeKillEvent(m_timerFFT);

		LPVOID lpvEntireLock;
		DWORD dwEntireLockSize;
		HRESULT hr=m_dsBuffer2->Lock(0,0,&lpvEntireLock,&dwEntireLockSize,0,0,DSBLOCK_ENTIREBUFFER);
		if(FAILED(hr)) return;

		memset(lpvEntireLock,0,dwEntireLockSize);

		m_dsBuffer2->Unlock(lpvEntireLock,dwEntireLockSize,NULL,NULL);

		ResetEvent(m_pHEvent[0]);
		ResetEvent(m_pHEvent[1]);

		m_dwCircles1=0;
		m_dwCircles2=0;

		m_bStatus=stopped;
	}
}


//返回以播放过的数据DWORD值                   
DWORD DsoundControl::GetSamplesPlayed(DWORD* pCurPlayPos/*取得播放游标的值*/)
{
	if (NULL == m_ppDS)
	{
		return 0;
	}

	//Get curren play position
	DWORD dwCurPlayPos = 0, dwCurPlaySample = 0;

	//pdwCurrentPlayCursor
	//Address of a variable that receives the offset, 
	//in bytes, of the play cursor. 
	//This parameter can be NULL if the value is not wanted.
	if (m_dsBuffer2!=NULL)
	{
		m_dsBuffer2->GetCurrentPosition(&dwCurPlayPos, NULL);
	}
	
	*pCurPlayPos = dwCurPlayPos;
	dwCurPlaySample = dwCurPlayPos/m_pwfx->nBlockAlign;

	//Caculate the samples played
	DWORD dwSamplesPlayed = 0;
	if (m_dwCircles2 < 1)
	{		
		return dwCurPlaySample;
	}

	dwSamplesPlayed = (m_dwCircles2-1)*2*m_pwfx->nSamplesPerSec + 3*m_pwfx->nSamplesPerSec/2;
	if (dwCurPlaySample > (3*m_pwfx->nSamplesPerSec/2))
	{
		if (m_dwCircles2 < m_dwCircles1)
		{
			dwSamplesPlayed = (m_dwCircles1-1)*2*m_pwfx->nSamplesPerSec + 3*m_pwfx->nSamplesPerSec/2;
		}

		dwSamplesPlayed += dwCurPlaySample - 3*m_pwfx->nSamplesPerSec/2 + 1;
	}
	else
	{
		dwSamplesPlayed += dwCurPlaySample + m_pwfx->nSamplesPerSec/2;
	}

	//CString strSamplesPlayed;
	//strSamplesPlayed.Format(_T("Samples Played: %d \n"), dwSamplesPlayed);
	//OutputDebugString(strSamplesPlayed);

	return dwSamplesPlayed;
}



//-------------------------------------------------------------
//timer to process fft things 
//1.copy
//-------------------------------------------------------------
void  DsoundControl::ConvertDataToFFT()
{
	DWORD dwCurPlayPos = 0;
	DWORD dwSamplesPlayed =GetSamplesPlayed(&dwCurPlayPos);
	DWORD dw2SecondByteSize = m_pwfx->nAvgBytesPerSec;

	if(dwSamplesPlayed >=m_dwTotalSamples)//文件播放结束了
	{
		Stop();
		timeKillEvent(m_timerID);
		m_timerID = 0;
		memset(m_floatMag, 0, FFT_SAMPLE_SIZE/2);
	}
	else
	{
		if(m_pAudioBuf == NULL)
			return;


	/*	是傅里叶变换在时域和频域上都呈离散的形式，将信号的时域采样变换为其DTFT的频域采样
		。在形式上，变换两端（时域和频域上）的序列是有限长的，而实际上这两组序列都应当被
			认为是离散周期信号的主值序列。即使对有限长的离散信号作DFT，也应当将其看作其周期
			延拓的变换。在实际应用中通常采用快速傅里叶变换计算DFT。
	*/

		float left, right;
		for(int i=0;i<FFT_SAMPLE_SIZE;i++) //FFT_SAMPLE_SIZE 2048  2048*4约等于4410*2
		{
			if(dwCurPlayPos >=dw2SecondByteSize)
				dwCurPlayPos -= dw2SecondByteSize;
			//dwCurPlayPos-=dw2SecondByteSize/2;

			//傅立叶数据采样
			left = (float)((m_pAudioBuf[dwCurPlayPos+1] << 8) + m_pAudioBuf[dwCurPlayPos+0])/32767.0F;
			right = (float)((m_pAudioBuf[dwCurPlayPos+3] << 8) + m_pAudioBuf[dwCurPlayPos+2])/32767.0F;
			m_floatSamples[i] = (left+right)/2.0F;
			dwCurPlayPos+=4;
		}

		FFT* fft = new FFT(FFT_SAMPLE_SIZE);
		float* lpFloatFFTData = fft->calculate(m_floatSamples, FFT_SAMPLE_SIZE);
		memcpy(m_floatMag, lpFloatFFTData, FFT_SAMPLE_SIZE/2);

		DrawSpectrum();

		
	}
}






void DsoundControl::DrawSpectrum()
{
	RECT rect(m_Spectrum_Rect);
	int cx=WIDTH(rect);
	int cy=HEIGHT(rect);

	#define PI_2 6.283185F
	#define PI   3.1415925F

// 	// Select the new pen and brush, and then draw.
// 	m_hpenOld = (HPEN)SelectObject(m_memDC, m_hpen);
// 	m_hbrushOld = (HBRUSH)SelectObject(m_memDC, m_hbrush);


	Rectangle(m_memDC, rect.left, rect.top, rect.right, rect.bottom);


	float c = 0;
	float floatFrrh = 1.0;
	float floatDecay = (float)m_iSpectrum_Decay;
	float floatSadFrr = (floatFrrh*floatDecay);
	float floatBandWidth = ((float)(cx)/(float)m_iSpectrum_Bands);  //band宽度=总宽 / 数量
	float floatMultiplier = 2.0;


	RECT r;//下面长条块
	for(int a=0, band=0; band < m_iSpectrum_Bands; a+=(int)floatMultiplier, band++)
	{
		float wFs = 0;

		// -- Average out nearest bands.
		for (int b = 0; b < floatMultiplier; b++)
			wFs += m_floatMag[a + b];

		// -- Log filter.
		wFs = (wFs * (float) log((float)(band + 2)));
		
		if (wFs>0.005f && wFs <0.1f)
			wFs*=0.9f * PI;
		else if (wFs >0.01f && wFs <0.1f)
			wFs*=3.0f * PI;
		else if ( wFs > 0.1f && wFs < 0.5f)
			wFs*=PI;

		if (wFs > 1.0f) 
			wFs = 0.9f;

		// -- Compute SA decay...
		if (wFs >= (m_floatMag[a] - floatSadFrr)) 
		{
			m_floatMag[a] = wFs;
		} 
		else
		{
			m_floatMag[a] -= floatSadFrr;
			if (m_floatMag[a] < 0) 
				m_floatMag[a] = 0;
			wFs = m_floatMag[a];
		}
		
		drawSpectrumAnalyserBar(&rect,c,cy,floatBandWidth-1,wFs*cy,band);
		c += floatBandWidth;
	}

	::BitBlt(m_hdc,0,0,cx,cy,m_memDC,0,0,SRCCOPY);
}


void DsoundControl::SetSpectrumRect(CRect rc)
{
	m_Spectrum_Rect=rc;
}


DsoundControl* DsoundControl::shared()
{
	static DsoundControl *p=NULL;
	if (!p)
		p=new DsoundControl;
	return p;
}

void DsoundControl::drawSpectrumAnalyserBar(RECT *pRect,int x,int y,int width,int height,int band)
{

	RECT barRect;
	barRect.left=x;
	barRect.right=x+width;
	barRect.top=pRect->top+height;
	barRect.bottom=pRect->bottom;

	FillRect(m_memDC, &barRect, m_hbrush1);



// 	//上面的小帽子滑块
// 	if(height > intPeaks[band])             //上涨
// 	{
// 		//intLastBarHeight[band] = height;
// 		intPeaks[band] = height;                 //peak 另一半长度
// 		intPeaksDelay[band] = m_iSpectrum_Delay;       //delay  一次下降高度
// 	}
// 	else                                               //滑块下降
// 	{
// 		intPeaksDelay[band]--;
// 		if (intPeaksDelay[band] < 0) 
// 			intPeaks[band]-=3;;
// 
// 		if (intPeaks[band] < 0) 
// 			intPeaks[band] = 0;
// 	}
// 
// 	RECT peakRect;
// 	peakRect.left=x;
// 	peakRect.right=peakRect.left+width;
// 	peakRect.top=y-intPeaks[band];
// 	peakRect.bottom=peakRect.top+BARHEIGHT;
// 	FillRect(m_memDC,&peakRect,m_hbrush2);

// 	r.top -= intPeaks[band];
// 
// 	if(r.top < rect.top)
// 		r.top = rect.top + 2;
// 
// 	if(r.top >= rect.bottom)
// 		r.top = rect.bottom - 2;
// 
// 	r.bottom = r.top + HATHEIGHT;
// 	FillRect(m_memDC, &r, m_hbrush2);

}
