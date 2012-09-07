#include "stdafx.h"

#include "SpectrumAnalyser.h"
#include "FFT.h"
#include "BasicPlayer.h"
#include "PlayerThread.h"


#define WIDTH(rc) ((rc).right-(rc.left))
#define HEIGHT(rc) ((rc).bottom-(rc.top))

CSpectrumAnalyser::CSpectrumAnalyser(CBasicPlayer *pB_)
{
	pB=pB_;
	m_SampleSize=DEFAULT_SAMPLE_SIZE;
	//m_Left=new FLOAT[DEFAULT_SAMPLE_SIZE];
	//m_Right=new FLOAT[DEFAULT_SAMPLE_SIZE];

	m_floatSamples=new FLOAT[FFT_SAMPLE_SIZE];
	fft = new FFT(FFT_SAMPLE_SIZE);
}

CSpectrumAnalyser::CSpectrumAnalyser(void)
{
	m_SampleSize=DEFAULT_SAMPLE_SIZE;
	//m_Left=new FLOAT[DEFAULT_SAMPLE_SIZE];
	//m_Right=new FLOAT[DEFAULT_SAMPLE_SIZE];
	
	m_floatSamples=new FLOAT[FFT_SAMPLE_SIZE];
	fft = new FFT(FFT_SAMPLE_SIZE);

}

CSpectrumAnalyser::~CSpectrumAnalyser(void)
{
}


void CSpectrumAnalyser::ProcessSamples()
{
	int pos=pB->m_pPlayerThread->playPosInFFt;
	BYTE *pbuf=pB->m_pPlayerThread->pBufFFT1;

	for (int i=0;i<FFT_SAMPLE_SIZE;i++,pos+=4)
	{
		float left=(pbuf[pos+1] << 8) + pbuf[pos+0]/32767.0F;
		float right=(pbuf[pos+3] << 8) + pbuf[pos+2]/32767.0F;
		m_floatSamples[i] = (left+right)/2.0F;
	}


	float* lpFloatFFTData = fft->calculate(m_floatSamples, FFT_SAMPLE_SIZE);
	memcpy(m_floatMag, lpFloatFFTData, FFT_SAMPLE_SIZE/2);
}


void CSpectrumAnalyser::DrawSpectrum()
{
	RECT rect(m_rc);
	int cx=WIDTH(rect);
	int cy=HEIGHT(rect);

#define PI_2 6.283185F
#define PI   3.1415925F

	Rectangle(m_memDC, rect.left, rect.top, rect.right, rect.bottom);


	float c = 0;
	float floatFrrh = 1.0;
	float floatDecay = (float)0.05f;
	float floatSadFrr = (floatFrrh*floatDecay);
	int   bands=60;
	float floatBandWidth = ((float)(cx)/(float)bands);  //band宽度=总宽 / 数量
	float floatMultiplier = 2.0;


	RECT r;//下面长条块
	for(int a=0, band=0; band < bands; a+=(int)floatMultiplier, band++)
	{
		float wFs = 0;

		// -- Average out nearest bands.
		for (int b = 0; b < floatMultiplier; b++)
			wFs += m_floatMag[a + b];

		// -- Log filter.
		wFs = (wFs * (float) log((float)(band + 2)));

		if (wFs>0.005f && wFs <0.1f)
			wFs*=0.9f * PI;
		else if (wFs >0.01f && wFs <0.1f)
			wFs*=3.0f * PI;
		else if ( wFs > 0.1f && wFs < 0.5f)
			wFs*=PI;

		if (wFs > 1.0f) 
			wFs = 0.9f;

		// -- Compute SA decay...
		if (wFs >= (m_floatMag[a] - floatSadFrr)) 
		{
			m_floatMag[a] = wFs;
		} 
		else
		{
			m_floatMag[a] -= floatSadFrr;
			if (m_floatMag[a] < 0) 
				m_floatMag[a] = 0;
			wFs = m_floatMag[a];
		}

		drawSpectrumAnalyserBar(&rect,c,cy,floatBandWidth-1,wFs*cy,band);
		c += floatBandWidth;
	}

	::BitBlt(m_hdc,0,0,cx,cy,m_memDC,0,0,SRCCOPY);
}



void CSpectrumAnalyser::Excute()
{
	while(1)
	{
		ProcessSamples();
		::Sleep(200);
	}
	
}