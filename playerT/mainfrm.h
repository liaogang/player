// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once
#include "stdafx.h"
#include "resource.h"
#include "AboutDlg.h"
//#include "DsoundControl.h"
#include "DialogConfig.h"
#include "CMyView.h"
#include "BasicPlayer.h"
static	CBasicPlayer *g_pSharedPlayer;
class CMyTrackBar :public CWindowImpl<CMyTrackBar,CTrackBarCtrl>
{
public:
	DECLARE_WND_SUPERCLASS(NULL,CTrackBarCtrl::GetWndClassName())

	BEGIN_MSG_MAP(CMyTrackBar)
		MESSAGE_HANDLER(TB_BUTTONCOUNT,TBB)
		MESSAGE_HANDLER(TB_GETITEMRECT,OnGetItemRect)
		MESSAGE_HANDLER(WM_USER+22,OnPos)
	END_MSG_MAP()

	LRESULT OnGetItemRect(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
	{
		RECT *rc=(RECT*)lParam;
		rc->left=0;
		rc->right=30;
		rc->bottom=30;
		rc->top=0;
		return 1;
	}

	LRESULT TBB(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		bHandled=TRUE;
		return 1;
	}

	LRESULT OnPos(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		double used,lefted;
		used=wParam;
		lefted=lParam;
		bHandled=FALSE;
		SetRange(0,used+lefted);
		SetPos(used);
		return 0;
	}

	//LRESULT InitialUpdate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	//{
	//	SetLineSize(1);
	//	SetPageSize(1);
	//	//mpg123_seek()

	//	bHandled=FALSE;
	//}
};

//#define WM_USER+22 
class CMainFrame : public CFrameWindowImpl<CMainFrame>, public CUpdateUI<CMainFrame>,
		public CMessageFilter, public CIdleHandler
{
public:
	DECLARE_FRAME_WND_CLASS(NULL, IDR_MAINFRAME)

	CCommandBarCtrl m_CmdBar;
	CDialogConfig   m_dlgConfig;

	virtual BOOL PreTranslateMessage(MSG* pMsg)
	{
		return CFrameWindowImpl<CMainFrame>::PreTranslateMessage(pMsg);
	}

	virtual BOOL OnIdle()
	{
		UIUpdateToolBar();
		return FALSE;
	}

	BEGIN_UPDATE_UI_MAP(CMainFrame)
		UPDATE_ELEMENT(ID_VIEW_TOOLBAR, UPDUI_MENUPOPUP)
		UPDATE_ELEMENT(ID_VIEW_STATUS_BAR, UPDUI_MENUPOPUP)
	END_UPDATE_UI_MAP()

	BEGIN_MSG_MAP(CMainFrame)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_PAINT+913,OnDrawSpectrum)
		MESSAGE_HANDLER(WM_SIZE,OnSize)
		MESSAGE_HANDLER(WM_USER+22,OnPos)
		COMMAND_ID_HANDLER(ID_APP_EXIT, OnFileExit)
		COMMAND_ID_HANDLER(ID_FILE_NEW, OnFileNew)
		COMMAND_ID_HANDLER(ID_VIEW_TOOLBAR, OnViewToolBar)
		COMMAND_ID_HANDLER(ID_VIEW_STATUS_BAR, OnViewStatusBar)
		COMMAND_ID_HANDLER(ID_APP_ABOUT, OnAppAbout)

		
		COMMAND_ID_HANDLER(ID_SHOWLYRIC,OnShowLyric)
		COMMAND_ID_HANDLER(ID_MENU_FFTTEST,OnFftDialog)
		COMMAND_ID_HANDLER(ID_PLAY, OnPlay)
		COMMAND_ID_HANDLER(ID_OPEN, OnOpen)
		COMMAND_ID_HANDLER(ID_PAUSE, OnPause)
		COMMAND_ID_HANDLER(ID_STOP, OnStop)
		COMMAND_ID_HANDLER(ID_CONFIG, OnConfig)
		COMMAND_ID_HANDLER(ID_TEST_INITPLAYLIST, OnTestInitplaylist)
		COMMAND_ID_HANDLER(ID_FILE_OPENDIRECTORY, OnFileOpendirectory)
		CHAIN_MSG_MAP(CUpdateUI<CMainFrame>)
		CHAIN_MSG_MAP(CFrameWindowImpl<CMainFrame>)
	END_MSG_MAP()

	LRESULT OnPos(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		double used,lefted;
		used=wParam;
		lefted=lParam;
		bHandled=FALSE;
		m_trackBar.SetRange(0,used+lefted);
		m_trackBar.SetPos(used);
		return 0;
	}
// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)
	CMyView m_view;
	CMyTrackBar m_trackBar;
	

	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		m_hWndClient = m_view.Create(m_hWnd, rcDefault, NULL, LVS_SINGLESEL | WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | LVS_REPORT | LVS_SHOWSELALWAYS, WS_EX_CLIENTEDGE);
		m_view.m_pMainFrame=this;

		// create command bar window
		HWND hWndCmdBar = m_CmdBar.Create(m_hWnd, rcDefault, NULL, ATL_SIMPLE_CMDBAR_PANE_STYLE);
		// attach menu
		CMenu *pMenu=new CMenu;
		pMenu->LoadMenu(IDR_MAINFRAME);
		m_CmdBar.AttachMenu(pMenu->m_hMenu);
		// load command bar images
		m_CmdBar.LoadImages(IDR_MAINFRAME);
		// remove old menu
		SetMenu(NULL);

		HWND hWndToolBar = CreateSimpleToolBarCtrl(m_hWnd,IDR_MAINFRAME, FALSE, ATL_SIMPLE_TOOLBAR_PANE_STYLE);


		CreateSimpleReBar(ATL_SIMPLE_REBAR_NOBORDER_STYLE);
		AddSimpleReBarBand(hWndCmdBar);
		AddSimpleReBarBand(hWndToolBar, NULL, TRUE,280,TRUE);


		//------------------------------------------------
		UINT style=WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
		style|= TBS_TOOLTIPS  |TBS_NOTICKS    ;
		//style=style|~TBS_NOTICKS;
		HWND hWndProgressBar=m_trackBar.Create(this->m_hWnd,NULL,NULL,style,0);
		m_trackBar.SetPageSize(1);
		m_trackBar.SetLineSize(1);
		m_trackBar.SetThumbLength(30);
		AddSimpleReBarBand(hWndProgressBar,NULL,FALSE,150,TRUE);
		//-------------------------------------------------

		CreateSimpleStatusBar();

		UIAddToolBar(hWndToolBar);
		UISetCheck(ID_VIEW_TOOLBAR, 1);
		UISetCheck(ID_VIEW_STATUS_BAR, 1);

		// register object for message filtering and idle updates
		CMessageLoop* pLoop = _Module.GetMessageLoop();
		ATLASSERT(pLoop != NULL);
		pLoop->AddMessageFilter(this);
		pLoop->AddIdleHandler(this);



		//--------------------------------------------------
		tstring columnName[]={_T("                    title                      "),_T(" artist "),_T(" album "),_T(" year "),_T(" comment "),_T(" genre ")};
		for (int i=0;i<6;i++)
		{
			tstring str=columnName[i];
			m_view.AddColumn(str.c_str(),i,-1, LVCF_FMT| LVCF_WIDTH|LVCF_TEXT|LVCF_SUBITEM ,LVCFMT_CENTER);
		}

		return 0;
	}

	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		// unregister message filtering and idle updates
		CMessageLoop* pLoop = _Module.GetMessageLoop();
		ATLASSERT(pLoop != NULL);
		pLoop->RemoveMessageFilter(this);
		pLoop->RemoveIdleHandler(this);

		//g_pSharedPlayer->stop();
		

		bHandled = FALSE;
		return 1;
	}

	LRESULT OnFileExit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		PostMessage(WM_CLOSE);
		return 0;
	}

	LRESULT OnFileNew(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		return 0;
	}

	LRESULT OnViewToolBar(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		static BOOL bVisible = TRUE;	// initially visible
		bVisible = !bVisible;
		CReBarCtrl rebar = m_hWndToolBar;
		int nBandIndex = rebar.IdToIndex(ATL_IDW_BAND_FIRST + 1);	// toolbar is 2nd added band
		rebar.ShowBand(nBandIndex, bVisible);
		UISetCheck(ID_VIEW_TOOLBAR, bVisible);
		UpdateLayout();
		return 0;
	}

	LRESULT OnViewStatusBar(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		BOOL bVisible = !::IsWindowVisible(m_hWndStatusBar);
		::ShowWindow(m_hWndStatusBar, bVisible ? SW_SHOWNOACTIVATE : SW_HIDE);
		UISetCheck(ID_VIEW_STATUS_BAR, bVisible);
		UpdateLayout();
		return 0;
	}

	LRESULT OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		CAboutDlg dlg;
		dlg.DoModal();
		return 0;
	}


	LRESULT OnSize(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		int _width=LOWORD(lParam);
		int _hight=HIWORD(lParam);
		//m_DsoundControl.SetSpectrumRect(CRect(30,100,_width,_hight));

		bHandled=FALSE;
		return 0;
	}
	
	
	LRESULT OnDrawSpectrum(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		//m_pDsoundControl->DrawSpectrum();
		return 0;
	}

public:
	LRESULT OnPlay(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnOpen(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnPause(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnStop(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnConfig(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnFftDialog(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnShowLyric(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	
public:
	LRESULT OnTestInitplaylist(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
public:
	LRESULT OnFileOpendirectory(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

};
