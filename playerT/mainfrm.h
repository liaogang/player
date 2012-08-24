// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once
#include "stdafx.h"
#include "customMsg.h"
#include "AboutDlg.h"
#include "DialogConfig.h"
#include "WTLTabViewCtrl.h"

#ifndef _MAINFRAME_H
#define _MAINFRAME_H
//-----------------------------------------
class CAlbumCoverView;
class CDialogLyric;
class CMyTrackBar;
class CMyTabBar;
class CPlayListView;
class CWTLTabViewCtrl;
class CPlayListView;
class CMyVolumeBar;
class CMyStatusBar;
class DialogSearch;
class CProcessingDlg;
//-----------------------------------------

class CMainFrame : public CFrameWindowImpl<CMainFrame>, public CUpdateUI<CMainFrame>,
		public CMessageFilter, public CIdleHandler
{
public:
	DECLARE_FRAME_WND_CLASS(NULL, IDR_MAINFRAME)
public:
	CMyTrackBar *m_pTrackBar;
	CMyVolumeBar *m_pVolumeBar;
	
	CCommandBarCtrl m_CmdBar;
	CDialogConfig   m_dlgConfig;
	CComboBox m_wndComboBox;
	CDialogLyric *m_dlgLrc;
	CProcessingDlg *pDlgProcess;

	CMyTabBar *m_pTabBar;
	CPlayListView *m_pPlaylistView;
	CSplitterWindow split;
	CHorSplitterWindow *leftPane;
	CAlbumCoverView    *albumView1;
	CMyStatusBar *m_pStatus;
	DialogSearch *m_pDlgSearch;
public:
	CMainFrame():m_dlgLrc(NULL),
		m_pTrackBar(NULL),m_pVolumeBar(NULL),
		m_pDlgSearch(NULL),pDlgProcess(NULL)
	{
	}

	~CMainFrame(){}

	virtual BOOL PreTranslateMessage(MSG* pMsg);

	virtual BOOL OnIdle();

	BEGIN_UPDATE_UI_MAP(CMainFrame)
		UPDATE_ELEMENT(ID_VIEW_TOOLBAR, UPDUI_MENUPOPUP)
		UPDATE_ELEMENT(ID_VIEW_STATUS_BAR, UPDUI_MENUPOPUP)
		UPDATE_ELEMENT(ID_FILE_OPENDIRECTORY, UPDUI_MENUPOPUP)
		UPDATE_ELEMENT(ID_FILE_OPEN, UPDUI_MENUPOPUP)
		UPDATE_ELEMENT(ID_FILE_SAVEPLAYLIST, UPDUI_MENUPOPUP)
	END_UPDATE_UI_MAP()

	BEGIN_MSG_MAP_EX(CMainFrame)
		MSG_WM_NOTIFY(OnNotify)
		MESSAGE_HANDLER(WM_CLOSE,OnExit)
		COMMAND_CODE_HANDLER_EX(CBN_SELCHANGE,OnCbnSelchanged)
		MESSAGE_HANDLER(WM_ADDFOLDERED,OnAddFolder)
		MESSAGE_HANDLER(WM_TRACKSTOPPED,OnTrackStopped)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_DRAWSPECTRUM,OnDrawSpectrum)
		MESSAGE_HANDLER(WM_TRACKPOS,OnPos)
		MESSAGE_HANDLER(WM_NEW_TRACK_STARTED,OnNewTrackStarted)
		MESSAGE_HANDLER(WM_PLAY_DIRECTLY,OnPlayDirectly)
		MESSAGE_HANDLER(WM_FILE_FINDED,OnFileFinded)

		

		COMMAND_ID_HANDLER(ID_APP_EXIT, OnFileExit)
		COMMAND_ID_HANDLER(ID_FILE_NEW, OnFileNew)
		COMMAND_ID_HANDLER(ID_VIEW_TOOLBAR, OnViewToolBar)
		COMMAND_ID_HANDLER(ID_VIEW_STATUS_BAR, OnViewStatusBar)
		COMMAND_ID_HANDLER(ID_APP_ABOUT, OnAppAbout)
		COMMAND_ID_HANDLER(ID_SHOWLYRIC,OnShowLyric)
		COMMAND_ID_HANDLER(ID_MENU_FFTTEST,OnFftDialog)
		COMMAND_ID_HANDLER(ID_PLAY, OnPlay)
		COMMAND_ID_HANDLER(ID_FILE_OPEN, OnOpen)
		COMMAND_ID_HANDLER(ID_PAUSE, OnPause)
		COMMAND_ID_HANDLER(ID_STOP, OnStop)
		COMMAND_ID_HANDLER(ID_CONFIG, OnConfig)
		COMMAND_ID_HANDLER(ID_FILE_OPENDIRECTORY, OnFileOpendirectory)
		COMMAND_ID_HANDLER(ID_FILE_NEWPLAYLIST,OnFileNewPlaylist)
		COMMAND_ID_HANDLER(ID_OPENPLAYLIST,OnOpenPlaylist)
		COMMAND_ID_HANDLER(ID_FILE_SAVEPLAYLIST,OnSavePlaylist)

		CHAIN_MSG_MAP(CUpdateUI<CMainFrame>)
		CHAIN_MSG_MAP(CFrameWindowImpl<CMainFrame>)
	END_MSG_MAP()


	LRESULT OnNotify(int idCtrl, LPNMHDR pnmh);
	LRESULT OnPos(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnCbnSelchanged(UINT,int id, HWND hWndCtl);
	LRESULT OnTrackStopped(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
	LRESULT OnAddFolder(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
	LRESULT OnNewTrackStarted(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
	LRESULT OnPlayDirectly(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
	LRESULT OnPaused(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
	LRESULT OnFileFinded(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	LRESULT OnExit(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
		
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

		nBandIndex = rebar.IdToIndex(ATL_IDW_BAND_FIRST + 2);	// toolbar is 2nd added band
		rebar.ShowBand(nBandIndex, bVisible);

		nBandIndex = rebar.IdToIndex(ATL_IDW_BAND_FIRST + 3);	// toolbar is 2nd added band
		rebar.ShowBand(nBandIndex, bVisible);

		nBandIndex = rebar.IdToIndex(ATL_IDW_BAND_FIRST + 4);	// toolbar is 2nd added band
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
	
	//新建播放列表
	LRESULT OnFileNewPlaylist(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	//加载选中的播放列表文件
	LRESULT OnOpenPlaylist(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	//保存选中的播放列表
	LRESULT OnSavePlaylist(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnFileOpendirectory(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
public:
	void OnSelectionChanged();
	void ChangeTitle();
	const TCHAR *GetAppName()
	{
		const static TCHAR *gpAppName=_T("mp3 player");
		return gpAppName;
	}

	
	void ShowSearchDialog();
	void DoSomethingBeforeShow();
	void ShowDlgProcessFile();
};


#endif