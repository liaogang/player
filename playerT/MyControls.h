
//#include "WTLTabViewCtrl.h"
#include "globalStuffs.h"
//#include "BasicPlayer.h"
#include "MyRebar.h"
#pragma once


//-----------------------------------------
//progress pos track bar
class CMyTrackBarBase
	:public CWindowImpl<CMyTrackBarBase,CTrackBarCtrl>
{
public:
	DECLARE_WND_SUPERCLASS(NULL,CTrackBarCtrl::GetWndClassName())

	BEGIN_MSG_MAP_EX(CMyTrackBarBase)
		MESSAGE_HANDLER(WM_GET_BAND_CLASSNAME,OnGetBandClassName)
		MSG_WM_SETFOCUS(OnSetFocus)
		MESSAGE_HANDLER(WM_ERASEBKGND,OnEraseBkgnd)
		MESSAGE_HANDLER(WM_LBUTTONDOWN,OnLButtonDown)
		MSG_WM_RBUTTONDOWN(OnRButtonDown)
	END_MSG_MAP()


	void OnRButtonDown(UINT nFlags, CPoint point)
	{
		::SetCapture(::GetParent(m_hWnd));
	}
	
	LRESULT OnGetBandClassName(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		_tcscpy((TCHAR*)wParam,_T("ComboBox"));
		return 0;
	}

	bool m_bPressing;
	LRESULT OnLButtonDown(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		SetCapture();
		m_bPressing=true;

		bHandled=FALSE;

		POINT point;
		point.x=GET_X_LPARAM(lParam);
		point.y=GET_Y_LPARAM(lParam); 
		

		RECT r;
		GetChannelRect(&r);

		if(r.left >= r.right) return 1;

		int start, stop;
		GetRange(start, stop);

		int pos = GetPos();

		r.left += 3;
		r.right -= 4;
		
		if(point.x < r.left) SetPos(start);
		else if(point.x >= r.right) SetPos(stop);
		else
		{
			int w = r.right - r.left;
			if(start < stop)
				SetPosInternal(start + ((stop - start) * (point.x - r.left) + (w/2)) / w);
		}


		return 0;
	}

	

	//we don't want a focus rect appear around
	void OnSetFocus(CWindow wndOld)
	{
		::SetFocus(wndOld.m_hWnd);
	}


	void SetPosInternal(int pos)
	{
		SetPos(pos);
		GetParent().PostMessage(WM_HSCROLL, MAKEWPARAM((short)pos, SB_THUMBPOSITION), (LPARAM)m_hWnd); // this will be reflected back on us
	}



	LRESULT OnEraseBkgnd(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
	{
		return 1;
	}

	HWND CreateIsWnd(HWND parent);

	void EnableWindow(BOOL bEnable)
	{
		::EnableWindow(m_hWnd,bEnable);
	}
};



class CMyTrackBar
	:public CMyTrackBarBase
	,public CCustomDraw<CMyTrackBar>
{
public:
	typedef  CMyTrackBarBase baseclass;
	
	static BOOL bRegister;
	CMyTrackBar()
	{
		if(bRegister==FALSE)
		{
			CMySimpleRebar::RegisterRebarBand(GetBandClassName(),CreateTrackBarBand);
			bRegister=TRUE;
		}
	}

	static TCHAR *GetBandClassName()
	{
		return _T("定位");
	}

	static HWND CreateTrackBarBand(HWND parent)
	{
		CMyTrackBar *track=new CMyTrackBar;
		return track->CreateIsWnd(parent);
	}
public:
	DECLARE_WND_SUPERCLASS(NULL,CMyTrackBarBase::GetWndClassName())

	BEGIN_MSG_MAP_EX(CMyTrackBar)
		MSG_WM_DESTROY(OnDestroy)

		//user message
		MESSAGE_HANDLER(WM_NEW_TRACK_STARTED,OnNewTrackStarted)
		MESSAGE_HANDLER(WM_PAUSED,OnPaused)
		MESSAGE_HANDLER(WM_PAUSE_START,OnPauseStarted)
		MESSAGE_HANDLER(WM_TRACKSTOPPED,OnTrackStopped)
		//
		
		MSG_WM_TIMER(OnTimer)
		MESSAGE_HANDLER(TB_BUTTONCOUNT,TBB)
		MESSAGE_HANDLER(TB_GETITEMRECT,OnGetItemRect)
		NOTIFY_CODE_HANDLER(TTN_NEEDTEXT,OnNeedText)
		MESSAGE_HANDLER(WM_HSCROLL,OnHscroll)
		REFLECTED_NOTIFY_CODE_HANDLER(NM_RELEASEDCAPTURE,OnMouseReleased)

		CHAIN_MSG_MAP(CMyTrackBarBase)


		//ALT_MSG_MAP(1) ,,//the reflect msg has problem ,
		CHAIN_MSG_MAP_ALT(CCustomDraw<CMyTrackBar>,1)	
		END_MSG_MAP()

	LRESULT OnMouseReleased(int /*idCtrl*/, LPNMHDR pnmh, BOOL& /*bHandled*/);

	bool m_bPaused;
	bool m_bStopped;
	UINT m_uCurrTime;

	void OnTimer(UINT_PTR /*nIDEvent*/)
	{
		if(!m_bPaused && !m_bStopped)
		{
			m_uCurrTime+=m_uElapse;
		
			if(!m_bPressing)
				SetPos(m_uCurrTime/1000);
		}
	}

	CToolTipCtrl m_CtrlTooltip;
	HWND CreateIsWnd(HWND parent)
	{
		HWND hWnd=baseclass::CreateIsWnd(parent);

		EnableWindow(FALSE);

		m_CtrlTooltip.Create(hWnd);
		m_CtrlTooltip.AddTool(hWnd, LPSTR_TEXTCALLBACK); 

		IWantToReceiveMessage(WM_NEW_TRACK_STARTED);
		IWantToReceiveMessage(WM_PAUSED);
		IWantToReceiveMessage(WM_PAUSE_START);
		IWantToReceiveMessage(WM_TRACKSTOPPED);

		return hWnd;
	}
	
	LRESULT OnHscroll(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
	{
		m_CtrlTooltip.Update();
		return 0;
	}

	LRESULT OnNeedText(int /*idCtrl*/, LPNMHDR pnmh, BOOL& /*bHandled*/)
	{
		TOOLTIPTEXT* pttt = (TOOLTIPTEXT*) pnmh;

		int curSec=GetPos();
		int minute=curSec / 60;
		int sec=curSec - minute*60 ;

		_stprintf(pttt->szText,_T("%d:%d"),minute,sec);
		
		return 0;
	}

	DWORD OnPrePaint(int /*idCtrl*/, LPNMCUSTOMDRAW lpNMCustomDraw)
	{
		return CDRF_NOTIFYITEMDRAW;
	}


	DWORD OnItemPrePaint(int /*idCtrl*/, LPNMCUSTOMDRAW lpNMCustomDraw)
	{
		DWORD dwRet=CDRF_DODEFAULT;
		NMLVCUSTOMDRAW* pLVCD = reinterpret_cast<NMLVCUSTOMDRAW*>( lpNMCustomDraw );  

		if (pLVCD->nmcd.dwItemSpec==TBCD_THUMB 
			&& !IsWindowEnabled())
			dwRet=CDRF_SKIPDEFAULT;

		return dwRet;
	}

	UINT m_nIDEvent;
	static const int m_uElapse=500;
	LRESULT OnNewTrackStarted(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
	{
		EnableWindow(TRUE);

		int totalSec=getTrackPosInfo()->used+getTrackPosInfo()->left;

		SetRange(0,totalSec);
		SetPos((int)getTrackPosInfo()->used);

		m_uCurrTime=getTrackPosInfo()->used * 1000;

		SetTimer((UINT_PTR)&m_nIDEvent,m_uElapse,NULL);

		m_bPaused=false;
		m_bStopped=false;

		

		return 0;
	}


	LRESULT OnPaused(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
	{
		m_bPaused=true;
		return 0;
	}

	LRESULT OnPauseStarted(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
	{
		m_bPaused=false;
		SetPos((int)getTrackPosInfo()->used);

		m_uCurrTime=getTrackPosInfo()->used * 1000;
		return 0;
	}
	

	LRESULT OnTrackStopped(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
	{
		m_bStopped=true;
		EnableWindow(FALSE);
		KillTimer(m_nIDEvent);
		return 0;
	}

	void OnDestroy()
	{
		IDonotWantToReceiveMessage(WM_NEW_TRACK_STARTED);
		IDonotWantToReceiveMessage(WM_PAUSED);
		IDonotWantToReceiveMessage(WM_PAUSE_START);
		IDonotWantToReceiveMessage(WM_TRACKSTOPPED);
	}

	
	LRESULT OnGetItemRect(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
	{
		if(lParam==NULL)
			return 0;

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
	public CMyTrackBarBase
	,public CCustomDraw<CMyVolumeBar>
{
public:
	static BOOL bRegister;
	CMyVolumeBar()
	{
		if(bRegister==FALSE)
		{
			CMySimpleRebar::RegisterRebarBand(GetBandClassName(),CreateVolumeBarBand);
			bRegister=TRUE;
		}
	}

	static TCHAR *GetBandClassName()
	{
		return _T("音量控制");
	}

	static HWND CreateVolumeBarBand(HWND parent)
	{
		CMyVolumeBar *vol=new CMyVolumeBar;
		return vol->CreateIsWnd(parent);
	}

public:
	DECLARE_WND_SUPERCLASS(NULL,CMyTrackBarBase::GetWndClassName())

	BEGIN_MSG_MAP(CMyVolumeBar)
		MESSAGE_HANDLER(WM_CREATE,OnCreate)
		MESSAGE_HANDLER(WM_LBUTTONUP,OnLButtonUp)
		MESSAGE_HANDLER(WM_MOUSEMOVE,OnMouseMove)
		MESSAGE_HANDLER(TB_BUTTONCOUNT,TBB)
		MESSAGE_HANDLER(TB_GETITEMRECT,OnGetItemRect)
		CHAIN_MSG_MAP( CMyTrackBarBase)

		CHAIN_MSG_MAP_ALT(CCustomDraw<CMyVolumeBar>, 1)
	END_MSG_MAP()

	DWORD OnPrePaint(int /*idCtrl*/, LPNMCUSTOMDRAW lpNMCustomDraw)
	{
		return CDRF_NOTIFYITEMDRAW;
	}
	

	DWORD OnItemPrePaint(int /*idCtrl*/, LPNMCUSTOMDRAW pNMCD);
	
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



	LRESULT OnMouseMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
	{    
		if(m_bPressing)
			OnPos(GetPos());

		bHandled=FALSE;
		return 1;
	}


	LRESULT OnLButtonUp(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled=FALSE;
		m_bPressing=false;
		ReleaseCapture();
		return 0;
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

	void Init();
};


class CMyStatusBar:public CWindowImpl<CMyStatusBar,CStatusBarCtrl>
{
public:
	DECLARE_WND_SUPERCLASS(NULL,CStatusBarCtrl::GetWndClassName())

	BEGIN_MSG_MAP_EX(CMyStatusBar)
		MSG_WM_LBUTTONDBLCLK(OnLButtonDblClk)

		//user message
// 		MESSAGE_HANDLER(WM_NEW_TRACK_STARTED,OnPlay)
// 		MESSAGE_HANDLER(WM_PAUSED,OnPaused)
// 		MESSAGE_HANDLER(WM_PAUSE_START,OnResume)
// 		MESSAGE_HANDLER(WM_TRACKSTOPPED,OnStopped)
		MESSAGE_HANDLER(WM_GET_BAND_CLASSNAME,OnGetBandClassName)
		MESSAGE_HANDLER(WM_USER_TIMER,OnTimer)
	END_MSG_MAP()


	//双击状态栏,激活当前播放音轨
	void OnLButtonDblClk(UINT nFlags, CPoint point);

	void Init()
	{
// 		IWantToReceiveMessage(WM_NEW_TRACK_STARTED);
// 		IWantToReceiveMessage(WM_PAUSED);
// 		IWantToReceiveMessage(WM_TRACKSTOPPED);
// 		IWantToReceiveMessage(WM_PAUSE_START);

		IWantToReceiveMessage(WM_USER_TIMER);
	}

	void UpdateTrackInfoText();

	LRESULT OnGetBandClassName(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		_tcscpy((TCHAR*)wParam,_T("ComboBox"));
		return 0;
	}

	LRESULT OnPlay(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		IWantToReceiveMessage(WM_USER_TIMER);
		//UpdateTrackInfoText();

		return 0;
	}

	LRESULT OnResume(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		IWantToReceiveMessage(WM_USER_TIMER);
		return 0;
	}

	LRESULT OnPaused(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		IDonotWantToReceiveMessage(WM_USER_TIMER);
		return 0;
	}


	LRESULT OnStopped(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		SetText(0,NULL);
		IDonotWantToReceiveMessage(WM_USER_TIMER);
		return 0;
	}

	
	LRESULT OnTimer(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		UpdateTrackInfoText();
		return 0;
	}

	virtual void OnFinalMessage(_In_ HWND /*hWnd*/)
	{
// 		IDonotWantToReceiveMessage(WM_NEW_TRACK_STARTED);	
// 		IDonotWantToReceiveMessage(WM_PAUSED);	
// 		IDonotWantToReceiveMessage(WM_PAUSE_START);	
// 		IDonotWantToReceiveMessage(WM_TRACKSTOPPED);

		IDonotWantToReceiveMessage(WM_USER_TIMER);
	}
};


class CMyToolBar:public CWindowImpl<CMyToolBar,CToolBarCtrl>
{
public:
	BEGIN_MSG_MAP_EX(CMyToolBar)
		MSG_WM_RBUTTONDOWN(OnRButtonDown)
	END_MSG_MAP()

public:
	static BOOL bRegister;
	CMyToolBar()
	{
		if(bRegister==FALSE)
		{
			CMySimpleRebar::RegisterRebarBand(GetBandClassName(),CreateToolBarBand);
			bRegister=TRUE;
		}
	}

	static TCHAR *GetBandClassName()
	{
		return _T("按钮");
	}

	static HWND CreateToolBarBand(HWND parent)
	{
		CMyToolBar *tool=new CMyToolBar;
		return tool->CreateIsWnd(parent);
	}

	HWND CreateIsWnd(HWND parent);

	void OnRButtonDown(UINT nFlags, CPoint point)
	{
		::SetCapture(::GetParent(m_hWnd));
	}

	
};

class CMyComboBox:public CWindowImpl<CMyComboBox,CComboBox>
{
public:
	DECLARE_WND_SUPERCLASS(_T("MyComboBox"),CComboBox::GetWndClassName())

	BEGIN_MSG_MAP_EX(CMyComboBox)
		MSG_WM_RBUTTONDOWN(OnRButtonDown)
		MSG_WM_SETFOCUS(OnSetFocus)
		MESSAGE_HANDLER(WM_GET_BAND_CLASSNAME,OnGetBandClassName)
		REFLECTED_COMMAND_CODE_HANDLER_EX(CBN_CLOSEUP,OnCloseUp)
		REFLECTED_COMMAND_CODE_HANDLER_EX(CBN_SELCHANGE,OnCbnSelchanged)
	END_MSG_MAP()

public:
	static BOOL bRegister;
	CMyComboBox()
	{
		if(bRegister==FALSE)
		{
			CMySimpleRebar::RegisterRebarBand(GetBandClassName(),CreateCommoBoxBand);
			bRegister=TRUE;
		}
	}

	static TCHAR *GetBandClassName()
	{
		return _T("播放次序");
	}

	static HWND CreateCommoBoxBand(HWND parent)
	{
		CMyComboBox *combo=new CMyComboBox;
		return combo->CreateIsWnd(parent);
	}



	void OnRButtonDown(UINT nFlags, CPoint point)
	{
		::SetCapture(::GetParent(m_hWnd));
	}

	LRESULT OnGetBandClassName(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		_tcscpy((TCHAR*)wParam,_T("ComboBox"));
		return 0;
	}

	LRESULT OnCbnSelchanged(UINT,int id, HWND hWndCtl);


	HWND CreateIsWnd(HWND parent);

	LRESULT OnCloseUp(UINT uNotifyCode, int nID, CWindow wndCtl)
	{
		::SetFocus(m_hWndOld);
		return 0;
	}

	//we don't want a focus rect appear around
	HWND m_hWndOld;
	void OnSetFocus(CWindow wndOld)
	{
		m_hWndOld=wndOld;
		//::SetFocus(GetMainFrame()->m_hWnd);
		
		SetMsgHandled(FALSE);
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
ALT_MSG_MAP(1)
CHAIN_MSG_MAP_ALT(CEditCommands<CMyEdit>, 1)
END_MSG_MAP()

DialogSearch *m_pDlgSearch;
//void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
};
*/


class CMyCommandBarCtrl : public CCommandBarCtrlImpl<CMyCommandBarCtrl>
{
	typedef CCommandBarCtrlImpl<CMyCommandBarCtrl> theBase;
private:
	~CMyCommandBarCtrl()
	{
		//prevent object create from stack
	}
public:
	DECLARE_WND_SUPERCLASS(_T("WTL_CommandBar"), GetWndClassName())
	
	BEGIN_MSG_MAP(CMyCommandBarCtrl)
		MESSAGE_HANDLER(WM_RBUTTONDOWN,OnRButtonDown)
		CHAIN_MSG_MAP(theBase)
		ALT_MSG_MAP(1)
		CHAIN_MSG_MAP_ALT(theBase, 1)
		ALT_MSG_MAP(2)
		CHAIN_MSG_MAP_ALT(theBase, 2)
		ALT_MSG_MAP(3)
		CHAIN_MSG_MAP_ALT(theBase, 3)
	END_MSG_MAP()

	static BOOL bRegister;

	CMyCommandBarCtrl()
	{
		if(bRegister==FALSE)
		{
			CMySimpleRebar::RegisterRebarBand(GetBandClassName(),CreateCmdBarBand);
			bRegister=TRUE;
		}
	}


	static TCHAR *GetBandClassName()
	{
		return _T("菜单");
	}

	static HWND CreateCmdBarBand(HWND parent)
	{
		CMyCommandBarCtrl *cmdbar=new CMyCommandBarCtrl;
		return cmdbar->CreateIsWnd(parent);
	}

	void OnFinalMessage(_In_ HWND /*hWnd*/)
	{
		delete this;
	}

	LRESULT OnRButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		::SetCapture(::GetParent(m_hWnd));
		return TRUE;
	}

	HWND CreateIsWnd(HWND hParent)
	{
		Create(::GetParent(hParent), rcDefault, NULL, ATL_SIMPLE_CMDBAR_PANE_STYLE);
		LoadMenu(MAKEINTRESOURCE(IDR_MAINFRAME));
		LoadImages(IDR_MAINFRAME);
		return m_hWnd;
	}
};

class CMySplitterWindow:
	public CSplitterWindowImpl<CSplitterWindowT<true>, true>
{
public:
	typedef CSplitterWindowImpl<CSplitterWindowT<true>, true> _baseClass1;
	DECLARE_WND_CLASS_EX(_T("WTL_SplitterWindow"), CS_DBLCLKS,NULL)
	BEGIN_MSG_MAP(CMySplitterWindow)
		REFLECT_NOTIFICATIONS()
		CHAIN_MSG_MAP(_baseClass1)
	END_MSG_MAP()
};


