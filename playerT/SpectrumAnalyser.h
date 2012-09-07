#pragma once


#define  DEFAULT_SAMPLE_SIZE  16000  //one time read from file
#define FFT_SAMPLE_SIZE 2048
class CBasicPlayer;
class FFT;
#include "Thread.h"
class CSpectrumAnalyser:public CThread
{
private:
	/* digital signal process */

	INT m_SampleSize;
	LONG m_FpsAsNS;
	LONG m_DesiredFpsAsNS;
	FLOAT * m_floatSamples;
	INT m_position;
	INT m_offset;
	INT m_sampleType;
	INT m_channelMode;
public:
	CBasicPlayer *pB;
	FFT* fft;
	float m_floatMag[DEFAULT_SAMPLE_SIZE/2];
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
	void DCRECTInit(HDC dc,RECT &rc)
	{
		m_hdc=dc;
		m_rc=rc;
		m_memDC=::CreateCompatibleDC(m_hdc);
	}

	void drawSpectrumAnalyserBar(RECT *pRect,int x,int y,int width,int height,int band)
	{
		static HBRUSH brush= CreateSolidBrush(RGB(58, 110, 165));
		RECT barRect;
		barRect.left=x;
		barRect.right=x+width;
		barRect.top=pRect->top+height;
		barRect.bottom=pRect->bottom;

		FillRect(m_memDC, &barRect, brush);



		// 	//上面的小帽子滑块
		// 	if(height > intPeaks[band])             //上涨
		// 	{
		// 		//intLastBarHeight[band] = height;
		// 		intPeaks[band] = height;                 //peak 另一半长度
		// 		intPeaksDelay[band] = m_iSpectrum_Delay;       //delay  一次下降高度
		// 	}
		// 	else                                               //滑块下降
		// 	{
		// 		intPeaksDelay[band]--;
		// 		if (intPeaksDelay[band] < 0) 
		// 			intPeaks[band]-=3;;
		// 
		// 		if (intPeaks[band] < 0) 
		// 			intPeaks[band] = 0;
		// 	}
		// 
		// 	RECT peakRect;
		// 	peakRect.left=x;
		// 	peakRect.right=peakRect.left+width;
		// 	peakRect.top=y-intPeaks[band];
		// 	peakRect.bottom=peakRect.top+BARHEIGHT;
		// 	FillRect(m_memDC,&peakRect,m_hbrush2);

		// 	r.top -= intPeaks[band];
		// 
		// 	if(r.top < rect.top)
		// 		r.top = rect.top + 2;
		// 
		// 	if(r.top >= rect.bottom)
		// 		r.top = rect.bottom - 2;
		// 
		// 	r.bottom = r.top + HATHEIGHT;
		// 	FillRect(m_memDC, &r, m_hbrush2);

	}

};
