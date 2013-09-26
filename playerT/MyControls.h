
#include "WTLTabViewCtrl.h"
#include "globalStuffs.h"
#include "WuLines.h"
#include "BasicPlayer.h"
//-----------------------------------------
//progress pos track bar
class CMyTrackBarBase
	:public CWindowImpl<CMyTrackBarBase,CTrackBarCtrl>
{
public:
	DECLARE_WND_SUPERCLASS(NULL,CTrackBarCtrl::GetWndClassName())

	BEGIN_MSG_MAP_EX(CMyTrackBarBase)
		MSG_WM_SETFOCUS(OnSetFocus)
		MESSAGE_HANDLER(WM_ERASEBKGND,OnEraseBkgnd)
		MESSAGE_HANDLER(WM_LBUTTONDOWN,OnLButtonDown)
	END_MSG_MAP()




	BOOL m_bPressing;
	LRESULT OnLButtonDown(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
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


	HWND CreateIsWnd(HWND parent)
	{
		UINT style=WS_CHILD  | WS_VISIBLE /*| WS_CLIPCHILDREN | WS_CLIPSIBLINGS*/;
		style|=TBS_TOOLTIPS  |TBS_NOTICKS /*|TBS_AUTOTICKS */ | TBS_BOTH ; 
		UINT styleEx=0; 
		Create( parent,NULL,NULL,style,styleEx);

		SetPageSize(1);
		SetLineSize(1);
		SetThumbLength(30);

		EnableWindow(FALSE);




		m_bPressing=FALSE;

		return m_hWnd;
	}

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
	
	CMyTrackBar()
	{

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
		CHAIN_MSG_MAP_ALT(CCustomDraw<CMyTrackBar>, 1)
	END_MSG_MAP()

	LRESULT OnMouseReleased(int /*idCtrl*/, LPNMHDR pnmh, BOOL& /*bHandled*/);





	bool m_bPaused;
	bool m_bStopped;

	UINT m_uCurrTime;
	void OnTimer(UINT_PTR /*nIDEvent*/)
	{
		if(!m_bPaused)
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
		SetPos((int)0);


		SetTimer((UINT_PTR)&m_nIDEvent,m_uElapse,NULL);

		m_bPaused=false;
		m_bStopped=false;
		m_uCurrTime=0;

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
	CMainFrame *pMain;
	BOOL m_bPressing;
	CMyVolumeBar():m_bPressing(FALSE)
	{

	}
public:
	DECLARE_WND_SUPERCLASS(NULL,CMyTrackBarBase::GetWndClassName())

	BEGIN_MSG_MAP(CMyVolumeBar)
		MESSAGE_HANDLER(WM_CREATE,OnCreate)
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

	DWORD OnItemPrePaint(int /*idCtrl*/, LPNMCUSTOMDRAW pNMCD)
	{
		LRESULT lr = CDRF_DODEFAULT;


		//draw a triangle channel bar
		if(pNMCD->dwItemSpec == TBCD_CHANNEL)
		{
			CDC dc;
			dc.Attach(pNMCD->hdc);
			
			CRect r;
			GetClientRect(r);
			r.DeflateRect(8, 4, 10, 6);
			CopyRect(&pNMCD->rc, &r);
			
			CPen shadow;
				shadow.CreatePen(PS_SOLID, 1, GetSysColor(COLOR_3DSHADOW));
			CPen light;
				light.CreatePen(PS_SOLID, 1, GetSysColor(COLOR_3DHILIGHT));
			CPen old= dc.SelectPen(light);

			dc.MoveTo(pNMCD->rc.right, pNMCD->rc.top);
			dc.LineTo(pNMCD->rc.right, pNMCD->rc.bottom);
			dc.LineTo(pNMCD->rc.left, pNMCD->rc.bottom);
			dc.SelectPen(shadow);
			//dc.LineTo(pNMCD->rc.right, pNMCD->rc.top);

			DrawWuLine(&dc,pNMCD->rc.left,pNMCD->rc.bottom,pNMCD->rc.right,pNMCD->rc.top,GetSysColor(COLOR_3DSHADOW));

			dc.SelectPen(old);

			dc.Detach();
			
			

			/*
			RECT rc=pNMCD->rc;
			HDC  hdc=pNMCD->hdc;
			DWORD clrLight=GetSysColor(COLOR_3DFACE);

			TRIVERTEX vertex[3];
			vertex[0].x=rc.left;
			vertex[0].y=rc.bottom;
			vertex[0].Red=GetRValue(clrLight);
			vertex[0].Green=GetGValue(clrLight);
			vertex[0].Blue=GetBValue(clrLight);
			vertex[0].Alpha = 0x0000;

			vertex[1].x=rc.right;
			vertex[1].y=rc.top;
			vertex[1].Alpha = 0x0000;
			vertex[1].Red=GetRValue(clrLight);
			vertex[1].Green=GetGValue(clrLight);
			vertex[1].Blue=GetBValue(clrLight);


			vertex[2].x=rc.right;
			vertex[2].y=rc.bottom;
			vertex[2].Red=GetRValue(clrLight);
			vertex[2].Green=GetGValue(clrLight);
			vertex[2].Blue=GetBValue(clrLight);
			vertex[2].Alpha = 0x0000;

			GRADIENT_TRIANGLE gTriangle;
			gTriangle.Vertex1 = 0;
			gTriangle.Vertex2 = 1;
			gTriangle.Vertex3 = 2;
			GradientFill(hdc, vertex, 3, &gTriangle, 1, GRADIENT_FILL_TRIANGLE);
			*/



			lr = CDRF_SKIPDEFAULT;
		}


		pNMCD->uItemState &= ~CDIS_FOCUS;

		return lr;
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



	LRESULT OnMouseMove(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
	{    
		int pos=GetPos();
		OnPos(pos);

		bHandled=FALSE;
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


	void Init()
	{
		SetPageSize(1);
		SetLineSize(1);
		SetThumbLength(2);
		SetRange(0,100);
		SetPos(CBasicPlayer::shared()->m_curVolume);
	}

};


class CMyStatusBar:public CWindowImpl<CMyStatusBar,CStatusBarCtrl>
{
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

	//Ë«»÷×´Ì¬À¸,¼¤»îµ±Ç°²¥·ÅÒô¹ì
	void OnLButtonDblClk(UINT nFlags, CPoint point);
#ifdef _DEBUG
	int bInit;
#endif
	void Init()
	{
#ifdef _DEBUG
		bInit=1;
#endif

// 		RECT rc;
// 		GetClientRect(&rc);
// 
// 		int width[]={rc.right-rc.left-260,-1};
// 		SetParts(sizeof(width)/sizeof(width[0]),width);


// 		IWantToReceiveMessage(WM_NEW_TRACK_STARTED);
// 		IWantToReceiveMessage(WM_PAUSED);
// 		IWantToReceiveMessage(WM_TRACKSTOPPED);
// 		IWantToReceiveMessage(WM_PAUSE_START);

		IWantToReceiveMessage(WM_USER_TIMER);
	}

	void UpdateTrackInfoText();

	

	LRESULT OnPlay(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		//UpdateTrackInfoText();

		return 0;
	}

	LRESULT OnPaused(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		return 0;
	}

	LRESULT OnResume(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		return 0;
	}

	LRESULT OnStopped(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
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



class CMyComboBox:public CWindowImpl<CMyComboBox,CComboBox>
{
public:
	DECLARE_WND_SUPERCLASS(_T("MyComboBox"),CComboBox::GetWndClassName())

	BEGIN_MSG_MAP(CMyComboBox)
		MSG_WM_SETFOCUS(OnSetFocus)
		REFLECTED_COMMAND_CODE_HANDLER_EX(CBN_CLOSEUP,OnCloseUp)
		REFLECTED_COMMAND_CODE_HANDLER_EX(CBN_SELCHANGE,OnCbnSelchanged)
	END_MSG_MAP()

public:
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

class CMySimpleRebar:public CWindowImpl<CMySimpleRebar,CReBarCtrl>
{   
public:
	typedef CWindowImpl<CMySimpleRebar,CReBarCtrl> thebase;
	DECLARE_WND_SUPERCLASS(NULL,CReBarCtrl::GetWndClassName())

	BEGIN_MSG_MAP_EX(CMySimpleRebar)
		//MESSAGE_HANDLER(WM_NOTIFY,OnNotify)
		MESSAGE_HANDLER(WM_CTLCOLORSTATIC,OnCtrlColorStatic)
		MESSAGE_HANDLER(WM_HSCROLL,OnHscroll)//Reflect Scroll Message
		
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()

	LRESULT ReflectNotifications(
		_In_ UINT uMsg,
		_In_ WPARAM wParam,
		_In_ LPARAM lParam,
		_Inout_ BOOL& bHandled)
	{
		//filter the Wm_CtrlColorStatic message , because the combobox will not display fine.
		if(uMsg == WM_CTLCOLORLISTBOX)
		{
			bHandled=TRUE;
			return ::DefWindowProc(m_hWnd,uMsg,wParam,lParam);
		}
		else
			return thebase::ReflectNotifications(uMsg,wParam,lParam,bHandled);
	}


	LRESULT OnHscroll(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		::SendMessage((HWND)lParam,WM_HSCROLL,wParam,NULL);
		bHandled=FALSE;
		return 0;
	}


	LRESULT OnNotify(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled);



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
	DECLARE_WND_CLASS_EX(_T("WTL_SplitterWindow"), CS_DBLCLKS,NULL)
	BEGIN_MSG_MAP(CMySplitterWindow)
		REFLECT_NOTIFICATIONS()
		CHAIN_MSG_MAP(_baseClass1)
	END_MSG_MAP()
};