// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once
#include "stdafx.h"
#include "AboutDlg.h"
#include "DialogConfig.h"
#include "CMyView.h"
#include "BasicPlayer.h"

class CAlbumCoverView;
class CDialogLyric;
static	CBasicPlayer *g_pSharedPlayer;
class CMyTrackBar :public CWindowImpl<CMyTrackBar,CTrackBarCtrl>
{
public:
	DECLARE_WND_SUPERCLASS(NULL,CTrackBarCtrl::GetWndClassName())

	BEGIN_MSG_MAP(CMyTrackBar)
		MESSAGE_HANDLER(TB_BUTTONCOUNT,TBB)
		MESSAGE_HANDLER(TB_GETITEMRECT,OnGetItemRect)
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

};


class CMainFrame : public CFrameWindowImpl<CMainFrame>, public CUpdateUI<CMainFrame>,
		public CMessageFilter, public CIdleHandler
{
public:
	DECLARE_FRAME_WND_CLASS(NULL, IDR_MAINFRAME)

	CMyView m_view;
	CMyTrackBar m_trackBar;
	CSplitterWindow split;
	CCommandBarCtrl m_CmdBar;
	CDialogConfig   m_dlgConfig;
	CComboBox m_wndComboBox;
	CDialogLyric *m_dlgLrc;
	CHorSplitterWindow *leftPane;
	CAlbumCoverView    *albumView1;
	CAlbumCoverView    *albumView2;


	CMainFrame():m_dlgLrc(NULL),albumView1(NULL),albumView2(NULL)
		,leftPane(NULL)
	{
	}

	~CMainFrame(){}

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
		COMMAND_CODE_HANDLER_EX(CBN_SELCHANGE,OnCbnSelchanged)
		MESSAGE_HANDLER(WM_TRACKSTOPPED,OnTrackStopped)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_DRAWSPECTRUM,OnDrawSpectrum)
		MESSAGE_HANDLER(WM_SIZE,OnSize)
		MESSAGE_HANDLER(WM_TRACKPOS,OnPos)
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
		COMMAND_ID_HANDLER(ID_FILE_OPENDIRECTORY, OnFileOpendirectory)
		CHAIN_MSG_MAP(CUpdateUI<CMainFrame>)
		CHAIN_MSG_MAP(CFrameWindowImpl<CMainFrame>)
	END_MSG_MAP()


	LRESULT OnPos(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnCbnSelchanged(UINT,int id, HWND hWndCtl);
	LRESULT OnTrackStopped(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
	
	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		// unregister message filtering and idle updates
		CMessageLoop* pLoop = _Module.GetMessageLoop();
		ATLASSERT(pLoop != NULL);
		pLoop->RemoveMessageFilter(this);
		pLoop->RemoveIdleHandler(this);
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
	LRESULT OnFileOpendirectory(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

};
