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

	virtual bool  Open(LPCTSTR pszPath)
	{
		HRESULT r= m_wavefile.Open((LPTSTR)pszPath,m_pwfx,WAVEFILE_READ);
		m_pwfx=m_wavefile.GetFormat();
		return r==S_OK;
	}

	int Close()
	{
		return m_wavefile.Close();
	}
	
	virtual bool Read(void* pBuf,DWORD dwSizeToRead, DWORD* pdwSizeRead)
	{
		return S_OK==m_wavefile.Read((BYTE*)pBuf,dwSizeToRead,pdwSizeRead);
	}

	virtual VOID  SetOutVolume(double vol)
	{
		return;
	}

	virtual void  SetPos(int cur,int max)
	{
		//to do
	}

private:
	CWaveFile m_wavefile;
};
