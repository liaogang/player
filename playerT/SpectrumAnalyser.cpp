#include "stdafx.h"

#include "SpectrumAnalyser.h"
#include "FFT.h"
#include "BasicPlayer.h"
#include "PlayerThread.h"
#include "MusicFile.h"


class CTimer
{
private:
	static LONGLONG frequency;
	static int ready;
	static LONGLONG last;

	static void init()
	{
		LARGE_INTEGER liFrequency = {0};
		QueryPerformanceFrequency(&liFrequency);
		frequency = LONGLONG(liFrequency.QuadPart);
		ready = 1;
	}
public:
	static LARGE_INTEGER liCounter;
	static LONGLONG nanoTime()
	{
		if(ready != 1)
			init();

		QueryPerformanceCounter(&liCounter);
		return liCounter.QuadPart;
	}


	static DWORD TimeLasted()
	{
		LONGLONG cur=nanoTime();
		last=cur;
		return cur-last/frequency;
	}
};



CSpectrumAnalyser::CSpectrumAnalyser(CBasicPlayer *pB_)
{
	pB=pB_;
	m_SampleSize=DEFAULT_SAMPLE_SIZE;
	//m_Left=new FLOAT[DEFAULT_SAMPLE_SIZE];
	//m_Right=new FLOAT[DEFAULT_SAMPLE_SIZE];

	fft = new FFT(FFT_SAMPLE_SIZE);

	m_hpen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
	// Create a red brush.
	m_hbrush = CreateSolidBrush(RGB(0, 0, 0));
	m_hbrush1 = CreateSolidBrush(RGB(58, 110, 165));
	m_hbrush2=CreateSolidBrush(RGB(153,153,153));
	bands=30;
	m_iSpectrum_Delay=10;

	for(int i=0;i<90;i++)
	{
		intPeaks[i]=0;
		intPeaksDelay[i]=0;
	}

}

CSpectrumAnalyser::CSpectrumAnalyser(void)
{
	m_SampleSize=DEFAULT_SAMPLE_SIZE;
	//m_Left=new FLOAT[DEFAULT_SAMPLE_SIZE];
	//m_Right=new FLOAT[DEFAULT_SAMPLE_SIZE];
	
	
	fft = new FFT(FFT_SAMPLE_SIZE);

}

CSpectrumAnalyser::~CSpectrumAnalyser(void)
{
}



void CSpectrumAnalyser::Excute()
{
	while(1)
	{
		ProcessSamples();
		::InvalidateRect(m_hWnd,&m_rc,TRUE);
		::Sleep(35);
	}          
}

void CSpectrumAnalyser::ProcessSamples()
{
	if (pB->stoped())
		return;
	
	int pos;
	pB->m_pPlayerThread->m_lpDSBuffer->GetCurrentPosition((LPDWORD)&pos,NULL);
	pos+=200;          


	signed char *pbuf=pB->m_pPlayerThread->pBufFft;
	int Len=pB->m_pPlayerThread->fftBufLen;

	m_channelMode=static_cast<CHANNELMODE>(pB->m_pFile->GetFormat()->nChannels);
	m_sampleType=static_cast<SAMPLETYPE>(pB->m_pFile->GetFormat()->wBitsPerSample);
	INT sampleSize=FFT_SAMPLE_SIZE;

	if (m_channelMode==MONO && m_sampleType==EIGHT_BIT)
	{
		for (int i=0;i<sampleSize;i++,pos++)
		{
			if (pos>=Len)  pos-=Len;
			
			m_floatSamples[i] =(float) (pbuf[pos] << 7);
		}
	}
	else if (m_channelMode==STEREO && m_sampleType==EIGHT_BIT)
	{
		for (int i=0;i<sampleSize;i++,pos+=2)
		{	
			if (pos>=Len)  pos-=Len;
			m_floatSamples[i]=( (pbuf[pos] << 7)+ (pbuf[pos+1] << 7) )/2.0f;
		}
	}
	else if (m_channelMode==MONO && m_sampleType==SIXTEEN_BIT)
	{
		for (int i=0;i<sampleSize;i++,pos+=2)
		{
			if (pos>=Len)  pos-=Len;
			m_floatSamples[i]= (pbuf[pos+1] << 8) + pbuf[pos+1]/32767.0f;
		}
	}
	else if (m_channelMode==STEREO && m_sampleType==SIXTEEN_BIT)
	{
		for (int i=0;i<sampleSize;i++,pos+=4)
		{
			if (pos>=Len)  pos-=Len;
			//wave pcm 采用little_endian字节顺序
			float left=(float)((pbuf[pos+1] << 8) + pbuf[pos+0]) /32767.0F;
			float right=(float)((pbuf[pos+3] << 8) + pbuf[pos+2]) /32767.0F;
			m_floatSamples[i] = (left+right)/2.0F;
		}
	}

}


void CSpectrumAnalyser::DrawSpectrum()
{
	RECT rect(m_rc);
	int cx=WIDTH(rect);
	int cy=HEIGHT(rect);

	static HBRUSH Brush3dface=::CreateSolidBrush(RGB(255,255,255));
	FillRect(m_memDC,&rect,Brush3dface);

	float c = 0;
	float floatFrrh = 1.0;
	float floatDecay = (float)0.05f;
	float floatSadFrr = (floatFrrh*floatDecay);
	float floatBandWidth = ((float)(cx)/(float)bands);  //band宽度=总宽 / 数量
	float floatMultiplier = 2.0;

	float* lpFloatFFTData = fft ->calculate(m_floatSamples, FFT_SAMPLE_SIZE);
	memcpy(m_floatMag, lpFloatFFTData, FFT_SAMPLE_SIZE/2);



	//下面长条块
	for(int a=0, band=0; band < bands; a+=(int)floatMultiplier, band++)
	{
		float wFs = 0;

		// -- Average out nearest bands.
		for (int b = 0; b < floatMultiplier; b++)
			wFs += m_floatMag[a + b];

		// -- Log filter.
		wFs = (wFs * (float) log((float)(band + 2.0f)));

		if (wFs>0.005f && wFs <0.009f)
			wFs*=0.9f * PI;
		else if (wFs >0.01f && wFs <0.1f)
			wFs*=3.0f * PI;
		else if ( wFs > 0.1f && wFs < 0.5f)
			wFs*=PI;

		if (wFs > 1.0f)  wFs = 0.9f;

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




void CSpectrumAnalyser::drawSpectrumAnalyserBar(RECT *pRect,int x,int cy,int width,int height,int band)
{
	static HBRUSH brush= CreateSolidBrush(RGB(58, 110, 165));
	RECT barRect;
	barRect.left=x;
	barRect.right=x+width;
	barRect.top=pRect->bottom-height;
	barRect.bottom=pRect->bottom;

	//基部长条
	BitBlt(m_memDC, barRect.left,barRect.top , WIDTH(barRect), HEIGHT(barRect), m_hDCgrids, barRect.left, barRect.top, SRCCOPY);

	//小帽子滑块
	if(height > intPeaks[band])             //上涨
	{
		intPeaks[band] = height;                 //peak 另一半长度
		intPeaksDelay[band] = m_iSpectrum_Delay;       //delay  一次下降高度
		if (height>(pRect->bottom-pRect->top)/2)
			intPeaksDelay[band]+=3;
	}
	else                                               //滑块下降
	{
		intPeaksDelay[band]--;
		if (intPeaksDelay[band] < 0) 
			intPeaks[band]-=3;

		if (intPeaks[band] < 0) 
			intPeaks[band] = 0;
	}


	RECT peakRect(barRect);
	peakRect.top=pRect->bottom-intPeaks[band];
	peakRect.bottom=peakRect.top+BARHEIGHT;
	FillRect(m_memDC,&peakRect,m_hbrush2);
}



void CSpectrumAnalyser::Prepare()
{
	/* draw gradient */
	RECT rect (m_rc);
	DWORD colors[] = {RGB(255,0,0), RGB(0,255,0), RGB(10,10,10), RGB(0,0,255)};
	DrawGradient(m_hDCgrids, &rect, colors, 4, GRADIENT_FILL_RECT_V);


	/* draw grid */
	for(int i=HEIGHT(m_rc);i>=0;i--)
	{
		if(i % 2 == 0)
			continue;

		MoveToEx(m_hDCgrids, 0, i, NULL);
		LineTo(m_hDCgrids,WIDTH(m_rc), i);
	}
}






BOOL DrawGradient(HDC hdc, CONST RECT* pRect, CONST DWORD* cl, int Num, DWORD dwMode)
{
	int Width;
	int Height;
	TRIVERTEX *pvert;
	GRADIENT_RECT    *pgRect;

	if (cl == NULL || Num < 1 || pRect == NULL || dwMode == GRADIENT_FILL_TRIANGLE)
	{
		::SetLastError(ERROR_INVALID_PARAMETER);
		return TRUE;
	}

	if (Num == 1)
	{
		HBRUSH hBrush = CreateSolidBrush(cl[0]);
		SelectObject(hdc, hBrush);
		FillRect(hdc, pRect, hBrush);
		DeleteObject(hBrush);
		return TRUE;
	}

	pvert = new TRIVERTEX[Num * 2 - 2];
	pgRect = new GRADIENT_RECT[Num];

	Width = pRect->right - pRect->left;
	Height = pRect->bottom - pRect->top;
	for (int i = 0; i < Num - 1; i++)
	{
		if (dwMode == GRADIENT_FILL_RECT_V)
		{
			pvert[i * 2].x = pRect->left;
			pvert[i * 2].y = pRect->top + Height / (Num - 1) * i;

			pvert[i * 2 + 1].x = pRect->right;
			pvert[i * 2 + 1].y = pRect->top + Height / (Num - 1) * (i + 1);
		}
		else if (dwMode == GRADIENT_FILL_RECT_H)
		{
			pvert[i * 2].x = pRect->left + Width / (Num - 1) * i;
			pvert[i * 2].y = pRect->top;

			pvert[i * 2 + 1].x = pRect->left + Width / (Num - 1) * (i + 1);
			pvert[i * 2 + 1].y = pRect->bottom;
		}

		pvert[i * 2] .Red    = (WORD)GetRValue((cl[i])) << 8;
		pvert[i * 2] .Green  = (WORD)GetGValue((cl[i])) << 8;
		pvert[i * 2] .Blue   = (WORD)GetBValue((cl[i])) << 8;
		pvert[i * 2] .Alpha  = 0x0000;

		pvert[i * 2 + 1] .Red    = (WORD)GetRValue((cl[i + 1])) << 8;
		pvert[i * 2 + 1] .Green  = (WORD)GetGValue((cl[i + 1])) << 8;
		pvert[i * 2 + 1] .Blue   = (WORD)GetBValue((cl[i + 1])) << 8;
		pvert[i * 2 + 1] .Alpha  = 0x0000;

		pgRect[i].UpperLeft  = i * 2;
		pgRect[i].LowerRight = i * 2 + 1;
	}

	BOOL bRet = ::GradientFill(hdc, pvert, Num * 2, pgRect, Num - 1, dwMode);

	delete []pvert;
	delete []pgRect;

	return bRet;
}




