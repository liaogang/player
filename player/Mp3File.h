#pragma once
#include "mmsystem.h"
#include <mpg123.h>

#pragma comment(lib, "winmm.lib")


#ifdef _DEBUG
#pragma comment(lib, "libmpg123d.lib")
#else
#pragma comment(lib, "libmpg123.lib")
#endif


#define PLAYERMANAGER_RESULT_OK       MPG123_OK
#define PLAYERMANAGER_RESULT_ERROR       -1

class Mp3File :public MusicFile
{
private:
	mpg123_handle* m_hmp3;
	//-----------------------------------------------------------------------
public:
	Mp3File(void):m_hmp3(NULL)
	{

	}

	virtual ~Mp3File(void)
	{
		if (m_hmp3)
		{
			mpg123_tclose(m_hmp3);
			mpg123_delete(m_hmp3);
			m_hmp3=NULL;
		}
	}

	int Close()
	{
		if (m_hmp3)
		{
			mpg123_tclose(m_hmp3);
			mpg123_delete(m_hmp3);
			m_hmp3=NULL;
		}

		return 0;
	}


	//total frame size
	virtual DWORD getFrameSize()
	{
		return mpg123_length(m_hmp3);
	}


	//tell the next frame will read
	off_t tellframe()
	{
		return mpg123_tellframe(m_hmp3);
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

	void GetFrameInfo(mpg123_frameinfo * pInfo)
	{
		mpg123_info(m_hmp3,pInfo);
	}

	int samplesPerFrame()
	{
		return mpg123_spf(m_hmp3);
	}

	int bytePerFrame()
	{
		mpg123_frameinfo info;
		GetFrameInfo(&info);
		info.framesize;
		
		int channels=2;
		if(info.mode == MPG123_M_MONO )
			channels=1;

		WAVEFORMATEX* format=GetFormat();

		return format->wBitsPerSample/8 * samplesPerFrame() * channels ;
	}

	virtual void  SetPos(int cur,int max)
	{
		long i=cur/(float)max * getFrameSize();
		mpg123_seek(m_hmp3,i,SEEK_SET);
	}



	void seek_sample(off_t offset , int whence)
	{
		//SEEK_SET;
		//SEEK_CUR;
		//SEEK_END;

		int err=mpg123_seek(m_hmp3,offset,whence);
		err=0;
	}


	void seek_frame(off_t offset , int whence)
	{

		int err=mpg123_seek_frame(m_hmp3,offset,whence);
		err=0;
	}


	virtual bool Open(LPCTSTR pszPath)
	{
		m_pszPath=(TCHAR*)pszPath;

		if (InitMpgLib()!=MPG123_OK)
			return false;

 		if (mpg123_topen(m_hmp3, pszPath) != MPG123_OK)
			return FALSE;
		
		int   channels, encode;
		unsigned short bitspersample, format;
		long  rate; 	
		if (mpg123_getformat(m_hmp3, &rate, &channels, &encode) != MPG123_OK)
		{
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
		ret = mpg123_read(m_hmp3,(unsigned char*)pBuf,dwSizeToRead, &done );
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
		if (mpg123_init() != MPG123_OK)
		{
			return PLAYERMANAGER_RESULT_ERROR;
		}

		int error=0;
		if ((m_hmp3 = mpg123_new(NULL,&error)) == NULL)
		{
			LPCSTR errorCode=mpg123_plain_strerror(error);
			return PLAYERMANAGER_RESULT_ERROR;
		}

		return PLAYERMANAGER_RESULT_OK;
	}

	//get the time cur/left
	LONG  GetPos(double *e,double *f)
	{
		off_t a=0,b=0,c,d;
		int ret=mpg123_position(m_hmp3,a,b,&c,&d,e,f);
		return ret;
	}

	//MPG123_OK or MPG123_ERR
// 	int GetID3(mpg123_id3v1 **v1,mpg123_id3v2 **v2)
// 	{
// 		return mpg123_id3(m_hmp3,v1,v2);
// 	}


// 	virtual INT  OpenAndReadID3Info(LPCTSTR pszPath)
// 	{
// 		int ret=0;
// 		ret=Open(pszPath);
// 		GetID3(&m_pMpg123_id3v1,&m_pMpg123_id3v2);
// 		return ret;
// 	}


	VOID  SetOutVolume(double vol)
	{
		mpg123_volume(m_hmp3,vol);
	}
};
