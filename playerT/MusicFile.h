#pragma once
#include "mpg123.h"

class MusicFile
{
public:
	LPTSTR  m_pszPath;
protected:
	WAVEFORMATEX*  m_pwfx;
public:
	MusicFile(void):m_pwfx(NULL){}
	~MusicFile(void){ if (m_pwfx) delete m_pwfx; }
public:
	virtual bool Read(void* pBuf,DWORD dwSizeToRead, DWORD* pdwSizeRead)=0;
	virtual DWORD  GetSize()=0;
	virtual WAVEFORMATEX* GetFormat() { return m_pwfx; }
	virtual HRESULT   ResetFile()=0;
	virtual LONG      GetPos(double *e,double *f){return 0;};
	virtual VOID  SetOutVolume(double vol)=0;
	virtual bool Open(LPCTSTR pszPath)=0;
	virtual void  SetPos(int cur,int max)=0;
	virtual int  Close()=0; 
};
