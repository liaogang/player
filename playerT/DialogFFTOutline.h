#include "SpectrumAnalyser.h"
#include "DialogFFT.h"

#pragma once
using namespace std;

class DialogFFTOutline : 
	public CDialogImpl<DialogFFTOutline>,
	public CDialogResize<DialogFFTOutline>
{
public:
	DialogFFTOutline():m_pDlgFFT(NULL),bFullScreen(FALSE),
		styleOld(0)
	{

	}

	enum { IDD = IDD_DLG_FFT_OUTLINE };

	BEGIN_MSG_MAP_EX(DialogFFTOutline)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(IDOK, OnCloseCmd)
		COMMAND_ID_HANDLER(IDCANCEL, OnCloseCmd)
		MESSAGE_HANDLER(WM_SIZE,OnSize)
	END_MSG_MAP()

	BEGIN_DLGRESIZE_MAP(DialogFFTOutline)
	END_DLGRESIZE_MAP()


	DialogFFT *m_pDlgFFT;
	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		DlgResize_Init(false,false);
		CenterWindow(GetParent());


		if (!m_pDlgFFT)
		{
			m_pDlgFFT=new DialogFFT;
			m_pDlgFFT->Create(m_hWnd,0,0,WS_CHILD|WS_VISIBLE);
			RECT rc;
			GetClientRect(&rc);
			m_pDlgFFT->SetWindowPos(0,rc.left,rc.top,rc.right-rc.left,rc.bottom-rc.top,SWP_NOZORDER);
			m_pDlgFFT->parent=this;
		}
		

		return TRUE;
	}

	
	LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
	{
		PAINTSTRUCT ps;
		::BeginPaint(m_hWnd,&ps);
		//CBasicPlayer::shared()->m_pSpectrumAnalyser->DrawSpectrum();

		::EndPaint(m_hWnd,&ps);
		return 0;
	}

	

	LRESULT OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		ShowWindow(SW_HIDE);	
		return 0;
	}

	void Show()
	{
		ShowWindow(SW_SHOW);
	}


	LRESULT OnSize(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
	{
		int _width=LOWORD(lParam);
		int _height=HIWORD(lParam);

		RECT rc;
		GetClientRect(&rc);
		m_pDlgFFT->SetWindowPos(0,rc.left,rc.top,rc.right-rc.left,rc.bottom-rc.top,SWP_NOZORDER);

		bHandled=FALSE;
		return 0;
	}



	BOOL bFullScreen;
	WINDOWPLACEMENT oldWpm;
	RECT fullScreecRC;
	UINT styleOld,styleBorder;
	LRESULT OnFullScreen()
	{
		if (!bFullScreen)
		{
			RECT windowRC;
			RECT clientRC;

			GetWindowPlacement(&oldWpm);

			GetWindowRect(&windowRC);	
			GetClientRect(&clientRC);
			ClientToScreen(&clientRC);


			int nFullWidth=GetSystemMetrics(SM_CXSCREEN);
			int nFullHeight=GetSystemMetrics(SM_CYSCREEN);


			fullScreecRC.left=windowRC.left-clientRC.left;
			fullScreecRC.top=windowRC.top-clientRC.top;
			fullScreecRC.right=nFullWidth+(windowRC.right-clientRC.right);
			fullScreecRC.bottom=nFullHeight+(windowRC.bottom-clientRC.bottom);

			WINDOWPLACEMENT wpm;
			wpm.length=sizeof(WINDOWPLACEMENT);
			wpm.flags=0;
			wpm.showCmd=SW_SHOWMAXIMIZED;
			wpm.rcNormalPosition=fullScreecRC;
			
			
			if (styleOld==0)
			{
				styleOld=GetWindowLong(GWL_STYLE);
				styleBorder=styleOld &~WS_BORDER;
			}

			SetWindowLong(GWL_STYLE,styleBorder);
			SetWindowPlacement(&wpm);
			SetWindowPos(HWND_TOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
			bFullScreen=TRUE;
		}
		else
		{
			SetWindowLong(GWL_STYLE,styleOld);
			ShowWindow(SW_HIDE);
			SetWindowPlacement(&oldWpm);
			SetWindowPos(HWND_NOTOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
			bFullScreen=FALSE;
		}
		return 0;	
	}
};
