
#include "WTLTabViewCtrl.h"

//-----------------------------------------
//progress pos track bar
class CMyTrackBar
	:public CWindowImpl<CMyTrackBar,CTrackBarCtrl>
{
public:
	BOOL m_bPressing;
	CMyTrackBar():m_bPressing(FALSE)
	{
		
	}
public:
	DECLARE_WND_SUPERCLASS(NULL,CTrackBarCtrl::GetWndClassName())

	BEGIN_MSG_MAP_EX(CMyTrackBar)
		MESSAGE_HANDLER(WM_LBUTTONDOWN,OnLBtnDown)
		MESSAGE_HANDLER(TB_BUTTONCOUNT,TBB)
		MESSAGE_HANDLER(TB_GETITEMRECT,OnGetItemRect)
		MESSAGE_HANDLER(WM_ERASEBKGND,OnEraseBkgnd)
	END_MSG_MAP()



	LRESULT OnEraseBkgnd(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
	{
		return 1;
	}

	LRESULT OnLBtnDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
	{
		bHandled=FALSE;
		m_bPressing=TRUE;
		return 1;
	}

	LRESULT OnGetItemRect(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
	{
		RECT *rc=(RECT*)lParam;
		rc->left=0;
		rc->right=24;
		rc->bottom=24;
		rc->top=0;
		return 1;
	}

	LRESULT TBB(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		bHandled=TRUE;
		return 1;
	}

};


class CMyVolumeBar:
	public CWindowImpl<CMyVolumeBar,CTrackBarCtrl>
{
public:
	CMainFrame *pMain;
	BOOL m_bPressing;
	CMyVolumeBar():m_bPressing(FALSE)
	{

	}
public:
	DECLARE_WND_SUPERCLASS(NULL,CTrackBarCtrl::GetWndClassName())

	BEGIN_MSG_MAP(CMyVolumeBar)
		MESSAGE_HANDLER(WM_CREATE,OnCreate)
		MESSAGE_HANDLER(WM_LBUTTONDOWN,OnLBtnDown)
		MESSAGE_HANDLER(WM_MOUSEMOVE,OnMouseMove)
		MESSAGE_HANDLER(WM_LBUTTONUP,OnReleaseLB)
		MESSAGE_HANDLER(TB_BUTTONCOUNT,TBB)
		MESSAGE_HANDLER(TB_GETITEMRECT,OnGetItemRect)
		MESSAGE_HANDLER(WM_ERASEBKGND,OnEraseBkgnd)
	END_MSG_MAP()

	LRESULT OnEraseBkgnd(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
	{
		return 1;
	}


	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
	{
		SetPageSize(1);
		SetLineSize(1);
		SetThumbLength(2);
		SetRange(0,100);
		SetPos(100);
		bHandled=FALSE;
		return 1;
	}

	LRESULT OnLBtnDown(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
	{
		//TO DO ,SET POS
		SetCapture();
		
		bHandled=FALSE;
		m_bPressing=TRUE;
		return 1;
	}
	
	LRESULT OnMouseMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
	{    
		if (GetCapture()==m_hWnd){
			int pos=GetPos();
			OnPos(pos);
		}

		bHandled=FALSE;
		return 1;
	}

	LRESULT OnReleaseLB(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
	{	

		if (GetCapture()==m_hWnd){
			int pos=GetPos();
			OnPos(pos);
		}
		ReleaseCapture();

		m_bPressing=FALSE;
		return 1;
	}


	void OnPos(int pos);

	LRESULT OnGetItemRect(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
	{
		RECT *rc=(RECT*)lParam;
		rc->left=0;
		rc->right=24;
		rc->bottom=24;
		rc->top=0;
		return 1;
	}

	LRESULT TBB(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		bHandled=TRUE;
		return 1;
	}
	



};


class CMyStatusBar:public CWindowImpl<CMyStatusBar,CStatusBarCtrl>
{
	DECLARE_WND_SUPERCLASS(NULL,CStatusBarCtrl::GetWndClassName())

	BEGIN_MSG_MAP_EX(CMyStatusBar)
		MSG_WM_LBUTTONDBLCLK(OnLButtonDblClk)
	END_MSG_MAP()

	//Ë«»÷×´Ì¬À¸,¼¤»îµ±Ç°²¥·ÅÒô¹ì
	void OnLButtonDblClk(UINT nFlags, CPoint point);

	CMainFrame *pMain;
};



class CMySimpleRebar:public CWindowImpl<CMySimpleRebar,CReBarCtrl>
{   
public:
	DECLARE_WND_SUPERCLASS(NULL,CReBarCtrl::GetWndClassName())

	BEGIN_MSG_MAP_EX(CMySimpleRebar)
		MESSAGE_HANDLER(WM_CTLCOLORSTATIC,OnCtrlColorStatic)
	END_MSG_MAP()


	CMyTrackBar *pTrack;
	CMyVolumeBar *pVolume;
	LRESULT OnCtrlColorStatic(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		LRESULT lResult=FALSE;
		HDC dc=(HDC)wParam;
		HWND wnd=(HWND)lParam;

		if (wnd==pVolume->m_hWnd 
			||wnd==pTrack->m_hWnd 
			)
		{
			//!m_hWnd ,¸¸¿Ø¼þ,Rebar
			//!wnd    ,×Ó¿Ø¼þ,trackBar

			RECT rc;
			::GetWindowRect(m_hWnd,&rc);

			HDC dcMem;
			dcMem= ::CreateCompatibleDC(dc);
			HBITMAP bmp,oldBmp;
			bmp=::CreateCompatibleBitmap(dc,rc.right-rc.left,rc.bottom-rc.top);
			oldBmp=(HBITMAP)::SelectObject(dcMem,bmp);

			POINT pt={0,0};
			::MapWindowPoints(wnd,m_hWnd,&pt,1);
			::OffsetWindowOrgEx(dcMem,pt.x,pt.y,&pt);
			::SendMessage(m_hWnd,WM_ERASEBKGND, (WPARAM)dcMem, 0L);
			::SetWindowOrgEx(dcMem,pt.x,pt.y,NULL);
			::DeleteDC(dcMem);

			static HBRUSH hBrush=0;
			if (hBrush)
				::DeleteObject((HGDIOBJ)hBrush);

			hBrush = ::CreatePatternBrush(bmp);
			::DeleteObject(bmp);

			lResult=(LRESULT)hBrush;
		}

		return lResult;
	}

};	




// //-----------------------------------------
// //tab bar
// class CMyTabBar 
// 	:public CWindowImpl<CMyTabBar,CTabCtrl>
// {
// 	CPlayListView m_viewPlaylist;
// 	CSplitterWindow split;
// 	CHorSplitterWindow *leftPane;
// 	CAlbumCoverView    *albumView1;
// 	CAlbumCoverView    *albumView2;
// 
// public:
// 	DECLARE_WND_SUPERCLASS(NULL,CTabCtrl::GetWndClassName())
// 		
// 	BEGIN_MSG_MAP(CMyTabBar)
// 		MSG_WM_SIZE(OnSize)
// 		MESSAGE_HANDLER(WM_CREATE,OnCreate)
// 	END_MSG_MAP()
// 	
// 	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
// 	{
// 		const DWORD dwSplitStyle = WS_CHILD | WS_VISIBLE | 
// 			WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
// 			dwSplitExStyle = WS_EX_CLIENTEDGE;
// 	
// 		split.Create(m_hWnd, rcDefault, NULL, 
// 			dwSplitStyle, dwSplitExStyle );
// 
// 		//right pane
// 		m_viewPlaylist.Create(split.m_hWnd, rcDefault, NULL, LVS_SINGLESEL | WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | LVS_REPORT | LVS_SHOWSELALWAYS, WS_EX_CLIENTEDGE);
// 
// 		//left pane
// 		leftPane=new CHorSplitterWindow;
// 		leftPane->Create(split.m_hWnd,rcDefault,NULL,dwSplitStyle,dwSplitExStyle);
// 			albumView1=new CAlbumCoverView;
// 			albumView2=new CAlbumCoverView;
// 			albumView1->Create(leftPane->m_hWnd);
// 			albumView2->Create(leftPane->m_hWnd);
// 		leftPane->SetSplitterPanes(albumView1->m_hWnd,albumView2->m_hWnd);
// 
// 		//set left and right
// 		split.SetSplitterPanes(leftPane->m_hWnd,m_viewPlaylist.m_hWnd,TRUE);
// 		split.SetSplitterPos(250,TRUE);
// 		leftPane->SetSplitterPos(500,TRUE);
// 
// 		bHandled=FALSE;
// 		return 0;
// 	}
// 
// 	void OnSize(UINT nType, CSize size)
// 	{
// 		RECT itemRect={0};
// 		GetItemRect(0,&itemRect);
// 		::SetWindowPos(split.m_hWnd,NULL,0,50+(itemRect.bottom-itemRect.top),
//  			size.cx, size.cy -(itemRect.bottom-itemRect.top),
// 			SWP_NOZORDER | SWP_NOACTIVATE);
// 	}
// };






//-----------------------------------------
//CUpDownCtrl bar
// class CMyUpDownCtrl :public CWindowImpl<CMyUpDownCtrl,CUpDownCtrl>
// {
// public:
// 	DECLARE_WND_SUPERCLASS(NULL,CUpDownCtrl::GetWndClassName())
// 	
// 	BEGIN_MSG_MAP(CMyUpDownCtrl)
// 		MESSAGE_HANDLER(WM_CREATE,OnCreate)
// 		
// 	END_MSG_MAP()
// 	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
// 	{
// 		bHandled=FALSE;
// 		return 0;
// 	}
// };



//-----------------------------------------
//volume track bar
// class CMyVolumeBar :public CWindowImpl<CMyVolumeBar,CTrackBarCtrl>
// {
// public:
// 	DECLARE_WND_SUPERCLASS(NULL,CTrackBarCtrl::GetWndClassName())
// 	BEGIN_MSG_MAP(CMyVolumeBar)
// 	END_MSG_MAP()
// };

/*
class DialogSearch;
class CMyEdit:public CWindowImpl<CMyEdit,CEdit>
	,CEditCommands<CMyEdit>
{
	DECLARE_WND_SUPERCLASS(NULL,CEdit::GetWndClassName())
	BEGIN_MSG_MAP(CMyEdit)
		//MSG_WM_KEYDOWN(OnKeyDown)
		//MSG_WM_SYSKEYDOWN(OnChar)
		//MSG_WM_CHAR(OnChar)
		CHAIN_MSG_MAP_ALT(CEditCommands<CMyEdit>, 1)
	END_MSG_MAP()

	DialogSearch *m_pDlgSearch;
	//void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
};
*/


 
class CMySplitterWindow:
	public CSplitterWindowImpl<CSplitterWindowT<true>, true>
{
public:
	typedef CSplitterWindowImpl<CSplitterWindowT<true>, true> _baseClass1;
	DECLARE_WND_CLASS_EX(_T("WTL_SplitterWindow"), CS_DBLCLKS, COLOR_WINDOW)
	BEGIN_MSG_MAP(CMySplitterWindow)
		REFLECT_NOTIFICATIONS()
		CHAIN_MSG_MAP(_baseClass1)
	END_MSG_MAP()
};