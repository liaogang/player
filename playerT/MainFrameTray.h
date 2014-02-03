#include "stdafx.h"
#include "resource1.h"
#include "CMyTrayNotifyIcon.h"
#include "customMsg.h"

class CMainFrame : public CWindowImpl<CMainFrame>
	, public CUpdateUI<CMainFrame>
	, public CMessageFilter
	, public CIdleHandler
{
public:
	CMyTrayNofityIcon trayNI;
	int m_nOrder;
	TCHAR *m_strAction;
	int HotKeyId;


	DECLARE_WND_CLASS(_T("WTLNTRAY") )

	virtual BOOL PreTranslateMessage(MSG* pMsg)
	{
		return FALSE;
	}

	virtual BOOL OnIdle()
	{
		return FALSE;
	}

	BEGIN_UPDATE_UI_MAP(CMainFrame)
		UPDATE_ELEMENT(ID_STOP,UPDUI_MENUPOPUP)
		UPDATE_ELEMENT(ID_PAUSE,UPDUI_MENUPOPUP)
		UPDATE_ELEMENT(ID_PLAY,UPDUI_MENUPOPUP)
		UPDATE_ELEMENT(ID_PLAY_PREV,UPDUI_MENUPOPUP)
		UPDATE_ELEMENT(ID_PLAY_NEXT,UPDUI_MENUPOPUP)
		UPDATE_ELEMENT(ID_PLAY_RANDOM,UPDUI_MENUPOPUP)
		UPDATE_ELEMENT(ID_ORDER_DEFAULT,UPDUI_MENUPOPUP)
		UPDATE_ELEMENT(ID_ORDER_REPEAT_LIST,UPDUI_MENUPOPUP)
		UPDATE_ELEMENT(ID_ORDER_REPEAT_TRACK,UPDUI_MENUPOPUP)
		UPDATE_ELEMENT(ID_ORDER_RANDOM,UPDUI_MENUPOPUP)
	END_UPDATE_UI_MAP()

	BEGIN_MSG_MAP(CMainFrame)
		MESSAGE_HANDLER(WM_CREATE, OnCreate)
		MESSAGE_HANDLER(WM_TRACK_REACH_END, OnTrackReachEnd)
		MESSAGE_HANDLER(WM_HOTKEY,OnHotKey)
		MESSAGE_HANDLER(WM_TRAYNOTIFY, OnTrayNotification)
		COMMAND_HANDLER(ID_STOP, 0, OnStop)
		COMMAND_HANDLER(ID_PAUSE, 0, OnPause)
		COMMAND_HANDLER(ID_PLAY, 0, OnPlay)
		COMMAND_HANDLER(ID_PLAY_PREV, 0, OnPlayPrev)
		COMMAND_HANDLER(ID_PLAY_NEXT, 0, OnPlayNext)
		COMMAND_HANDLER(ID_PLAY_RANDOM, 0, OnPlayRandom)
		COMMAND_RANGE_CODE_HANDLER(ID_ORDER_DEFAULT,ID_ORDER_RANDOM, 0, OnSetOrder)
		COMMAND_HANDLER(ID_APP_EXIT, 0, OnFileExit)
		CHAIN_MSG_MAP(CUpdateUI<CMainFrame>)
	END_MSG_MAP()



	HWND Create()
	{
		RECT rc;
		memset(&rc,0,sizeof(RECT));
		return CWindowImpl<CMainFrame>::Create(NULL,rc,_T("WTLNTRAY"),WS_POPUPWINDOW,WS_EX_TOOLWINDOW);
	}

	void Update();

	LRESULT OnTrayNotification(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled=TRUE;
		trayNI.OnTrayNotification(wParam,lParam);
		return 0L;
	}


	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnTrackReachEnd(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);
	LRESULT OnPlay(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnPause(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnStop(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnPlayPrev(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnPlayNext(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnPlayRandom(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnSetOrder(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnFileExit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnHotKey(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	void RegisterMyHotKeys();
};

