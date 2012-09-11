#pragma once
using namespace std;
#include "SpectrumAnalyser.h"
class DialogFFT : public CDialogImpl<DialogFFT>
	,public CDialogResize<DialogFFT>
{
public:
	enum { IDD = IDD_DIALO_FFT };

	BEGIN_MSG_MAP(DialogFFT)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(IDOK, OnCloseCmd)
		COMMAND_ID_HANDLER(IDCANCEL, OnCloseCmd)
		MESSAGE_HANDLER(WM_SIZE,OnSize)
		MESSAGE_HANDLER(WM_PAINT,OnPaint)
		MESSAGE_HANDLER(WM_ERASEBKGND,OnErase)
	END_MSG_MAP()

	BEGIN_DLGRESIZE_MAP(DialogFFT)
	END_DLGRESIZE_MAP()

	// Handler prototypes (uncomment arguments if needed):
	//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	
	LRESULT OnErase(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		return 0;
	}
	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		DlgResize_Init(false,false);
		CenterWindow(GetParent());
		GetClientRect(&rc);

		//dscrl->SetSpectrumRect(CRect(0,0,500,400));
		//dscrl->SetFftEnvironment(this->m_hWnd);
		CBasicPlayer::shared()->m_pSpectrumAnalyser->DCRECTInit(m_hWnd,GetDC(),rc);
		CBasicPlayer::shared()->m_pSpectrumAnalyser->Init(FALSE);
		return TRUE;
	}

	
	LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
	{
		
		PAINTSTRUCT ps;
		::BeginPaint(m_hWnd,&ps);
		CBasicPlayer::shared()->m_pSpectrumAnalyser->DrawSpectrum();

		::EndPaint(m_hWnd,&ps);
		return 0;
	}

	RECT rc;
	LRESULT OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
	{
		int _width=LOWORD(lParam);
		int _height=HIWORD(lParam);

		rc.left=0;
		rc.right=0;
		rc.right=_width;
		rc.bottom=_height;

		CBasicPlayer::shared()->m_pSpectrumAnalyser->m_rc=rc;
		
		bHandled=FALSE;
		return 0;
	}

	LRESULT OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		CBasicPlayer::shared()->m_pSpectrumAnalyser->Suspend();
		ShowWindow(SW_HIDE);	
		return 0;
	}

	void Show()
	{
		ShowWindow(SW_SHOW);
		GetClientRect(&rc);
		CBasicPlayer::shared()->m_pSpectrumAnalyser->m_rc=rc;
		CBasicPlayer::shared()->m_pSpectrumAnalyser->Resume();
	}
};
