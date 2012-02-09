#pragma once
#include "WaveFile.h"
#include "FFT.h"
#include "MusicFile.h"
#define FFT_SAMPLE_SIZE 2048

//-------------------------------------------------------------
//use this class
//1.SetShowWindow
//2.OpenFile
//-------------------------------------------------------------
#define FFT_SIZE			512

enum PlayStatus
{
	stopped=0,
	paused,
	playing
};

class CMainFrame;
class DsoundControl
{
public:
	DsoundControl(void);
public:
	~DsoundControl(void);
public:
	CMainFrame* m_pMainFrame;
	DWORD m_dwCircles1,m_dwCircles2;
	WAVEFORMATEX* m_pwfx;
	HANDLE m_pHEvent[2];
	LPDIRECTSOUND m_ppDS;
	LPDIRECTSOUNDBUFFER m_dsBuffer2;
	LPBYTE m_pAudioBuf;
	DWORD m_dwSecBufLen;

	MusicFile* m_pfile;
	MMRESULT m_timerID;
	MMRESULT m_timerFFT;

	float m_floatSamples[FFT_SAMPLE_SIZE];
	float m_floatMag[FFT_SAMPLE_SIZE/2];
private:

	HWND *m_pWndShow;

	DWORD m_dwTotalSamples;   //wave file's total sample
	LPBYTE m_pFFTBuffer;    //buffer ,a copy of sound buffer,for fft
public:
	static DsoundControl* shared();
public:
	void SetShowWindow(HWND *pWndShow){	m_pWndShow=pWndShow;}
	void OpenFile(LPTSTR filepath);
	void Play();
	void Pause();
	void Stop();
	void  TimerNotify();
	BOOL InitDSound();
	HRESULT GetAudioSampleDatas(LPBYTE lpDesBuf,const DWORD dwRequiredSamples,DWORD &dwRetSamples);
	void ConvertDataToFFT();
	DWORD GetSamplesPlayed(DWORD* pCurPlayPos/*取得播放游标的值*/);
	void DrawSpectrum();
public:
	PlayStatus m_bStatus;
	PlayStatus IsPlaying(){return m_bStatus;}

public:
	int m_iSpectrum_Decay;
	int m_iSpectrum_Bands;
	int m_iSpectrum_Delay;
	CRect m_Spectrum_Rect;

	HDC m_memDC,m_hdc;
	HBITMAP m_bitmap;

	void SetSpectrumRect(CRect rc);

	void SetFftEnvironment( HWND hwnd)
	{
		HDC hdc=GetDC(hwnd);
		//if (m_memDC)
		{
			//ReleaseDC(hwnd,m_memDC);
		//	m_memDC=NULL;
		}
		//if (m_bitmap!=NULL)
		{
			//DeleteObject(m_bitmap);
			//m_bitmap=NULL;
		}
		m_memDC=::CreateCompatibleDC(hdc);

		m_hdc=hdc;
	}
};

