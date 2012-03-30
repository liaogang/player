#pragma once
#include "mpg123.h"

class MusicFile
{
public:
	LPTSTR  m_pszPath;
protected:
	WAVEFORMATEX*  m_pwfx;
	mpg123_id3v1* m_pMpg123_id3v1;
	mpg123_id3v2* m_pMpg123_id3v2;
public:
	MusicFile(void){}

	~MusicFile(void){}


public:
	//mpg123_id3v1 getMpg123_id3v1() { return m_Mpg123_id3v1; }
	//mpg123_id3v2 getMpg123_id3v2() { return m_Mpg123_id3v2; }
public:
	//open and read the id3 tag info
	
	virtual bool Read(void* pBuf,DWORD dwSizeToRead, DWORD* pdwSizeRead)=0;
	virtual DWORD  GetSize()=0;
	virtual WAVEFORMATEX* GetFormat() { return m_pwfx; }
	virtual HRESULT   ResetFile()=0;
	virtual LONG      GetPos(double *e,double *f){return 0;};
	virtual INT  OpenAndReadID3Info(LPTSTR pszPath){return 0;};
protected:
	virtual bool Open(LPTSTR pszPath)=0;
	virtual int GetID3( mpg123_id3v1 **v1,mpg123_id3v2 **v2)=0;
};
