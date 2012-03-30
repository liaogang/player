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

DWORD DS_GetAvailable(int maxDSBufferLen , int playCursor, int curWritePos)
{
	if (playCursor>curWritePos)
		return playCursor-curWritePos;
	else
		return maxDSBufferLen+playCursor-curWritePos;
}

DWORD DSoundBufferWrite(LPDIRECTSOUNDBUFFER lpDSBuffer, void* pBuf , int len)
{
	static DWORD curWritePos=0;
	static BOOL bFirstIn=TRUE; 

	HRESULT result;
	DWORD reMainderSize=-1;
	DWORD playCursor,writeCursor;

	result= lpDSBuffer->GetCurrentPosition(&playCursor,&writeCursor);
	
	if (FAILED(result)) return -1;

	if(bFirstIn)
	{
		curWritePos=writeCursor;
		bFirstIn=FALSE;
	}

	DWORD available=DS_GetAvailable(g_dwMaxDSBufferLen,playCursor,curWritePos);
	if (available>g_dwMaxDSBufferLen/4)
		Sleep(g_dwSleepTime);
	
	if (len>available)
		len=available;

	void *buffer1,*buffer2;
	DWORD buffer1Len,buffer2Len;
	result=lpDSBuffer->Lock(curWritePos,len ,&buffer1,&buffer1Len,&buffer2,&buffer2Len,NULL );
	if (result==DS_OK)
	{
		memcpy(buffer1,pBuf,buffer1Len);
		if (buffer2!=NULL)
			memcpy(buffer2,(char*)pBuf+buffer1Len,buffer2Len);
	}
	else if (result==DSERR_BUFFERLOST)
	{
		return -1;
	}
	else
	{
		return -1;
	}

	lpDSBuffer->Unlock(buffer1,buffer1Len,buffer2,buffer2Len);

	curWritePos+=buffer1Len+buffer2Len;
	if (curWritePos>g_dwMaxDSBufferLen)
	{
		curWritePos-=g_dwMaxDSBufferLen;
	}
	return buffer1Len+buffer2Len;
}