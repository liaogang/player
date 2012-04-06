#pragma once
#include "mmsystem.h"
#include "mpg123.h"

#pragma comment(lib, "winmm.lib")

#ifdef _DEBUG
#pragma comment(lib, "mpg123_dencd.lib")
#else
#pragma comment(lib, "mpg123_denc.lib")
#endif


#define PLAYERMANAGER_RESULT_OK       MPG123_OK
#define PLAYERMANAGER_RESULT_ERROR       -1

class Mp3File:public MusicFile
{
private:
	///////////////////////////////////
	//function ptr
	MPG123_INIT  m_mpg123_init;
	MPG123_NEW   m_mpg123_new;
	MPG123_EXIT  m_mpg123_exit;
	MPG123_DELETE m_mpg123_delete;
	MPG123_GETFORMAT m_mpg123_getformat;
	typedef int (*MPG123_TOPEN)(mpg123_handle *, const _TCHAR *);
	MPG123_TOPEN  m_mpg123_open;
	MPG123_READ  m_mpg123_read;
	typedef int(*MPG123_TCLOSE)(mpg123_handle *);
	MPG123_TCLOSE m_mpg123_close;

private:
	mpg123_handle* m_hmp3;
	//-----------------------------------------------------------------------
public:
	Mp3File(void):m_hmp3(NULL)
	{

	}

	~Mp3File(void)
	{
		if (m_hmp3)
		{
			m_mpg123_close(m_hmp3);
			//mpg123_exit();
		}
	}

	virtual DWORD GetSize()
	{
		return mpg123_length(m_hmp3);
	}

	virtual HRESULT ResetFile()
	{
		if (m_hmp3)
		{
			mpg123_seek(m_hmp3,0,SEEK_SET);
			return 0;
		}
		else
			return -1;
	}

	virtual bool Open(LPTSTR pszPath)
	{
		m_pszPath=pszPath;

		if (InitMpgLib()!=MPG123_OK)
			return false;

		if (m_mpg123_open(m_hmp3, pszPath) != MPG123_OK)
			return FALSE;
		
		int   channels, encode;
		unsigned short bitspersample, format;
		long  rate; 	
		if (m_mpg123_getformat(m_hmp3, &rate, &channels, &encode) != MPG123_OK)
		{
			m_mpg123_close(m_hmp3);
			return FALSE;
		}

		/////////////////////////////////////////////////
		if(encode & MPG123_ENC_FLOAT_64)
		{
			bitspersample = 64;
#define WAVE_FORMAT_IEEE_FLOAT 0x0003
			format = WAVE_FORMAT_IEEE_FLOAT;
		}
		else if(encode & MPG123_ENC_FLOAT_32)
		{
			bitspersample = 32;
			format = WAVE_FORMAT_IEEE_FLOAT;
		}
		else if(encode & MPG123_ENC_16)
		{
			bitspersample = 16;
			format = WAVE_FORMAT_PCM;
		}
		else
		{
			bitspersample = 8;
			format = WAVE_FORMAT_PCM;
		}

		//////////////////////////////////////////////////
		//m_pwfx=new WAVEFORMATEX;
		m_pwfx = (WAVEFORMATEX*)new CHAR[ sizeof(WAVEFORMATEX) ];
		m_pwfx->wFormatTag		=	format;
		m_pwfx->nChannels		=	channels;
		m_pwfx->nSamplesPerSec	=   rate;
		m_pwfx->nAvgBytesPerSec  =   rate * channels * bitspersample/8;
		m_pwfx->nBlockAlign	    =   channels * bitspersample/8;
		m_pwfx->wBitsPerSample	=   bitspersample;
		m_pwfx->cbSize			=   0;
		return TRUE;
	}

	virtual bool Read(void* pBuf,DWORD dwSizeToRead, DWORD* pdwSizeRead)
	{
		size_t done; 	int ret;
		ret = m_mpg123_read(m_hmp3,(unsigned char*)pBuf,dwSizeToRead, &done );
		if (ret == MPG123_ERR)
		{
			return false;
		}
		else if (ret == MPG123_DONE)
		{
			return false;
		}
		else
		{
			*pdwSizeRead=done;
			return true;
		}

	}
private:

	int InitMpgLib()
	{
		m_mpg123_init      = mpg123_init;
		m_mpg123_new       = mpg123_new;
		m_mpg123_delete    = mpg123_delete;
		m_mpg123_open      = mpg123_topen;
		m_mpg123_read      = mpg123_read;
		m_mpg123_close     = mpg123_tclose;
		m_mpg123_exit      = mpg123_exit;   
		m_mpg123_getformat = mpg123_getformat;


		if (m_mpg123_init() != MPG123_OK)
		{
			return PLAYERMANAGER_RESULT_ERROR;
		}

		int error=0;
		if ((m_hmp3 = m_mpg123_new(NULL,&error)) == NULL)
		{
			LPCSTR errorCode=mpg123_plain_strerror(error);
			return PLAYERMANAGER_RESULT_ERROR;
		}
	
		mpg123_param(m_hmp3, MPG123_RESYNC_LIMIT, -1, 0); /* New in library version 0.0.1 . */

		return PLAYERMANAGER_RESULT_OK;
	}

	LONG  GetPos(double *e,double *f)
	{
		off_t a=0,b=0,c,d;
		int ret=mpg123_position(m_hmp3,a,b,&c,&d,e,f);
		return ret;
	}

	//MPG123_OK or MPG123_ERR
	int GetID3(mpg123_id3v1 **v1,mpg123_id3v2 **v2)
	{
		return mpg123_id3(m_hmp3,v1,v2);
	}


	virtual INT  OpenAndReadID3Info(LPTSTR pszPath)
	{
		int ret=0;
		ret=Open(pszPath);
		GetID3(&m_pMpg123_id3v1,&m_pMpg123_id3v2);
		return ret;
	}
};
