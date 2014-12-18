#pragma once
#include "mpg123.h"
#include "customMsg.h"
class MusicFile
{
public:
	LPTSTR  m_pszPath;
protected:
	WAVEFORMATEX*  m_pwfx;
public:
	MusicFile(void):m_pwfx(NULL){}
	virtual ~MusicFile(void){ if (m_pwfx) delete m_pwfx; }
public:
	virtual bool Read(void* pBuf,DWORD dwSizeToRead, DWORD* pdwSizeRead)=0;
	virtual WAVEFORMATEX* GetFormat() { return m_pwfx; }
	virtual HRESULT   ResetFile()=0;
	virtual LONG      GetPos(double *e,double *f){return 0;};
	virtual VOID  SetOutVolume(double vol)=0;
	virtual bool Open(LPCTSTR pszPath)=0;
	virtual void  SetPos(int cur,int max)=0;
	virtual int  Close()=0; 
	virtual void  seek_sample(off_t offset , int whence){};
	virtual void  seek_frame(off_t offset , int whence){};

	virtual void GetFrameInfo(mpg123_frameinfo * pInfo)
	{
		ATLASSERT(FALSE);
	}

	virtual int samplesPerFrame()
	{
		return -1;
	}


	virtual DWORD getFrameSize()
	{
		return -1;
	}


	//tell the next frame will read
	virtual off_t tellframe()
	{
		return -1;
	}

	virtual int bytePerFrame(){return -1;}
};
