#pragma  once




#define WIDTH(rc) ((rc).right-(rc.left))
#define HEIGHT(rc) ((rc).bottom-(rc.top))

//-----------------------------------------


static DWORD g_dwMaxDSBufferLen;  //ds buffer length 
static DWORD g_dwSleepTime;       //  1/4 of the ds buffer
int gDefaultBufferSize=16000;  //one time read from file

inline int DSoundGetDistance(int maxDSBufferLen , int pos1 , int pos2)
{
	int distance=pos2-pos1;
	if (distance<0)
		distance=distance+maxDSBufferLen;
	return distance;
}

LPDIRECTSOUND DSoundDeviceCreate(LPGUID lpGuid =NULL )
{
	LPDIRECTSOUND lpDSound;
	if(FAILED(DirectSoundCreate(lpGuid,&lpDSound,NULL) ))return NULL;

	//可以设置主缓冲的播放模式的级别
	// 	HWND hWnd=GetForegroundWindow();
	// 	if (!hWnd)
	// 		hWnd=GetDesktopWindow();

	if(FAILED(lpDSound->SetCooperativeLevel( GetDesktopWindow(), DSSCL_NORMAL) ))return NULL;
	return lpDSound;
}



LPDIRECTSOUNDBUFFER DSoundBufferCreate(WAVEFORMATEX *pwfx)
{
	static LPDIRECTSOUND lpDsound=DSoundDeviceCreate();

	//DSBSIZE_MIN  DSBSIZE_MAX
	//创建次缓冲区
	DSBUFFERDESC dsBufferDesc;
	ZeroMemory(&dsBufferDesc,sizeof(dsBufferDesc) );
	dsBufferDesc.dwSize=sizeof(DSBUFFERDESC);
	dsBufferDesc.dwFlags=DSBCAPS_GETCURRENTPOSITION2 | DSBCAPS_GLOBALFOCUS |DSBCAPS_CTRLVOLUME ;
	dsBufferDesc.dwBufferBytes=6*pwfx->nAvgBytesPerSec;
	dsBufferDesc.lpwfxFormat=pwfx;

	LPDIRECTSOUNDBUFFER lpDSBuffer=NULL;
	if(FAILED(lpDsound->CreateSoundBuffer(&dsBufferDesc,&lpDSBuffer,NULL)))
	{
		lpDSBuffer->Release();
		return NULL;
	}

	g_dwMaxDSBufferLen=6*pwfx->nAvgBytesPerSec;
	g_dwSleepTime=(dsBufferDesc.dwBufferBytes/pwfx->nAvgBytesPerSec)*1000/30;
	
	gDefaultBufferSize=pwfx->nAvgBytesPerSec;

	return lpDSBuffer;
}


//返回剩余可写数据量
DWORD DS_GetAvailable(int maxDSBufferLen , int playCursor, int curWritePos)
{
	if (playCursor>curWritePos)
		return playCursor-curWritePos;
	else
		return maxDSBufferLen+playCursor-curWritePos;
}


//返回已写入但未播放的数据量
DWORD DS_GetWritedNotPlayed(int maxDSBufferLen , int playCursor, int curWritePos)
{
	if (curWritePos>=playCursor)
		return curWritePos-playCursor;
	else
		return maxDSBufferLen+curWritePos-playCursor;
}

//return sleeped?true:false.
BOOL Sleep2WaitReadCursor(DWORD available)
{
	if (available<gDefaultBufferSize + 1)
	{
		::Sleep(g_dwSleepTime);
		return TRUE;
	}
	else
		return FALSE;
}



