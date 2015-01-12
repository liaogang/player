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
		MSG_WM_RBUTTONUP(OnRButtonUp)
		MSG_WM_SIZE(OnSize)
		MESSAGE_HANDLER(WM_NEW_TRACK_STARTED,OnNewTrackStarted)
		MESSAGE_HANDLER(WM_TRACKSTOPPED,OnStopped)
		MSG_WM_LBUTTONDBLCLK(OnLButtonDblClk)
	END_MSG_MAP()
	
	void OnLButtonDblClk(UINT nFlags, CPoint point)
	{
		::PostMessage(GetMainFrame()->m_hWnd,WM_PLAYLISTVIEW_CENTER_ITEM,NULL,NULL);
	}

	RECT rcImage;
	CStatic label1;
	CStatic label2;
	CHyperLink link;

	HWND CreateMyWnd();

	

	void Init()
	{
		IWantToReceiveMessage(WM_NEW_TRACK_STARTED);
		IWantToReceiveMessage(WM_TRACKSTOPPED);


	}

	void OnSize(UINT nType, CSize size)
	{
		GetClientRect(&rc);
	
	const int m = 8;

	rcImage = {0,0,250,250};

	if (label1.IsWindow())
	{
		RECT rcLabel1 = { rcImage.right, m, rc.right - m, rcImage.bottom };
		label1.SetWindowPos(0, rcLabel1.left, rcLabel1.top, rcLabel1.right - rcLabel1.left, rcLabel1.bottom - rcLabel1.top, SWP_NOZORDER);

		rcLabel1 = { m, rcImage.bottom + m, rc.right - m, rcImage.bottom + m + 330 };
		label2.SetWindowPos(0, rcLabel1.left, rcLabel1.top, rcLabel1.right - rcLabel1.left, rcLabel1.bottom - rcLabel1.top, SWP_NOZORDER);
		rcLabel1.top = rcLabel1.bottom + m;

		link.SetWindowPos(0, rcLabel1.left, rcLabel1.top, rcLabel1.right - rcLabel1.left, 40 , SWP_NOZORDER);
	}
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
	Job1 job2;
	void scrobble();
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


