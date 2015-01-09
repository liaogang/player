#pragma  once
#include "mainfrm.h"
#include "threadpool.h"

class CLastFmPaneView :
	public CWindowImpl<CLastFmPaneView >
{
public:

	CLastFmPaneView ()
	{

	}

	~CLastFmPaneView ()
	{

	}

public:
	DECLARE_WND_CLASS_EX(_T("CLastFmPaneView"),  CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS , COLOR_BTNSHADOW)

	BEGIN_MSG_MAP_EX(CLastFmPaneView )
		MSG_WM_DESTROY(OnDestroy)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MSG_WM_RBUTTONUP(OnRButtonUp)
		MSG_WM_SIZE(OnSize)
		MESSAGE_HANDLER(WM_NEW_TRACK_STARTED,OnNewTrackStarted)
		MESSAGE_HANDLER(WM_TRACKSTOPPED,OnStopped)
		MSG_WM_ERASEBKGND(OnEraseBkgnd)
		MSG_WM_LBUTTONDBLCLK(OnLButtonDblClk)
	END_MSG_MAP()
	
	void OnLButtonDblClk(UINT nFlags, CPoint point)
	{
		::PostMessage(GetMainFrame()->m_hWnd,WM_PLAYLISTVIEW_CENTER_ITEM,NULL,NULL);
	}


	HWND CreateMyWnd();

	BOOL OnEraseBkgnd(CDCHandle dc)
	{
		//no bkgnd repaint needed
		if (bHasPic)
			return 1;
		else 
		{
			SetMsgHandled(FALSE);
			return 0;
		}
	}

	void Init()
	{
		IWantToReceiveMessage(WM_NEW_TRACK_STARTED);
		IWantToReceiveMessage(WM_TRACKSTOPPED);

	}

	void OnSize(UINT nType, CSize size)
	{
		GetClientRect(&rc);
		SetMsgHandled(FALSE);
	}

	LRESULT OnPicSave(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		const TCHAR szFilter[]=_T("BMPÍ¼ÏñÎÄ¼þ(*.bmp)\0*.bmp\0");
		const TCHAR szDefaultExt[]=_T("bmp");

		CFileDialog dlg(FALSE,szDefaultExt,NULL,OFN_OVERWRITEPROMPT,szFilter ,m_hWnd);
		if(dlg.DoModal()==IDOK)
		{
			track->GetImg()->Save(dlg.m_ofn.lpstrFile);
		}

		return 0;
	}

	void OnRButtonUp(UINT nFlags, CPoint point)
	{
		::ClientToScreen(m_hWnd,&point);
		//::TrackPopupMenu(menu,TPM_LEFTALIGN,point.x,point.y,0,m_hWnd,0);
	}


	BOOL bHasPic;
	LPCPlayListItem track;
	RECT rc;

	tstring infoDisplay;
	LPCPlayListItem *item;
	Job1 job;
	void TrackChanged(LPCPlayListItem *item);


	LRESULT OnNewTrackStarted(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		LPCPlayListItem *item = (LPCPlayListItem *)wParam;
		if (item)
		{
			TrackChanged(item);
		}

		return 0;
	}

	LRESULT OnStopped(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		bHasPic=FALSE;
		Invalidate(TRUE);
		return 0;
	}


	LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		PAINTSTRUCT ps;
		::BeginPaint(m_hWnd,&ps);

		RECT m_rcClient = rc;

		::FillRect(ps.hdc, &m_rcClient, (HBRUSH)LongToPtr(COLOR_APPWORKSPACE + 1));

		SetBkMode(ps.hdc, TRANSPARENT);
		//::TextOut(ps.hdc, m_rcClient.left + (m_rcClient.right - m_rcClient.left) / 2 - 10, m_rcClient.top + (m_rcClient.bottom - m_rcClient.top) / 2 - 10, infoDisplay.c_str(), infoDisplay.length());
		::DrawText(ps.hdc, infoDisplay.c_str(), infoDisplay.length(), &m_rcClient, DT_CENTER);
		//::TextOut(ps.hdc, 0, 0 , infoDisplay.c_str(), infoDisplay.length());

		::EndPaint(m_hWnd,&ps);	
		return 0;
	}

	void ResetMenu()
	{

	}


	void OnDestroy()
	{
		IDonotWantToReceiveMessage(WM_NEW_TRACK_STARTED);
		IDonotWantToReceiveMessage(WM_TRACKSTOPPED);
		SetMsgHandled(FALSE);
	}

	virtual void OnFinalMessage(_In_ HWND /*hWnd*/)
	{


		delete this;
	}
};


