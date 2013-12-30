#include "stdafx.h"
#include "FunctionReg.h"
#include "MySerialize.h"
#include "MyControls.h"


HWND CreateTrackBand(HWND hWndParent);
HWND CreateVolumeBand(HWND hWndParent);
HWND CreateComboBand(HWND hWndParent);



class CMySimpleRebar:
	public CWindowImpl<CMySimpleRebar,CReBarCtrl>
	,public SerializeObj
{   
public:
	typedef CWindowImpl<CMySimpleRebar,CReBarCtrl> thebase;
	DECLARE_WND_SUPERCLASS(NULL,CReBarCtrl::GetWndClassName())

	BEGIN_MSG_MAP_EX(CMySimpleRebar)
		//MESSAGE_HANDLER(WM_NOTIFY,OnNotify)
		MESSAGE_HANDLER(WM_CTLCOLORSTATIC,OnCtrlColorStatic)
		MESSAGE_HANDLER(WM_HSCROLL,OnHscroll)//Reflect Scroll Message
		MSG_WM_RBUTTONDOWN(OnRButtonDown)
		MSG_WM_RBUTTONUP(OnRButtonUp)
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()

	int SerializeB(FILE *pFile);
	int ReSerialize(FILE *pFile);

	void OnRButtonDown(UINT nFlags, CPoint point)
	{
		SetCapture();

	}

	void OnRButtonUp(UINT nFlags, CPoint point)
	{
		if(GetCapture()==m_hWnd)
		{
			POINT pt=point;
			ClientToScreen(&pt);
			HMENU menu=::GetSubMenu(::LoadMenu(NULL,MAKEINTRESOURCE(IDR_MENU_REBAR)),0);
			::TrackPopupMenu(menu,TPM_LEFTALIGN,pt.x,pt.y,0,m_hWnd,0);

			ReleaseCapture();
		}
	}

	BOOL ReserializeRebarBand(REBARBANDINFO &info,TCHAR *szClassName)
	{
		//get the szClassName from file
		//TCHAR szClassName[128];


		HWND hWndBand=CreateRebarBand(szClassName);
		if(hWndBand!=NULL)
		{

		}

	}

	//copyed from atlframe.h
	static BOOL AddSimpleReBarBandCtrl(HWND hWndReBar, HWND hWndBand, int nID = 0, LPCTSTR lpstrTitle = NULL, BOOL bNewRow = FALSE, int cxWidth = 0, BOOL bFullWidthAlways = FALSE)
	{
		ATLASSERT(::IsWindow(hWndReBar));   // must be already created
#ifdef _DEBUG
		// block - check if this is really a rebar
		{
			TCHAR lpszClassName[sizeof(REBARCLASSNAME)] = { 0 };
			::GetClassName(hWndReBar, lpszClassName, sizeof(REBARCLASSNAME));
			ATLASSERT(lstrcmp(lpszClassName, REBARCLASSNAME) == 0);
		}
#endif // _DEBUG
		ATLASSERT(::IsWindow(hWndBand));   // must be already created

		// Get number of buttons on the toolbar
		int nBtnCount = (int)::SendMessage(hWndBand, TB_BUTTONCOUNT, 0, 0L);

		// Set band info structure
		REBARBANDINFO rbBand = { RunTimeHelper::SizeOf_REBARBANDINFO() };
#if (_WIN32_IE >= 0x0400)
		rbBand.fMask = RBBIM_CHILD | RBBIM_CHILDSIZE | RBBIM_STYLE | RBBIM_ID | RBBIM_SIZE | RBBIM_IDEALSIZE;
#else
		rbBand.fMask = RBBIM_CHILD | RBBIM_CHILDSIZE | RBBIM_STYLE | RBBIM_ID | RBBIM_SIZE;
#endif // !(_WIN32_IE >= 0x0400)
		if(lpstrTitle != NULL)
			rbBand.fMask |= RBBIM_TEXT;
		rbBand.fStyle = RBBS_CHILDEDGE;
#if (_WIN32_IE >= 0x0500)
		if(nBtnCount > 0)   // add chevron style for toolbar with buttons
			rbBand.fStyle |= RBBS_USECHEVRON;
#endif // (_WIN32_IE >= 0x0500)
		if(bNewRow)
			rbBand.fStyle |= RBBS_BREAK;

		rbBand.lpText = (LPTSTR)lpstrTitle;
		rbBand.hwndChild = hWndBand;
		if(nID == 0)   // calc band ID
			nID = ATL_IDW_BAND_FIRST + (int)::SendMessage(hWndReBar, RB_GETBANDCOUNT, 0, 0L);
		rbBand.wID = nID;

		// Calculate the size of the band
		BOOL bRet = FALSE;
		RECT rcTmp = { 0 };
		if(nBtnCount > 0)
		{
			bRet = (BOOL)::SendMessage(hWndBand, TB_GETITEMRECT, nBtnCount - 1, (LPARAM)&rcTmp);
			ATLASSERT(bRet);
			rbBand.cx = (cxWidth != 0) ? cxWidth : rcTmp.right;
			rbBand.cyMinChild = rcTmp.bottom - rcTmp.top;
			if(bFullWidthAlways)
			{
				rbBand.cxMinChild = rbBand.cx;
			}
			else if(lpstrTitle == NULL)
			{
				bRet = (BOOL)::SendMessage(hWndBand, TB_GETITEMRECT, 0, (LPARAM)&rcTmp);
				ATLASSERT(bRet);
				rbBand.cxMinChild = rcTmp.right;
			}
			else
			{
				rbBand.cxMinChild = 0;
			}
		}
		else	// no buttons, either not a toolbar or really has no buttons
		{
			bRet = ::GetWindowRect(hWndBand, &rcTmp);
			ATLASSERT(bRet);
			rbBand.cx = (cxWidth != 0) ? cxWidth : (rcTmp.right - rcTmp.left);
			rbBand.cxMinChild = bFullWidthAlways ? rbBand.cx : 0;
			rbBand.cyMinChild = rcTmp.bottom - rcTmp.top;
		}

#if (_WIN32_IE >= 0x0400)
		rbBand.cxIdeal = rbBand.cx;
#endif // (_WIN32_IE >= 0x0400)

		// Add the band
		LRESULT lRes = ::SendMessage(hWndReBar, RB_INSERTBAND, (WPARAM)-1, (LPARAM)&rbBand);
		if(lRes == 0)
		{
			ATLTRACE2(atlTraceUI, 0, _T("Failed to add a band to the rebar.\n"));
			return FALSE;
		}

#if (_WIN32_IE >= 0x0501)
		DWORD dwExStyle = (DWORD)::SendMessage(hWndBand, TB_GETEXTENDEDSTYLE, 0, 0L);
		::SendMessage(hWndBand, TB_SETEXTENDEDSTYLE, 0, dwExStyle | TBSTYLE_EX_HIDECLIPPEDBUTTONS);
#endif // (_WIN32_IE >= 0x0501)

		return TRUE;
	}

	BOOL AddSimpleReBarBand(HWND hWndBand,TCHAR *szClassName, LPCTSTR lpstrTitle = NULL, BOOL bNewRow = FALSE, int cxWidth = 0, BOOL bFullWidthAlways = FALSE)
	{
		ATLASSERT(::IsWindow(hWndBand));        // must be created
		return AddSimpleReBarBandCtrl(m_hWnd, hWndBand, 0, lpstrTitle, bNewRow, cxWidth, bFullWidthAlways);
	}


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