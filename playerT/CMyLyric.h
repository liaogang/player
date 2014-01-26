#include "stdafx.h"
#include "DialogLrcList.h"
#include "dlgLrcSearch.h"
#include "LrcMng.h"
#include "customMsg.h"

#pragma once
using namespace std;

#define WIDTH(rc) ((rc).right-(rc.left))
#define HEIGHT(rc) ((rc).bottom-(rc.top))

class FileTrack;
class PlayListItem;

class lineinfo{
public:
	LONG yPos;
	int nStrLines;//几行字?
};


class CMyLyricWnd:public CWindowImpl<CMyLyricWnd>
{
public:
	HMENU menu,trackMenu;
	HBRUSH brush,oldBrush;
	HPEN  newPen,oldPen; 

	UINT m_uTotalTime;
	UINT m_uCurrTime;
	SIZE sz;
	BOOL bLrcReady;
	FileTrack* track;
	std::wstring title;
	RECT m_rcClient;
	HDC m_memDCNormal,m_memDCHighlight;
	CFont m_Font;
	vector<lineinfo> veclineinfo;
	int m_iCurLine;
	vector<LrcLine> lrcs;
	int m_nFontHeight;//字高
	int m_iESize;//歌词上方的空白区域
	UINT m_nIDEvent;
	bool m_bPaused;
	static const int m_uElapse=150;
	BOOL m_bSized;
	BOOL m_bInitSize;

	CDlgLrcSearch dlgLrcSearch;
	BOOL m_memDCReady;
	BOOL bNewTrack;
public:
	CMyLyricWnd();

	~CMyLyricWnd();

public:
	DECLARE_WND_CLASS_EX(_T("MyLyricsView"), CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS,COLOR_BTNSHADOW)

	BEGIN_MSG_MAP_EX(CMyLyricWnd)
		MESSAGE_HANDLER(WM_TRACK_POS_CHANGED,OnTrackPosChanged)
		MESSAGE_HANDLER(WM_LYRIC_RELOAD,OnLyricReload)
		MESSAGE_HANDLER(WM_NEW_TRACK_STARTED,OnNewTrackStarted)
		MESSAGE_HANDLER(WM_PAUSED,OnPaused)
		MESSAGE_HANDLER(WM_PAUSE_START,OnPauseStarted)
		MESSAGE_HANDLER(WM_TRACKSTOPPED,OnTrackStopped)
		MSG_WM_TIMER(OnTimer)
		MSG_WM_CREATE(OnCreate)
		MSG_WM_DESTROY(OnDestroy)
		MSG_WM_ERASEBKGND(OnEraseBkgnd)
		MSG_WM_PAINT(OnPaint)
		MESSAGE_HANDLER(WM_SIZE,OnSize)
		MSG_WM_RBUTTONUP(OnRButtonUp)
		COMMAND_ID_HANDLER(IDOK, OnCloseCmd)
		COMMAND_ID_HANDLER(IDCANCEL, OnCloseCmd)
		COMMAND_ID_HANDLER(ID_OPEN_LRC_PATH, OnMenuFolderOpen)
		COMMAND_ID_HANDLER(ID_OPEN_LRCFILE, OnOpenLrcFile)
		COMMAND_ID_HANDLER(ID_MENU_LRC_PANE_PROPERTY,OnProperty)
		COMMAND_ID_HANDLER(ID_MENU_SEARCH_ONLINE,OnShowDlgLrcSearch)
		COMMAND_ID_HANDLER(ID_SHOW_LRC_LIST,OnShowDlgLrcList)
		COMMAND_ID_HANDLER(ID_RELOAD_LYRICS,OnReloadLyrics)
	END_MSG_MAP()


	void OnTimer(UINT_PTR /*nIDEvent*/);
	void UpdateTime();

	//return true if line number changed.
	BOOL UpdateCurrLine();

	void OnDestroy();
	BOOL OnEraseBkgnd(CDCHandle dc)
	{
		//no bkgnd repaint needed
		if(bLrcReady)
			return 1;
		else 
		{
			SetMsgHandled(FALSE);
			return 0;
		}
	}

	int OnCreate(LPCREATESTRUCT lpCreateStruct)
	{
		Init();

		SetMsgHandled(FALSE);
		return 0;
	}


	LRESULT OnNewTrackStarted(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
	{
		PrepareShowLyric();
		return 0;
	}

	LRESULT OnPaused(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		m_bPaused=true;
		return 0;
	}

	LRESULT OnPauseStarted(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		m_bPaused=false;
		return 0;
	}

	LRESULT OnTrackStopped(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		StopShowLyric();

		ResetTitle();

		Invalidate(TRUE);

		return 0;
	}



	void OnPaint(CDCHandle dc)
	{
		PAINTSTRUCT ps;
		::BeginPaint(m_hWnd,&ps);
		if(bLrcReady)
			mydraw(ps.hdc);
		else
		{
			::FillRect(ps.hdc,&m_rcClient,(HBRUSH)LongToPtr(COLOR_APPWORKSPACE+ 1));

			SetBkMode(ps.hdc,TRANSPARENT);
			::TextOut(ps.hdc,m_rcClient.left+(m_rcClient.right-m_rcClient.left)/2 - 10,
				m_rcClient.top+(m_rcClient.bottom - m_rcClient.top )/2 - 10,L"无歌词",wcslen(L"无歌词"));
		}

		::EndPaint(m_hWnd,&ps);	
	}

	static bool RectIsEqual(RECT &a,RECT &b)
	{
		return a.left==b.left && a.right==b.right &&
			a.top==b.top && a.bottom == b.bottom;
	}

	//If an application processes this message, it should return zero. 
	LRESULT OnSize(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		if(m_bInitSize==FALSE)
		{
			GetClientRect(&m_rcClient);
			PrepareShowLyric();
			m_bInitSize=TRUE;
		}
		else 
		{
			RECT rcTemp=m_rcClient;
			GetClientRect(&m_rcClient);
			if( bLrcReady && !RectIsEqual(rcTemp ,m_rcClient) && m_rcClient.right!=0 &&m_rcClient.bottom!=0 )
				CreateBackDC();
		}

		bHandled=FALSE;
		return 1;
	}

	LRESULT OnProperty(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		// 			CColorDialog dlg;
		// 			dlg.DoModal(m_hWnd);
		return 0;
	}


	LRESULT OnReloadLyrics(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		StopShowLyric();
		PrepareShowLyric();

		return 0;
	}


	LRESULT OnShowDlgLrcList(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);




	LRESULT OnShowDlgLrcSearch(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);



	LRESULT OnTrackPosChanged(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		UpdateTime();
		if(UpdateCurrLine())
			mydraw();

		return 0;
	}

	LRESULT OnLyricReload(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		PrepareShowLyric();
		return 0;
	}

	void OnRButtonUp(UINT nFlags, CPoint point)
	{
		::ClientToScreen(m_hWnd,&point);
		::TrackPopupMenu(trackMenu,TPM_LEFTALIGN,point.x,point.y,0,m_hWnd,0);
	}



	//We will Create two memory drawing context
	//One is normal,another is highlighted
	//Then,we will cut on line space from the true display draw context
	//parse with the highlighted memory one
	//parse other with the normal memory one
	void CreateBackDC();



	void mydraw(HDC dc_=NULL)
	{
		if(m_memDCReady==FALSE)
			return;

		HDC dc=dc_==NULL?GetDC():dc_;

		if(m_bSized)
		{
			CreateBackDC();
			m_bSized=FALSE;
		}

		RECT rcDest;
		GetClientRect(&rcDest);

		if(!bLrcReady)return;

		int y= veclineinfo[m_iCurLine].yPos - (m_rcClient.bottom-m_rcClient.top)/2 ;

		// bitblt the lyrics
		RECT rcLine=rcDest;
		rcLine.top=HEIGHT(rcDest) /2;
		rcLine.bottom=rcLine.top+veclineinfo[m_iCurLine].nStrLines*m_nFontHeight;

		::BitBlt(dc,rcDest.left,rcDest.top,WIDTH(rcDest),HEIGHT(rcDest),
			m_memDCNormal,0,y , SRCCOPY);

		::BitBlt(dc,rcLine.left,rcLine.top,WIDTH(rcLine),HEIGHT(rcLine),
			m_memDCHighlight,0,y+rcLine.top, SRCCOPY);

		if(dc_==NULL)
			ReleaseDC(dc);
	}

	void PrepareShowLyric();

	void StopShowLyric()
	{
		if(m_nIDEvent!=0)
			KillTimer((UINT_PTR)&m_nIDEvent);
		bLrcReady=FALSE;
		m_memDCReady=FALSE;
	}

	void Init();

	void ChangedTitle()
	{
		SetWindowText((LPTSTR)title.c_str());
	}

	void ResetTitle()
	{
		SetWindowText(_T("歌词"));
	}

	LRESULT OnOpenLrcFile(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

	LRESULT OnMenuFolderOpen(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);


	LRESULT OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		ShowWindow(SW_HIDE);
		return 0;
	}

	virtual void OnFinalMessage(_In_ HWND /*hWnd*/)
	{
		//todo
		//delete this;
	}
};

