#include "PlayerThread.h"
#include "Utils.h"
#include "MusicFile.h"
#include "forwardMsg.h"


CPlayerThread::CPlayerThread(MusicFile * pFile,CCriticalSection *cs,BOOL *pStop):
	m_pFile(pFile),
	m_cs(cs),
	CThread(TRUE),
	m_lpDSBuffer(NULL),
	m_dwCurWritePos(-1),m_bKeepPlaying(TRUE),
	pPosInfo(NULL),m_pStopped(pStop)
{
	pBufFFT1=new signed char[gDefaultBufferSize];
	//memset(pBufFFT1,0,gDefaultBufferSize);

	pPosInfo=new trackPosInfo;
}

CPlayerThread::~CPlayerThread(void)
{
	delete[] pBufFFT1;
	delete pPosInfo;
	
	if(m_lpDSBuffer)
	{	
		m_lpDSBuffer->Release();
		m_lpDSBuffer=NULL;
	}
}

BOOL WFXIsEqual(WAVEFORMATEX * a, WAVEFORMATEX * b)
{
	return  a->wBitsPerSample == b->wBitsPerSample &&
			a->nChannels == b->nChannels &&
			a->nSamplesPerSec == b->nSamplesPerSec ;
}

void CPlayerThread::Reset()
{
	WAVEFORMATEX *pwfx=m_pFile->GetFormat();
	m_iBytePerFrame=m_pFile->bytePerFrame();
	m_iTotalFrames= m_pFile->getFrameSize();
	m_wBitsPerSample=pwfx->wBitsPerSample;

	m_lpDSBuffer=DSoundBufferCreate(pwfx);

	fftBufLen=g_dwMaxDSBufferLen;
	//pBufFft=new signed char[fftBufLen];
	playPosInFFt=0;
	
	m_dwCurWritePos=0;

	gDefaultBufferSize = m_iBytePerFrame * 3 ;
}



BOOL CPlayerThread:: ReadFileReduceVol(BOOL bReduce)
{
	BOOL bFileEnd=FALSE;
	
	int  fileBufferLen= m_iBytePerFrame ;

	int times= bReduce ? 16 :  8 ;
	int   totalBufferLen=fileBufferLen * times;

	BYTE *fileBuffer=new BYTE[16*fileBufferLen];

	double vol=bReduce ? 1 : 0;
	
	double step=1/(double)times;
	int   bufferOffset=0;
	while(times--)
	{
		vol = bReduce ? vol - step : vol + step ;

		m_pFile->SetOutVolume(vol);

		if(!m_pFile->Read(fileBuffer + bufferOffset,fileBufferLen,&m_dwSizeRead) ||
			m_dwSizeRead==0 )
		{
			bFileEnd=TRUE;
			break;
		}
		else
		{
			bufferOffset+=m_dwSizeRead;
		}
		
	}

	DSoundBufferWrite(fileBuffer,totalBufferLen);

	m_pFile->SetOutVolume(bReduce ? 0 : 1);

	delete[] fileBuffer;

	return bFileEnd;
}




double CPlayerThread::GetOffsetSeconds()
{
	DWORD playCursor;
	if(m_lpDSBuffer==NULL)return 0;
	if (FAILED(m_lpDSBuffer->GetCurrentPosition(&playCursor,NULL))) return 0;
	DWORD notPlayed=DS_GetWritedNotPlayed(g_dwMaxDSBufferLen,playCursor,m_dwCurWritePos);

	WAVEFORMATEX *pwfx=m_pFile->GetFormat();
	
	double timePlayed=notPlayed/(double)pwfx->nAvgBytesPerSec;
	return timePlayed;
}

BOOL CPlayerThread::BeginChangeTrackPos()
{
	//Set our write pos to actually direct sound write cursor
	DWORD dwWrite;
	m_lpDSBuffer->GetCurrentPosition(NULL,&dwWrite);

	//And The File Cursor Too!
	int fileOffset=m_dwCurWritePos-dwWrite;
	if(fileOffset<0)
		fileOffset+=g_dwMaxDSBufferLen;

	int frameOffset=fileOffset /(double) m_iBytePerFrame;

	m_pFile->seek_frame(0-frameOffset ,SEEK_CUR);

	m_dwCurWritePos=dwWrite;
	//read file . send to direct sound buffer .
	ReadFileReduceVol(TRUE);

	return dwWrite;
}


BOOL CPlayerThread::EndChangeTrackPos()
{
	ReadFileReduceVol(FALSE);

	return TRUE;
}



BOOL CPlayerThread::CleanDSBuffer()
{
	BOOL bRet=TRUE;
	LPVOID buffer;
	DWORD   bufferLen;

	HRESULT result= m_lpDSBuffer->Lock(0,0,&buffer,&bufferLen,NULL,NULL,DSBLOCK_ENTIREBUFFER);
	if (result==DS_OK)
	{
		int emptyByte=( m_wBitsPerSample == 8)?128:0;
		memset(buffer, emptyByte, bufferLen);
		m_lpDSBuffer->Unlock(buffer,bufferLen,NULL,NULL);		
	}

	m_lpDSBuffer->SetCurrentPosition(0);
	
	return bRet;
}


void CPlayerThread::Excute()
{
	while(m_bKeepPlaying)
	{
		//if the direct sound buffer is about to over flow ,
		//wait some times.
		DWORD available;
		do 
		{
			DWORD playCursor;
			if (FAILED(m_lpDSBuffer->GetCurrentPosition(&playCursor,NULL))) return;
			available=DS_GetAvailable(g_dwMaxDSBufferLen,playCursor,m_dwCurWritePos);
		} while (Sleep2WaitReadCursor(available));
		
		if(m_bKeepPlaying)
			WriteDataToDSBuf();
	}

	CleanDSBuffer();
	
	delete this;
}

void CPlayerThread::WriteDataToDSBuf()
{
	m_cs->Enter();

	//Read data from file . 
	char *pFileBuffer= (char*)pBufFFT1;
	int nextFrame= m_pFile->tellframe();
	int bufferFrames= gDefaultBufferSize / m_iBytePerFrame;

	if(nextFrame + bufferFrames >= m_iTotalFrames)
	{
		if(ReadFileReduceVol())
			goto fileEnd;
		else
			goto NormalEnd;
	}

	if(!m_pFile->Read(pBufFFT1,gDefaultBufferSize,&m_dwSizeRead) ||
		m_dwSizeRead==0 )
	{
		goto fileEnd;
	}
	

	/*
	//send data to spectrum analysis  buffer
	if (fftBufLen-playPosInFFt<m_dwSizeRead)
	{
		memcpy(pBufFft+playPosInFFt,pBufFFT1,fftBufLen-playPosInFFt);
		memcpy(pBufFft,pBufFFT1+fftBufLen-playPosInFFt,m_dwSizeRead-(fftBufLen-playPosInFFt) );
		playPosInFFt+=m_dwSizeRead-fftBufLen;
	}
	else
	{
		memcpy(pBufFft+playPosInFFt,pBufFFT1,m_dwSizeRead);
		playPosInFFt+=m_dwSizeRead;
	}
	*/


	//send data to direct sound sys buffer
	DWORD written;
	m_dwSizeToWrite=m_dwSizeRead;
	while(m_dwSizeToWrite > 0)
	{
		written= DSoundBufferWrite(pFileBuffer, m_dwSizeToWrite);
		if (written==-1){
			//m_pPlayer->m_bStopped=TRUE;
			break;
		}

		m_dwSizeToWrite-=written;
		pFileBuffer=pFileBuffer+written;
	}

NormalEnd:
	m_cs->Leave();
	return;

fileEnd:
	m_lpDSBuffer->Stop();
	m_bKeepPlaying=FALSE;
	*m_pStopped=TRUE;
	m_pFile->Close();
	m_pFile=NULL;
	m_cs->Leave();	
	NotifyMsg(WM_TRACKSTOPPED);
	NotifyMsg(WM_TRACK_REACH_END);


	return;
}

DWORD CPlayerThread::DSoundBufferWrite(void* pBuf , int len)
{
	HRESULT result;
	LPVOID buffer1   ,buffer2;
	DWORD  buffer1Len,buffer2Len;
	result=m_lpDSBuffer->Lock(m_dwCurWritePos,len ,&buffer1,&buffer1Len,&buffer2,&buffer2Len,NULL );
	if (result==DS_OK){
		memcpy(buffer1,pBuf,buffer1Len);
		if (buffer2!=NULL)
			memcpy(buffer2,(char*)pBuf+buffer1Len,buffer2Len);
	}
	else if (result==DSERR_BUFFERLOST)
		return -1;
	else 
		return -1;


	m_lpDSBuffer->Unlock(buffer1,buffer1Len,buffer2,buffer2Len);
	m_dwCurWritePos+=buffer1Len+buffer2Len;
	if (m_dwCurWritePos>=g_dwMaxDSBufferLen)
		m_dwCurWritePos-=g_dwMaxDSBufferLen;
	return buffer1Len+buffer2Len;
}


