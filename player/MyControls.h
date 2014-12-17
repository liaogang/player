#pragma once
#include "forwardMsg.h"
#include "globalStuffs.h"
#include "MyRebar.h"
#include "BasicPlayer.h"



template<typename T>
class CMyRebarBandBase
{
private:

	/*
	enum
	{
		ENUM_REMOVEABLE = TRUE,
		ENUM_NEWROW = FALSE ,
		ENUM_WIDTH = 0 ,
		ENUM_FULLWIDTHALWAYS = TRUE ,
	};
	*/

// 	~CMyRebarBandBase()
// 	{
// 		//prevent object create from stack
// 	}
public:
	static void RegisterSelf()
	{
		CMySimpleRebar::RegisterRebarBand(
			T::GetBandClassName(),
			CreateRebarBand,
			T::ENUM_REMOVEABLE,
			T::ENUM_NEWROW,
			T::ENUM_WIDTH,
			T::ENUM_FULLWIDTHALWAYS);
	}

	static TCHAR *GetBandClassName()
	{
		ATLASSERT( FALSE );// must be implemented in a derived class
		return _T(" ");
	}

	static HWND CreateRebarBand(HWND parent)
	{
		T* pT = static_cast<T*>(new T);
		return pT->CreateIsWnd(parent);
	}

	HWND CreateIsWnd(HWND parent)
	{
		ATLASSERT( FALSE );// must be implemented in a derived class
		return NULL;
	}

// 	virtual void OnFinalMessage(_In_ HWND hWnd)
// 	{
// 		T* pT = static_cast<T*>(this);
// 		pT->OnFinalMessage(hWnd);
// 		delete pT;
// 	}
};





class CMyTrackBarBase
	:public CWindowImpl<CMyTrackBarBase,CTrackBarCtrl>
{
public:
	DECLARE_WND_SUPERCLASS(NULL,CTrackBarCtrl::GetWndClassName())

	BEGIN_MSG_MAP_EX(CMyTrackBarBase)
		MSG_WM_SETFOCUS(OnSetFocus)
		MESSAGE_HANDLER(WM_ERASEBKGND,OnEraseBkgnd)
		MESSAGE_HANDLER(WM_LBUTTONDOWN,OnLButtonDown)
		MSG_WM_RBUTTONDOWN(OnRButtonDown)
	END_MSG_MAP()



	void OnRButtonDown(UINT nFlags, CPoint point)
	{
		::SetCapture(::GetParent(m_hWnd));
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
	,public CCustomDraw< CMyTrackBar >
	,public CMyRebarBandBase< CMyTrackBar >
{
public:
	typedef  CMyTrackBarBase baseclass;

	static TCHAR *GetBandClassName()
	{
		return _T("��λ");
	}

	enum
	{
		ENUM_REMOVEABLE = TRUE,
		ENUM_NEWROW = FALSE ,
		ENUM_WIDTH = 280 ,
		ENUM_FULLWIDTHALWAYS = TRUE ,
	};
public:
	DECLARE_WND_SUPERCLASS(NULL,CMyTrackBarBase::GetWndClassName())

	BEGIN_MSG_MAP_EX(CMyTrackBar)
		MSG_WM_DESTROY(OnDestroy)

		//user message
		MESSAGE_HANDLER(WM_NEW_TRACK_STARTED,OnNewTrackStarted)
		MESSAGE_HANDLER(WM_PAUSED,OnPaused)
		MESSAGE_HANDLER(WM_PAUSE_START,OnPauseStarted)
		MESSAGE_HANDLER(WM_TRACKSTOPPED,OnTrackStopped)
		MESSAGE_HANDLER(WM_TRACK_POS_CHANGED,OnTrackPosChanged)
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
				UpdatePos();
		}
	}


	CToolTipCtrl m_CtrlTooltip;
	HWND CreateIsWnd(HWND parent);
	
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
	static const int m_uElapse=800;
	LRESULT OnNewTrackStarted(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
	{
		EnableWindow(TRUE);

		
		SetMaxRange();
		UpdateTime();
		UpdatePos();

		SetTimer((UINT_PTR)&m_nIDEvent,m_uElapse,NULL);

		m_bPaused=false;
		m_bStopped=false;
		
		return 0;
	}

	void UpdatePos()
	{
		SetPos(m_uCurrTime / 1000);
	}

	void UpdateTime()
	{
		m_uCurrTime=CBasicPlayer::shared()->MillisecondsPlayed();
	}

	void SetMaxRange()
	{
		double totalSec=CBasicPlayer::shared()->MillisecondsTotal()/1000 ;
		int iTotalSec=totalSec;
		if(totalSec - iTotalSec > 0)
			++iTotalSec;

		SetRange(0,iTotalSec);
	}
	

	LRESULT OnTrackPosChanged(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		UpdateTime();
		UpdatePos();
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
		UpdateTime();
		UpdatePos();
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
	public CMyTrackBarBase,
	public CCustomDraw<CMyVolumeBar>,
	public CMyRebarBandBase<CMyVolumeBar>
{
public:
	static TCHAR *GetBandClassName()
	{
		return _T("��������");
	}

	enum
	{
		ENUM_REMOVEABLE = TRUE,
		ENUM_NEWROW = FALSE ,
		ENUM_WIDTH = 70 ,
		ENUM_FULLWIDTHALWAYS = TRUE ,
	};

	HWND CreateIsWnd(HWND parent)
	{
		CMyTrackBarBase::CreateIsWnd(parent);
		Init();
		return m_hWnd;
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




class CMyToolBar:
	public CWindowImpl<CMyToolBar,CToolBarCtrl>,
	public CMyRebarBandBase<CMyToolBar>
{
public:
	BEGIN_MSG_MAP_EX(CMyToolBar)
		MSG_WM_RBUTTONDOWN(OnRButtonDown)
	END_MSG_MAP()

	static TCHAR *GetBandClassName()
	{
		return _T("��ť");
	}

	enum
	{
		ENUM_REMOVEABLE = TRUE,
		ENUM_NEWROW = TRUE ,
		ENUM_WIDTH = 0 ,
		ENUM_FULLWIDTHALWAYS = TRUE ,
	};

	HWND CreateIsWnd(HWND parent);

	void OnRButtonDown(UINT nFlags, CPoint point)
	{
		::SetCapture(::GetParent(m_hWnd));
	}
	
};

class CMyComboBox:
	public CWindowImpl<CMyComboBox,CComboBox>,
	public CMyRebarBandBase<CMyComboBox>
{
public:
	DECLARE_WND_SUPERCLASS(_T("MyComboBox"),CComboBox::GetWndClassName())

	BEGIN_MSG_MAP_EX(CMyComboBox)
		MSG_WM_RBUTTONDOWN(OnRButtonDown)
		MSG_WM_SETFOCUS(OnSetFocus)
		REFLECTED_COMMAND_CODE_HANDLER_EX(CBN_CLOSEUP,OnCloseUp)
		REFLECTED_COMMAND_CODE_HANDLER_EX(CBN_SELCHANGE,OnCbnSelchanged)
	END_MSG_MAP()

	enum
	{
		ENUM_REMOVEABLE = TRUE,
		ENUM_NEWROW = FALSE ,
		ENUM_WIDTH = 120 ,
		ENUM_FULLWIDTHALWAYS = TRUE ,
	};

	static TCHAR *GetBandClassName()
	{
		return _T("���Ŵ���");
	}

	void OnRButtonDown(UINT nFlags, CPoint point)
	{
		::SetCapture(::GetParent(m_hWnd));
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


class CMyCommandBarCtrl :
	public CCommandBarCtrlImpl< CMyCommandBarCtrl >,
	public CMyRebarBandBase< CMyCommandBarCtrl >
{
	typedef CCommandBarCtrlImpl<CMyCommandBarCtrl> theBase;
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

	static TCHAR *GetBandClassName()
	{
		return _T("�˵�");
	}

	enum
	{
		ENUM_REMOVEABLE = FALSE,
		ENUM_NEWROW = TRUE ,
		ENUM_WIDTH = 0 ,
		ENUM_FULLWIDTHALWAYS = TRUE ,
	};

	void OnFinalMessage(_In_ HWND /*hWnd*/)
	{
		//delete this;
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

// class CMySplitterWindow:
// 	public CSplitterWindowImpl<CSplitterWindowT<true>, true>
// {
// public:
// 	typedef CSplitterWindowImpl<CSplitterWindowT<true>, true> _baseClass1;
// 	DECLARE_WND_CLASS_EX(_T("WTL_SplitterWindow"), CS_DBLCLKS,NULL)
// 	BEGIN_MSG_MAP(CMySplitterWindow)
// 		REFLECT_NOTIFICATIONS()
// 		CHAIN_MSG_MAP(_baseClass1)
// 	END_MSG_MAP()
// };





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
		MESSAGE_HANDLER(WM_USER_TIMER,OnTimer)
	END_MSG_MAP()


	//˫��״̬��,���ǰ��������
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