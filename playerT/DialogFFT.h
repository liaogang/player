#pragma once

#include "DsoundControl.h"
using namespace std;

class DialogFFT : public CDialogImpl<DialogFFT>
{
public:
	enum { IDD = IDD_DIALO_FFT };

	BEGIN_MSG_MAP(DialogFFT)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(IDOK, OnCloseCmd)
		COMMAND_ID_HANDLER(IDCANCEL, OnCloseCmd)
		MESSAGE_HANDLER( (WM_PAINT+913) , OnMyPaint)
		MESSAGE_HANDLER(WM_SIZE,OnSize)
	END_MSG_MAP()

	// Handler prototypes (uncomment arguments if needed):
	//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		CenterWindow(GetParent());

		dscrl->SetSpectrumRect(CRect(0,0,500,400));
		dscrl->SetFftEnvironment(this->m_hWnd);
		return TRUE;
	}

	LRESULT OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
	{
		int _width=LOWORD(lParam);
		int _height=HIWORD(lParam);
		dscrl->SetSpectrumRect(CRect(0,0,_width,_height));

		bHandled=FALSE;
		return 0;
	}

	LRESULT OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		//EndDialog(wID);
		ShowWindow(SW_HIDE);
		return 0;
	}

	DsoundControl *dscrl;
	LRESULT OnMyPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		if(!IsIconic())
			dscrl->DrawSpectrum();
		return 0;
	}

	
};
