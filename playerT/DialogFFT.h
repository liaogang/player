
class DialogFFTOutline;

using namespace std;

#pragma once
class DialogFFT : public CWindowImpl<DialogFFT>
{
public:
	HMENU menu,trackMenu;
	DialogFFT():bFullScreen(FALSE),parent(NULL)
	{
		menu=::LoadMenu(NULL,MAKEINTRESOURCE (IDR_MENU_SPECTRUM) );
		trackMenu=::GetSubMenu(menu,0);
	}

	~DialogFFT()
	{
		::DestroyMenu(menu);
	}

public:
	BEGIN_MSG_MAP_EX(DialogFFT)
		MESSAGE_HANDLER(WM_CREATE,OnCreate)
		//MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(IDOK, OnCloseCmd)
		COMMAND_ID_HANDLER(IDCANCEL, OnCloseCmd)
		MESSAGE_HANDLER(WM_SIZE,OnSize)
		MESSAGE_HANDLER(WM_PAINT,OnPaint)
		MESSAGE_HANDLER(WM_ERASEBKGND,OnErase)
		MSG_WM_RBUTTONUP(OnRButtonUp)

		COMMAND_ID_HANDLER(ID_MENU_SPE_FULLSCREEN,OnFullScreen)
		COMMAND_ID_HANDLER(ID_BANDS_10,OnBands10)
		COMMAND_ID_HANDLER(ID_BANDS_20,OnBands20)
		COMMAND_ID_HANDLER(ID_BANDS_40,OnBands40)
		COMMAND_ID_HANDLER(ID_BANDS_80,OnBands80)
		//COMMAND_ID_HANDLER(ID_BANDS_160,OnBands160)
	END_MSG_MAP()



	// Handler prototypes (uncomment arguments if needed):
	//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)
	
	BOOL bFullScreen;
	WINDOWPLACEMENT oldWpm;
	RECT fullScreecRC;
	DialogFFTOutline *parent;
	LRESULT OnFullScreen(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);


	LRESULT OnBands10(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		CBasicPlayer::shared()->m_pSpectrumAnalyser->SetBands(10);
		return 0;	
	}

	LRESULT OnBands20(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		CBasicPlayer::shared()->m_pSpectrumAnalyser->SetBands(20);
		return 0;	
	}

	LRESULT OnBands40(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		CBasicPlayer::shared()->m_pSpectrumAnalyser->SetBands(40);
		return 0;	
	}

	LRESULT OnBands80(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		CBasicPlayer::shared()->m_pSpectrumAnalyser->SetBands(80);
		return 0;	
	}

	LRESULT OnBands160(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		CBasicPlayer::shared()->m_pSpectrumAnalyser->SetBands(160);
		return 0;	
	}
	LRESULT OnErase(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		return 0;
	}

	void OnRButtonUp(UINT nFlags, CPoint point)
	{
		::ClientToScreen(m_hWnd,&point);
		::TrackPopupMenu(trackMenu,TPM_LEFTALIGN,point.x,point.y,0,m_hWnd,0);
	}

	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		//DlgResize_Init(false,false);
		//CenterWindow(GetParent());
		GetClientRect(&rc);

		//dscrl->SetSpectrumRect(CRect(0,0,500,400));
		//dscrl->SetFftEnvironment(this->m_hWnd);
		HDC hdc=GetDC();
		CBasicPlayer::shared()->m_pSpectrumAnalyser->DCRECTInit(m_hWnd,hdc,rc);
		CBasicPlayer::shared()->m_pSpectrumAnalyser->Init(FALSE);
		ReleaseDC(hdc);
		bHandled=FALSE;
		return 0;
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
		HDC hdc=GetDC();
		//CBasicPlayer::shared()->m_pSpectrumAnalyser->m_rc=rc;
		CBasicPlayer::shared()->m_pSpectrumAnalyser->DCRECTInit(m_hWnd,hdc,rc);
		CBasicPlayer::shared()->m_pSpectrumAnalyser->Init(FALSE);
		ReleaseDC(hdc);
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
