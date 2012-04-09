#pragma  once
#include "config.h"
static DWORD g_dwMaxDSBufferLen;  //ds buffer length 
static DWORD g_dwSleepTime;       //  1/4 of the ds buffer
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

	//-------------------
	//ds缓冲区大小
	g_dwMaxDSBufferLen=pwfx->nAvgBytesPerSec;
	g_dwSleepTime=(dsBufferDesc.dwBufferBytes/pwfx->nAvgBytesPerSec)*1000/8;//缓冲时间的1/8
	//-------------------

	return lpDSBuffer;
}


//返回剩余可写数据
DWORD DS_GetAvailable(int maxDSBufferLen , int playCursor, int curWritePos)
{
	if (playCursor>curWritePos)
		return playCursor-curWritePos;
	else
		return maxDSBufferLen+playCursor-curWritePos;
}

