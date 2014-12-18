#pragma once


//-----------------------------------------

#define WIDTH(rc) ((rc).right-(rc.left))
#define HEIGHT(rc) ((rc).bottom-(rc.top))


#define PI_2 6.283185F
#define PI   3.1415925F

#define    HATHEIGHT    2
#define  BARHEIGHT   1

#define  DEFAULT_SAMPLE_SIZE  16000  //one time read from file
#define FFT_SAMPLE_SIZE 2048
//-----------------------------------------

BOOL DrawGradient(HDC hdc, CONST RECT* pRect, CONST DWORD* cl, int Num, DWORD dwMode);

class CBasicPlayer;
class FFT;
#include "Thread.h"
class CSpectrumAnalyser:public CThread
{
	enum CHANNELMODE{
		MONO=1,              //单声道
		STEREO               //立体声,即双声道
	};
	enum SAMPLETYPE
	{
		//高字节为实际数据,低字节为0-padding
		//PCM采样位数.2^8次方 , 1次2字节,可存入UINT
		EIGHT_BIT=8,         
		SIXTEEN_BIT=16          //2^16次方 ,  1次4字节 ,可存入int
	};

private:
	/* digital signal process */

	INT m_SampleSize;
	LONG m_FpsAsNS;
	LONG m_DesiredFpsAsNS;
	FLOAT m_floatSamples[FFT_SAMPLE_SIZE];
	INT m_position;
	INT m_offset;
	SAMPLETYPE m_sampleType;
	CHANNELMODE m_channelMode;

public:
	CBasicPlayer *pB;
	FFT* fft;
	float m_floatMag[DEFAULT_SAMPLE_SIZE/2];
	INT   m_floatMagDecay[DEFAULT_SAMPLE_SIZE/2];
	HDC m_memDC;

public:
	CSpectrumAnalyser(void);
	CSpectrumAnalyser(CBasicPlayer *pB_);

	
	~CSpectrumAnalyser(void);
	void ProcessSamples();
	void DrawSpectrum();
	void Excute(); 

	HDC m_hdc;
	RECT m_rc;
	HBITMAP m_hBmp;
	HWND    m_hWnd;
	void DCRECTInit( HWND _hwnd,HDC dc,RECT &rc)
	{
		m_hWnd=_hwnd;
		m_hdc=dc;
		m_rc=rc;

		m_memDC=::CreateCompatibleDC(m_hdc);
		m_hBmp=::CreateCompatibleBitmap(m_hdc,WIDTH(m_rc),HEIGHT(m_rc));
		::SelectObject(m_memDC,m_hBmp);

		m_hDCgrids=CreateCompatibleDC(m_hdc);
		HBITMAP tmp=::CreateCompatibleBitmap(m_hdc,WIDTH(m_rc),HEIGHT(m_rc));
		::SelectObject(m_hDCgrids,tmp);

		Prepare();
	}

	void SetSize(RECT &rc)
	{
	    m_rc=rc;
		Prepare();
	}
		
	void drawSpectrumAnalyserBar(RECT *pRect,int x,int cy,int width,int height,int band);


private:
	HPEN m_hpen,m_hpenOld;
	HBRUSH m_hbrush,m_hbrush1,m_hbrush2;
	HBRUSH m_hbrushOld,m_hbrushOld1,m_hbrushOld2;
private:
	int m_iSpectrum_Delay;
	INT   intPeaks[90];
	INT	  intPeaksDelay[90];
	INT   m_fOldwFs[90];
public:
	HDC m_hDCgrids;//网格背景

	void Prepare();


private:
	int   bands;
public:
	inline void SetBands(int b){bands=b;}
};
