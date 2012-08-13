#include "PlayListView.h"
#include "WTLTabViewCtrl.h"
#include "mainfrm.h"


void OnMyCtrlEraseBkgnd(HWND hWnd,HDC hdc)
{
	RECT rc;
	HBRUSH newBrush,oldBrush;

	newBrush=::CreateSolidBrush(RGB(242,244,243));
	//newBrush=GetSysColorBrush(COLOR_WINDOWFRAME);
	GetClientRect(hWnd,&rc);
	oldBrush=(HBRUSH)::SelectObject(hdc,newBrush);
	::Rectangle(hdc,rc.left,rc.top,rc.right,rc.bottom);
	::SelectObject(hdc,oldBrush);
}


//-----------------------------------------
//progress pos track bar
class CMyTrackBar
	:public CWindowImpl<CMyTrackBar,CTrackBarCtrl>
	,public CCustomDraw<CMyTrackBar>
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
		MSG_WM_ERASEBKGND(OnEraseBkgnd)
		//CHAIN_MSG_MAP(CCustomDraw<CMyTrackBar>)
		
		NOTIFY_CODE_HANDLER(NM_CUSTOMDRAW, OnReflectedCustomDraw)
		ALT_MSG_MAP(1)
		REFLECTED_NOTIFY_CODE_HANDLER(NM_CUSTOMDRAW, OnReflectedCustomDraw)
	END_MSG_MAP()

	DWORD OnPrePaint(int /*idCtrl*/, LPNMCUSTOMDRAW lpNMCustomDraw)
	{
		NMCUSTOMDRAW nmcd = *(LPNMCUSTOMDRAW)lpNMCustomDraw;
		

		HBRUSH newBrush,oldBrush;
		
		RECT clientRC;
		GetClientRect(&clientRC);
		newBrush=::CreateSolidBrush(RGB(242,244,243));
		oldBrush=(HBRUSH)::SelectObject(nmcd.hdc,newBrush);
		::Rectangle(nmcd.hdc,clientRC.left,clientRC.top,clientRC.right,clientRC.bottom);
		::SelectObject(nmcd.hdc,oldBrush);
		//return CDRF_SKIPDEFAULT;
		return  CDRF_NOTIFYITEMDRAW | CDRF_NOTIFYPOSTPAINT;
		//return CDRF_DODEFAULT;
	}

	DWORD OnItemPrePaint(int /*idCtrl*/, LPNMCUSTOMDRAW lpNMCustomDraw)
	{
		NMCUSTOMDRAW nmcd = *(LPNMCUSTOMDRAW)lpNMCustomDraw;
	

		HBRUSH newBrush,oldBrush;
		//newBrush=::CreateSolidBrush(RGB(242,244,243));
		newBrush=GetSysColorBrush(COLOR_WINDOWFRAME);
		::SelectObject(nmcd.hdc,newBrush);
		::Rectangle(nmcd.hdc,nmcd.rc.left,nmcd.rc.top,nmcd.rc.right,nmcd.rc.bottom);
		return CDRF_SKIPDEFAULT;
	}


	
	LRESULT OnReflectedCustomDraw(int idCtrl,NMHDR* pNMHDR, BOOL) 
	{
		LRESULT result;
		LRESULT *pResult=&result;
		// for additional info, read beginning MSDN "Customizing a Control's Appearance Using Custom Draw" at
		// http://msdn.microsoft.com/library/default.asp?url=/library/en-us/shellcc/platform/commctls/custdraw/custdraw.asp
		// slider controls (which are actually called "trackbar" controls) are specifically discussed at
		// http://msdn.microsoft.com/library/default.asp?url=/library/en-us/shellcc/platform/commctls/trackbar/notifications/nm_customdraw_trackbar.asp

		NMCUSTOMDRAW nmcd = *(LPNMCUSTOMDRAW)pNMHDR;

		UINT drawStage = nmcd.dwDrawStage;
		UINT itemSpec = nmcd.dwItemSpec;

		switch ( drawStage )
		{
		case CDDS_PREPAINT:		// Before the paint cycle begins

			// most important of the drawing stages
			// must return CDRF_NOTIFYITEMDRAW or else we will not get further 
			// NM_CUSTOMDRAW notifications for this drawing cycle
			// we also return CDRF_NOTIFYPOSTPAINT so that we will get post-paint notifications

			*pResult = CDRF_NOTIFYITEMDRAW | CDRF_NOTIFYPOSTPAINT ;
			break;

		case CDDS_PREERASE:		// Before the erase cycle begins
		case CDDS_POSTERASE:	// After the erase cycle is complete
		case CDDS_ITEMPREERASE:	// Before an item is erased
		case CDDS_ITEMPOSTERASE:	// After an item has been erased

			// these are not handled now, but you might like to do so in the future

			*pResult = CDRF_DODEFAULT;
			break;

		case CDDS_ITEMPREPAINT:	// Before an item is drawn

			// this is where we perform our item-specific custom drawing


			switch ( itemSpec )
			{
			case TBCD_CHANNEL:	// channel that the trackbar control's thumb marker slides along
				// For the pre-item-paint of the channel, we simply tell the control to draw the default
				// and then tell us when it's done drawing the channel (i.e., item-post-paint) using
				// CDRF_NOTIFYPOSTPAINT.  In post-item-paint of the channel, we draw a simple 
				// colored highlight in the channel's recatngle

				// Frankly, when I returned CDRF_SKIPDEFAULT, in an attempt to skip drawing here
				// and draw everything in post-paint, the control seems to ignore the CDRF_SKIPDEFAULT flag,
				// and it seems to draw the channel even if we returned CDRF_SKIPDEFAULT

				*pResult = CDRF_DODEFAULT| CDRF_NOTIFYPOSTPAINT;
				break;

			case TBCD_TICS:		// the increment tick marks that appear along the edge of the trackbar control
				// currently, there is no special drawing of the  tics

				*pResult = CDRF_DODEFAULT;
				break;

			case TBCD_THUMB:	// trackbar control's thumb marker. This is the portion of the control that the user moves
				// For the pre-item-paint of the thumb, we draw everything completely here, during item 
				// pre-paint, and then tell the control to skip default painting and NOT to notify 
				// us during post-paint.
				// If I asked for a post-paint notification, then for some reason, when the control gets to 
				// post-paint, it completely over-writes everthing that I do here, and draws the default thumb
				// (which is partially obscured by the thumb drawn here).  This happens even if in post-paint
				// I return another CDRF_SKIPDEFAULT.  I don't understand why.  
				// Anyway, it works fine if I draw everthing here, return CDRF_SKIPDEFAULT, and do not ask for
				// a post-paint item notification
// 				
// 				{
// 					CDC* pDC = CDC::FromHandle( nmcd.hdc );
// 					int iSaveDC = pDC->SaveDC();
// 
// 					CBrush* pB = &m_normalBrush;
// 					CPen pen( PS_SOLID, 1, m_crShadow );
// 
// 					// if thumb is selected/focussed, switch brushes
// 
// 					if ( nmcd.uItemState && CDIS_FOCUS )
// 					{
// 						pB = &m_focusBrush;
// 
// 						pDC->SetBrushOrg( nmcd.rc.right%8, nmcd.rc.top%8 );
// 						pDC->SetBkColor( m_crPrimary );
// 						pDC->SetTextColor( m_crHilite );				
// 					}
// 
// 					pDC->SelectObject( pB );
// 					pDC->SelectObject( &pen );
// 
// #if 1	// draw an ellipse
// 
// 					pDC->Ellipse( &(nmcd.rc) );
// 
// #else	// draw a diamond
// 
// 					int xx, yy, dx, dy, cx, cy;
// 					xx = nmcd.rc.left;
// 					yy = nmcd.rc.top;
// 					dx = 2;
// 					dy = 2;
// 					cx = nmcd.rc.right - xx - 1;
// 					cy = nmcd.rc.bottom - yy - 1;
// 					POINT pts[8] = { {xx+dx, yy}, {xx, yy+dy}, {xx, yy+cy-dy}, {xx+dx, yy+cy},
// 					{xx+cx-dx, yy+cy}, {xx+cx, yy+cy-dy}, {xx+cx, yy+dy}, {xx+cx-dx, yy} };
// 
// 					pDC->Polygon( pts, 8 );
// 
// #endif	// which shape to draw
// 
// 					pDC->RestoreDC( iSaveDC );
// 					pDC->Detach();
// 				}
				*pResult = CDRF_SKIPDEFAULT;	// don't let control draw itself, or it will un-do our work
				break;

			default:;
				//ASSERT( FALSE );	// all of a slider's items have been listed, so we shouldn't get here
			};

			break;

		case CDDS_ITEMPOSTPAINT:	// After an item has been drawn

			switch ( itemSpec )
			{
			case TBCD_CHANNEL:	// channel that the trackbar control's thumb marker slides along
				// For the item-post-paint of the channel, we basically like what the control has drawn, 
				// which is a four-line high rectangle whose colors (in order) are white, mid-gray, black, 
				// and dark-gray.
				// However, to emphasize the control's color, we will replace the middle two lines
				// (i.e., the mid-gray and black lines) with hilite and shadow colors of the control
				// using CDC::Draw3DRect.
// 				{
// 					CDC* pDC = CDC::FromHandle( nmcd.hdc );
// 
// 					RECT rrc = {nmcd.rc.left+1, nmcd.rc.top+1, nmcd.rc.right-1, nmcd.rc.bottom-1};
// 					pDC->Draw3dRect( &rrc, m_crMidShadow, m_crHilite );
// 					pDC->Detach();
// 				}
				*pResult = CDRF_SKIPDEFAULT;
				break;

			case TBCD_TICS:		// the increment tick marks that appear along the edge of the trackbar control
				// currently, there is no special post-item-paint drawing of the tics

				*pResult = CDRF_DODEFAULT;
				break;

			case TBCD_THUMB:	// trackbar control's thumb marker. This is the portion of the control that the user moves
				// currently, there is no special post-item-paint drawing for the thumb

				*pResult = CDRF_DODEFAULT ;	// don't let control draw itself, or it will un-do our work
				break;

			default:;
				//ASSERT( FALSE );	// all of a slider's items have been listed, so we shouldn't get here
			};

			break;

		case CDDS_POSTPAINT:		// After the paint cycle is complete
			// this is the post-paint for the entire control, and it's possible to add to whatever is 
			// now visible on the control
			// To emphasize the directivity of the control, we simply draw in two colored dots at the 
			// extreme edges of the control
// 			{
// 				CDC* pDC = CDC::FromHandle( nmcd.hdc );
// 
// 				CBrush bWhite( RGB(255, 255, 255) );	// white brush
// 				CBrush bDark( m_crDarkerShadow );		// dark but still colored brush
// 				CPen p(PS_SOLID, 1, m_crPrimary);
// 
// 				CRect rClient;
// 				GetClientRect( &rClient );
// 				DWORD dwStyle = GetStyle();
// 
// 
// 				int cx = 8;
// 				CRect rrcFirst;( 1, 1, cx, cx );
// 				CRect rrcLast;
// 
// 				// TBS_RIGHT, TBS_BOTTOM and TBS_HORZ are all defined as 0x0000, so avoid testing on them
// 
// 				if ( dwStyle & TBS_VERT )
// 				{
// 					if ( dwStyle & TBS_LEFT )
// 					{
// 						rrcFirst = CRect( rClient.right-cx, 1, rClient.right-1, cx ); 
// 						rrcLast = CRect( rClient.right-cx, rClient.bottom-cx, rClient.right-1, rClient.bottom-1 );
// 					}
// 					else
// 					{
// 						rrcFirst = CRect( 1, 1, cx, cx ); 
// 						rrcLast = CRect( 1, rClient.bottom-cx, cx, rClient.bottom-1 );
// 					}
// 				}
// 				else
// 				{
// 					if ( dwStyle & TBS_TOP )
// 					{
// 						rrcFirst = CRect( 1, rClient.bottom-cx, cx, rClient.bottom-1 ); 
// 						rrcLast = CRect( rClient.right-cx, rClient.bottom-cx, rClient.right-1, rClient.bottom-1 );
// 					}
// 					else
// 					{
// 						rrcFirst = CRect( 1, 1, cx, cx ); 
// 						rrcLast = CRect( rClient.right-cx, 1, rClient.right-1, cx );
// 					}
// 				}
// 
// 				int iSave = pDC->SaveDC();
// 				pDC->SelectObject( &bWhite );
// 				pDC->SelectObject( &p );
// 				pDC->Ellipse( &rrcFirst );
// 				pDC->SelectObject( &bDark );
// 				pDC->Ellipse( &rrcLast );
// 				pDC->RestoreDC( iSave );
// 
// 				pDC->Detach();
// 			}

			*pResult = CDRF_SKIPDEFAULT;
			break;


		default:	;		
			//ASSERT( FALSE );	// all drawing stages are listed, so we shouldn't get here

		};

		return	 result;
	}


#if (_WIN32_IE >= 0x0400)
	DWORD OnSubItemPrePaint(int /*idCtrl*/, LPNMCUSTOMDRAW /*lpNMCustomDraw*/)
	{
		return CDRF_DODEFAULT;
	}
#endif // (_WIN32_IE >= 0x0400)



	BOOL OnEraseBkgnd(CDCHandle dc)
	{
		OnMyCtrlEraseBkgnd(m_hWnd,dc.m_hDC);
		SetMsgHandled(TRUE);
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
		rc->right=30;
		rc->bottom=30;
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
	END_MSG_MAP()


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

	void OnPos(int pos)
	{
		CBasicPlayer::shared()->SetVolumeByEar(pos);
	}



	LRESULT OnGetItemRect(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& bHandled)
	{
		RECT *rc=(RECT*)lParam;
		rc->left=0;
		rc->right=30;
		rc->bottom=30;
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


// 
// class CMySimpleRebar:public CWindowImpl<CMySimpleRebar,CReBarCtrl>
// {   
// // 
// // 	static ATL::CWndClassInfo& GetWndClassInfo1() 
// // 	{ 
// // 	/*
// // 	typedef struct tagWNDCLASSEXA {
// // 		UINT        cbSize;
// // 		UINT        style;
// // 		WNDPROC     lpfnWndProc;
// // 		int         cbClsExtra;
// // 		int         cbWndExtra;
// // 		HINSTANCE   hInstance;
// // 		HICON       hIcon;
// // 		HCURSOR     hCursor;
// // 		HBRUSH      hbrBackground;
// // 		LPCSTR      lpszMenuName;
// // 		LPCSTR      lpszClassName;
// // 		HICON       hIconSm;
// // 	} WNDCLASSEXA
// // 	*/
// // 
// // 		HBRUSH brush;
// // 		brush=GetSysColorBrush(COLOR_BTNFACE);
// // 		static ATL::CWndClassInfo wc = 
// // 		{ 
// // 			{ sizeof(WNDCLASSEX), 0, StartWindowProc, 
// // 			0, 0, NULL, NULL, NULL, brush, NULL, NULL, NULL }, \
// // 			NULL, NULL, NULL, TRUE, 0, _T("") 
// // 		}; 
// // 		return wc; 
// // 	}
// 
// 	DECLARE_WND_SUPERCLASS(NULL,CReBarCtrl::GetWndClassName())
// 
// 	BEGIN_MSG_MAP_EX(CMySimpleRebar)
// 		//NOTIFY_CODE_HANDLER_EX(RBN_LAYOUTCHANGED,LayOutChanged)
// 		//NOTIFY_CODE_HANDLER(RBN_ENDDRAG ,LayOutChanged)
// 		
// 		ALT_MSG_MAP(1)
// 		REFLECTED_NOTIFY_CODE_HANDLER(RBN_ENDDRAG, LayOutChanged)
// 	END_MSG_MAP()
// 
// 	LRESULT LayOutChanged(int idCtrl,NMHDR* pNMHDR, BOOL) 
//     //LRESULT LayOutChanged(LPNMHDR pnmh)
// 	{
// 		return 0;
// 	}
// };	




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