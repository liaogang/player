#pragma  once

LPDIRECTSOUND DSoundDeviceCreate(LPGUID lpGuid =NULL )
{
	LPDIRECTSOUND lpDSound;

	if(FAILED(DirectSoundCreate(lpGuid,&lpDSound,NULL) ))return NULL;

	//可以设置主缓冲的播放模式的级别
	HWND hWnd=GetForegroundWindow();
	if (!hWnd)
		hWnd=GetDesktopWindow();

	if(FAILED(lpDSound->SetCooperativeLevel( hWnd, DSSCL_NORMAL) ))return NULL;

	return lpDSound;
}


LPDIRECTSOUNDBUFFER DSoundBufferCreate(LPDIRECTSOUND lpDSound,WAVEFORMATEX *pwfx)
{
	//DSBSIZE_MIN  DSBSIZE_MAX
	//创建次缓冲区
	DSBUFFERDESC dsBufferDesc;
	ZeroMemory(&dsBufferDesc,sizeof(dsBufferDesc) );
	dsBufferDesc.dwSize=sizeof(DSBUFFERDESC);
	dsBufferDesc.dwFlags=DSBCAPS_GETCURRENTPOSITION2 | DSBCAPS_GLOBALFOCUS;
	dsBufferDesc.dwBufferBytes=pwfx->nAvgBytesPerSec;   //1秒缓冲
	dsBufferDesc.lpwfxFormat=pwfx;

	LPDIRECTSOUNDBUFFER lpDSBuffer=NULL;
	if(FAILED(lpDSound->CreateSoundBuffer(&dsBufferDesc,&lpDSBuffer,NULL)))return NULL;

	return lpDSBuffer;
}


DWORD DSoundBufferWrite(LPDIRECTSOUNDBUFFER lpDSBuffer, void* pBuf , int len)
{
	DWORD playCursor,writeCursor;

	LRESULT result= lpDSBuffer->GetCurrentPosition(&playCursor,&writeCursor);
	
	if (FAILED(result)) return -1;



	                                                                                            
	return 0;
}

inline int DSoundGetDistance(int maxDSBufferLen , int readPos , int writePos)
{
	return readPos-writePos + readPos>writePos?0:maxDSBufferLen; 
}
// 
// void a ()
// {
// 	//New audio buffer
// 	if (m_pAudioBuf!=NULL)
// 	{
// 		delete []m_pAudioBuf;
// 		m_pAudioBuf=NULL;
// 	}
// 	m_pAudioBuf=new BYTE[m_pwfx->nAvgBytesPerSec];
// 
// 	memset(m_pAudioBuf,0,m_pwfx->nAvgBytesPerSec);
// 
// 	//new fft data buffer
// 	if (m_pFFTBuffer!=NULL)
// 	{
// 		delete []m_pFFTBuffer;
// 		m_pFFTBuffer=NULL;
// 	}
// 
// 	m_pFFTBuffer=new BYTE[m_pwfx->nAvgBytesPerSec*2];
// 
// 	memset(m_pFFTBuffer,0,m_pwfx->nAvgBytesPerSec*2);
// 
// 	return 0;
// 
// }