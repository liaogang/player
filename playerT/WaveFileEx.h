#pragma once
#include "musicfile.h"
#include "WaveFile.h"

class WaveFile :
	public MusicFile
{
public:
	WaveFile(void)
	{

	}
	~WaveFile(void)
	{
		m_wavefile.Close();
	}
public:
	virtual DWORD GetSize()
	{
		return m_wavefile.GetSize();
	}

	virtual HRESULT ResetFile()
	{
		return m_wavefile.ResetFile();
	}

	virtual bool  Open(LPTSTR pszPath)
	{
		m_wavefile.Open(pszPath,m_pwfx,WAVEFILE_READ);
		m_pwfx=m_wavefile.GetFormat();
		return 0;
	}

	virtual bool Read(void* pBuf,DWORD dwSizeToRead, DWORD* pdwSizeRead)
	{
		m_wavefile.Read((BYTE*)pBuf,dwSizeToRead,pdwSizeRead);
		return 0;
	}

	virtual INT  OpenAndReadID3Info(LPTSTR pszPath)
	{
		int ret=0;
		ret=Open(pszPath);
		mpg123_id3v1 *v1=&m_Mpg123_id3v1;
		mpg123_id3v2 *v2=&m_Mpg123_id3v2;
		GetID3(&v1,&v2);
		return ret;
	}

	//MPG123_OK or MPG123_ERR
	int GetID3(mpg123_id3v1 **v1,mpg123_id3v2 **v2)
	{
		//wait to full

		return -1;
	}


private:
	CWaveFile m_wavefile;
};
