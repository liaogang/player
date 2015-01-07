// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once
#include "stdafx.h"
#include "customMsg.h"
#include "AboutDlg.h"
#include "forwardMsg.h"
#include "MySerialize.h"
#include "MyControls.h"
#include "mysplit.h"
#include "DialogSearch.h"
#ifndef _MAINFRAME_H
#define _MAINFRAME_H


class CDialogConfig;
class CAlbumCoverView;
class CDialogLyric;
class CPlayListView;
class CMyStatusBar;
class CProcessingDlg;
class CMySimpleRebar;
class DialogFFT;
class DialogFFTOutline;
class DialogPLManager;
class CDialogConsole;
class CMultiSpliltWnd;
class MYTREE;




class CMainFrame: 
	public CFrameWindowImpl<CMainFrame>, 
	public CIdleHandler,
	public CUpdateUI<CMainFrame>,
	public CMessageFilter, 
	public SerializeObj<CMainFrame>
{
public:
	DECLARE_FRAME_WND_CLASS(NULL, IDR_MAINFRAME)
public:
	typedef CFrameWindowImpl<CMainFrame> thebase;

	//load from file .
	BOOL m_bLoaded;
	//save to file 
	BOOL m_bSaved;
	UINT m_uShowState;
	WINDOWPLACEMENT m_wndsPlacement;
	//�ܱߴ���
	CMySimpleRebar m_wndRebar;
	CMyStatusBar *m_pStatus;

	//����ͼ
	CMultiSpliltWnd m_WndMultiSplitter;

	//��������
	CDialogConfig   *m_pDlgConfig;
	CDialogLyric *m_pDlgLrc;
	CProcessingDlg *pDlgProcess;
	DialogSearch m_DlgSearch;
	DialogFFT *m_pDlgFFT;
	DialogFFTOutline *m_pDlgFFTOutline;
	DialogPLManager *m_pDlgPLMng;
	CDialogConsole  *m_pDlgConsole;
	
	RECT m_rcConfig; 
	RECT m_rcLrc;
	RECT m_rcProcess;
	RECT m_rcFFT;
	RECT m_rcPLMng;
	RECT m_rcPLConsole;
	
	BOOL m_dlgConfigShow;
	RECT m_dlgLrcShow;
	RECT m_dlgProcessShow;
	RECT m_dlgSearchShow;
	RECT m_dlgFFTShow;
	RECT m_dlgPLMngShow;
	RECT m_dlgPLConsoleShow;

	BOOL m_bShowStatusBar;
	BOOL m_bExit2Tray;


	UINT m_nIDEvent;
	static const int m_uElapse=150;

	

public:
	CMainFrame():m_pDlgLrc(NULL),
		pDlgProcess(NULL),
		m_pDlgFFT(NULL),m_pDlgFFTOutline(NULL),
		m_pDlgPLMng(NULL),m_pDlgConsole(NULL),
		m_pDlgConfig(NULL),m_bShowStatusBar(TRUE),m_bExit2Tray(FALSE),
		m_bLoaded(FALSE),m_bSaved(FALSE)
	{
		RECT_INIT(m_rcConfig)
		RECT_INIT(m_rcLrc)
		RECT_INIT(m_rcProcess)
		RECT_INIT(m_rcFFT)
		RECT_INIT(m_rcPLMng)
		RECT_INIT(m_rcPLConsole)

		m_wndsPlacement.flags=0;
		m_wndsPlacement.showCmd=SW_SHOWNORMAL;
	}


	~CMainFrame();

	


	virtual BOOL PreTranslateMessage(MSG* pMsg);

	virtual BOOL OnIdle();

	BEGIN_UPDATE_UI_MAP(CMainFrame)

// 		UPDATE_ELEMENT(ID_VIEW_TOOLBAR, UPDUI_MENUPOPUP)
 		UPDATE_ELEMENT(ID_VIEW_STATUS_BAR, UPDUI_MENUPOPUP)
		UPDATE_ELEMENT(ID_FILE_OPENDIRECTORY, UPDUI_MENUPOPUP)
		UPDATE_ELEMENT(ID_FILE_OPEN, UPDUI_MENUPOPUP)
		UPDATE_ELEMENT(ID_FILE_SAVEPLAYLIST, UPDUI_MENUPOPUP)
	END_UPDATE_UI_MAP()

	BEGIN_MSG_MAP(CMainFrame)
		
		MESSAGE_HANDLER(WM_NOTIFY,OnNotify)
		MESSAGE_HANDLER(WM_CLOSE,OnExit)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_NCDESTROY,OnNcDestroy)
		MESSAGE_HANDLER(WM_DRAWSPECTRUM,OnDrawSpectrum)
		MESSAGE_HANDLER(WM_HOTKEY,OnHotKey)

		//message from message center
		MESSAGE_HANDLER(WM_FILE_FINDED,OnFileFinded)
		MESSAGE_HANDLER(WM_ADDFOLDERED,OnAddFolder)
		MESSAGE_HANDLER(WM_NEW_TRACK_STARTED,OnNewTrackStarted)
		MESSAGE_HANDLER(WM_TRACK_REACH_END,OnTrackReachEnd)
		MESSAGE_HANDLER(WM_TRACKSTOPPED,OnTrackStopped)
		//MESSAGE_HANDLER(WM_PL_WILL_DELETED,OnPLChanged)
		//MESSAGE_HANDLER(WM_PL_TRACKNUM_CHANGED,OnPLTrackNumChanged)

		MESSAGE_HANDLER(WM_SHOW_DIALOG_CONFIGURE,OnMsgShowDialogConfigure)
		MESSAGE_HANDLER(WM_SAVE_CONFIGURE ,OnMsgSaveConfigure)
		MESSAGE_HANDLER(WM_SHOW_DIALOG_SEARCH ,OnMsgShowDialogSearch)
		MESSAGE_HANDLER(WM_SHOW_DIALOG_PLAYLIST_MANAGER ,OnMsgShowDialogPlaylistManager)
//MESSAGE_HANDLER(WM_CHANGE_LISTVIEW_FONT_ENLARGE 
//MESSAGE_HANDLER(WM_CHANGE_LISTVIEW_FONT_REDUCE 

		//message from message center

		MESSAGE_HANDLER(WM_PLAYLISTVIEW_CENTER_ITEM,OnPLVCenterItem)
		MESSAGE_HANDLER(WM_ACTIVATE,OnWndActive)
		MSG_WM_TIMER(OnTimer)

		COMMAND_ID_HANDLER(ID_EDIT_SEARCH, OnEditSearch)
		COMMAND_ID_HANDLER(ID_EXIT2TRAY, OnFileExit)
		COMMAND_ID_HANDLER(ID_APP_EXIT, OnFileExit)
		COMMAND_ID_HANDLER(ID_FILE_NEW, OnFileNew)
 		COMMAND_ID_HANDLER(ID_VIEW_STATUS_BAR, OnViewStatusBar)
		COMMAND_ID_HANDLER(ID_CHANGEPLAYLISTCOLOR_DEFAULT,OnChangePLColorDefault)
		COMMAND_ID_HANDLER(ID_CHANGEPLAYLISTCOLOR_BLUE,OnChangePLColorBlue)
		COMMAND_ID_HANDLER(ID_VIEW_PLAYLIST_MNG,OnViewPlaylistManager)		
		COMMAND_ID_HANDLER(ID_APP_ABOUT, OnAppAbout)
		COMMAND_ID_HANDLER(ID_CONFIG_LYRIC,OnConfigLyric)
		COMMAND_ID_HANDLER(ID_MEDIALIBRARY_CONFIG,OnConfigMediaLib)
		COMMAND_ID_HANDLER(ID_SHOWLYRIC,OnShowLyric)
		COMMAND_ID_HANDLER(ID_VIEW_CONSOLE,OnShowConsole)
		COMMAND_ID_HANDLER(ID_MENU_FFTTEST,OnFftDialog)
		COMMAND_ID_HANDLER(ID_PLAY, OnPlay)
		COMMAND_ID_HANDLER(ID_FILE_OPEN, OnOpen)
		COMMAND_ID_HANDLER(ID_PAUSE, OnPause)
		COMMAND_ID_HANDLER(ID_STOP, OnStop)
		COMMAND_ID_HANDLER(ID_PLAY_NEXT, OnPlayNext)
		COMMAND_ID_HANDLER(ID_PLAY_RANDOM, OnPlayRandom)
		COMMAND_ID_HANDLER(ID_CONFIG, OnConfig)
		COMMAND_ID_HANDLER(ID_SAVE_CONFIG, OnSaveConfig)
		COMMAND_ID_HANDLER(ID_FILE_OPENDIRECTORY, OnFileOpendirectory)
		COMMAND_ID_HANDLER(ID_FILE_NEWPLAYLIST,OnFileNewPlaylist)
		COMMAND_ID_HANDLER(ID_OPENPLAYLIST,OnOpenPlaylist)
		COMMAND_ID_HANDLER(ID_FILE_SAVEPLAYLIST,OnSavePlaylist)
		COMMAND_ID_HANDLER(ID_LV_ENLARGE_FONT,OnChangeLVFont)
		COMMAND_ID_HANDLER(ID_LV_DECREASE_FONT,OnChangeLVFont)
		COMMAND_ID_HANDLER(ID_EDIT_REMOVE_DEAD,OnRemoveDeadItems)
		COMMAND_ID_HANDLER(ID_EDIT_REMOVE_DUP,OnRemoveDuplicates)
		COMMAND_ID_HANDLER(ID_CONFIG_LAYOUT,OnConfigLayout)
		CHAIN_MSG_MAP(CUpdateUI<CMainFrame>)
		CHAIN_MSG_MAP(CFrameWindowImpl<CMainFrame>)

		//REFLECT_NOTIFICATIONS()
	END_MSG_MAP()



	FILE& operator<<(FILE& f);
	FILE& operator>>(FILE& f) const ;


	void OnTimer(UINT_PTR /*nIDEvent*/)
	{
		NotifyMsg(WM_USER_TIMER,FALSE,0,0);
	}

	LRESULT OnNotify2(int idCtrl, LPNMHDR pnmh);
	
	LRESULT OnNotify(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		return OnNotify2((int)wParam, (LPNMHDR)lParam); 
	}

		
	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnTrackReachEnd(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
	LRESULT OnTrackStopped(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
	LRESULT OnPLVCenterItem(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
	LRESULT OnWndActive(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
	LRESULT OnAddFolder(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
	LRESULT OnNewTrackStarted(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
	LRESULT OnPaused(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
	LRESULT OnFileFinded(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnChangePLColorDefault(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnChangePLColorBlue(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnViewPlaylistManager(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);	
	LRESULT OnExit(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
	LRESULT OnNcDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
	
	LRESULT OnMsgShowDialogConfigure(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		showDialogConfigure();
		return 0;
	}
	LRESULT OnMsgSaveConfigure(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		SaveAll();
		return 0;
	}
	LRESULT OnMsgShowDialogSearch(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		ShowSearchDialog();
		return 0;
	}
	LRESULT OnMsgShowDialogPlaylistManager(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		showDialogPlaylistManager();
		return 0;
	}


	LRESULT OnEditSearch(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		ShowSearchDialog();
		return 0;
	}

	LRESULT OnFileExit(WORD wNotifyCode, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		if(wID == ID_EXIT2TRAY )
			m_bExit2Tray=TRUE;

		PostMessage(WM_CLOSE);
		return 0;
	}

	LRESULT OnFileNew(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		return 0;
	}

	//LRESULT OnViewToolBar(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	//{
	//	static BOOL bVisible = TRUE;	// initially visible
	//	bVisible = !bVisible;
	//	CReBarCtrl rebar = m_hWndToolBar;

	//	for(int i=1;i<=4;++i)
	//	rebar.ShowBand(rebar.IdToIndex(ATL_IDW_BAND_FIRST + i), bVisible);

	//	UISetCheck(ID_VIEW_TOOLBAR, bVisible);
	//	UpdateLayout();
	//	return 0;
	//}
	
	LRESULT OnViewStatusBar(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		m_bShowStatusBar=!m_bShowStatusBar;
		ShowStatusBar(m_bShowStatusBar);
		return 0;
	}

	void ShowStatusBar(BOOL bShow)
	{
		::ShowWindow(m_hWndStatusBar, bShow ? SW_SHOWNOACTIVATE : SW_HIDE);
		UISetCheck(ID_VIEW_STATUS_BAR, bShow);
		UpdateLayout();
	}

	LRESULT OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		CAboutDlg dlg;
		dlg.DoModal();
		return 0;
	}


	LRESULT OnDrawSpectrum(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		return 0;
	}

	LRESULT OnHotKey(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);



public:

	LRESULT OnPlay(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnOpen(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnPause(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnStop(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnPlayNext(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnPlayRandom(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnConfig(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnSaveConfig(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnConfigLayout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnConfigLyric(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnConfigMediaLib(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

	
	LRESULT OnFftDialog(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnShowLyric(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnShowConsole(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

	//�½������б�
	LRESULT OnFileNewPlaylist(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	//����ѡ�еĲ����б��ļ�
	LRESULT OnOpenPlaylist(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	//����ѡ�еĲ����б�
	LRESULT OnSavePlaylist(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnFileOpendirectory(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnChangeLVFont(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnRemoveDeadItems(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnRemoveDuplicates(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
public:
	void OnSelectionChanged();
	void ChangeTitle();

	void OnChangeLVFont(int offset);


	void ShowDlgProcessFile();
	void lrcChanged();
	void UpdateTreeView(MYTREE *treeData);
	void ShowSearchDialog();

	void showDialogConfigure();

	void showDialogPlaylistManager();
	
	void ReceiveMsg();
	
	void registCmdFuntions();
	//when  player booted
	
	void ShowDialog_Configure();

	void InitUILayout();

	int HotKeyId;
	void RegisterMyHotKeys();
	

	//when player is shuting down
	void OnFinalMessage(_In_ HWND /*hWnd*/);
	

};




#endif