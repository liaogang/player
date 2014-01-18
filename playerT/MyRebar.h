#include "stdafx.h"
#include "FunctionReg.h"
#include "MySerialize.h"



HWND CreateTrackBand(HWND hWndParent);
HWND CreateVolumeBand(HWND hWndParent);
HWND CreateComboBand(HWND hWndParent);


#define ID_REBAR_BASE 42948
struct MY_REBARBANDINFO 
{
public:
	REBARBANDINFO info;
	UINT menuID;
	CreateReBarBandFuns createFunc;
	TCHAR szClassName[MAX_PATH];
	BOOL bShow;
};
 
	
class CMyTrackBar;
class CMyVolumeBar;

class CMySimpleRebar:
	public CWindowImpl<CMySimpleRebar,CReBarCtrl>
	,public SerializeObj<CMySimpleRebar>
{   
public:
	typedef CWindowImpl<CMySimpleRebar,CReBarCtrl> thebase;
	DECLARE_WND_SUPERCLASS(NULL,CReBarCtrl::GetWndClassName())

	BEGIN_MSG_MAP_EX(CMySimpleRebar)
		//MSG_WM_ERASEBKGND(OnEraseBkgnd)
		//MESSAGE_HANDLER(WM_NOTIFY,OnNotify)
		COMMAND_ID_HANDLER(ID_REBAR_LOCK,OnLockBands)
		COMMAND_RANGE_HANDLER(ID_REBAR_BASE,ID_REBAR_BASE+m_vecBandInfos.size(),OnShowBandX)
		MESSAGE_HANDLER(WM_CTLCOLORSTATIC,OnCtrlColorStatic)
		MESSAGE_HANDLER(WM_HSCROLL,OnHscroll)//Reflect Scroll Message
		MSG_WM_RBUTTONDOWN(OnRButtonDown)
		MSG_WM_RBUTTONUP(OnRButtonUp)
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()

public:
	HMENU menu;
	BOOL m_bLock;
	CMyTrackBar *pTrack;
	CMyVolumeBar *pVolume;

	static vector<MY_REBARBANDINFO*> m_vecBandInfos;

public:
	FILE& operator<<(FILE& f);
	FILE& operator>>(FILE& f);

	CMySimpleRebar():m_bLock(FALSE),pTrack(NULL),pVolume(NULL)
	{
		menu=::GetSubMenu(::LoadMenu(NULL,MAKEINTRESOURCE(IDR_MENU_REBAR)),0);
	}

	static void RegisterRebarBand(TCHAR* szBandClassName,CreateReBarBandFuns func)
	{
		MY_REBARBANDINFO *rbBand = static_cast<MY_REBARBANDINFO*>(malloc(sizeof(MY_REBARBANDINFO)));
		_tcscpy(rbBand->szClassName,szBandClassName);
		rbBand->createFunc=func;
		
		m_vecBandInfos.push_back(rbBand);
	}

	static UINT MenuID2BandID(UINT menuID)
	{
		return MenuID2BandInfo(menuID)->info.wID;
	}

	static MY_REBARBANDINFO* MenuID2BandInfo(UINT menuID)
	{
		for(auto i=m_vecBandInfos.begin();i!=m_vecBandInfos.end();++i)
		{
			MY_REBARBANDINFO *mri=*i;
			if(mri->menuID==menuID)
				return mri;
		}

		ATLASSERT(FALSE);
		return NULL;
	}

	static MY_REBARBANDINFO* ClassName2BandInfo(TCHAR *className)
	{
		for(auto i=m_vecBandInfos.begin();i!=m_vecBandInfos.end();++i)
		{
			MY_REBARBANDINFO *mri=*i;
			if(_tcscmp(mri->szClassName,className)==0)
				return mri;
		}

		ATLASSERT(FALSE);
		return NULL;
	}

	void OnRButtonDown(UINT nFlags, CPoint point)
	{
		SetCapture();
	}



	void OnRButtonUp(UINT nFlags, CPoint point)
	{
		if(GetCapture()==m_hWnd)
		{
			ReleaseCapture();

			POINT pt=point;
			ClientToScreen(&pt);
			
			::CheckMenuItem(menu,ID_REBAR_LOCK,(m_bLock?MF_CHECKED:MF_UNCHECKED)|MF_BYCOMMAND);


			static BOOL bRebarMenu=0;
			if(bRebarMenu==0)//Rebar's menu added?
			{
				for(auto i=m_vecBandInfos.begin();i!=m_vecBandInfos.end();++i)
				{
					MY_REBARBANDINFO *mri=*i;
					
					TCHAR *szBandClassNames=mri->szClassName;
					
					MENUITEMINFO mi;
					mi.cbSize=sizeof(mi);
					mi.fMask=MIIM_ID|MIIM_STRING;
					mi.fState=MFS_ENABLED;
					mi.wID=mri->menuID=ID_REBAR_BASE+bRebarMenu++;
					mi.dwTypeData=szBandClassNames;
					mi.cch=_tcslen(szBandClassNames);


					InsertMenuItem(menu,2,TRUE,&mi);
				}
			}

			for(auto i=m_vecBandInfos.begin();i!=m_vecBandInfos.end();++i)
			{
				MY_REBARBANDINFO *mri=*i;
				::CheckMenuItem(menu,mri->menuID,(mri->bShow?MF_CHECKED:MF_UNCHECKED)|MF_BYCOMMAND);
			}
			
			::TrackPopupMenu(menu,TPM_LEFTALIGN,pt.x,pt.y,0,m_hWnd,0);
		}
	}

	HWND CreateIsWnd(HWND hParent);

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

		BOOL bRet= AddSimpleReBarBandCtrl(m_hWnd, hWndBand, 0, lpstrTitle, bNewRow, cxWidth, bFullWidthAlways);
		
		MY_REBARBANDINFO *info= ClassName2BandInfo(szClassName);
		info->info.cbSize=sizeof(REBARBANDINFO);
		info->info.fMask=RBBIM_ID;
		GetBandInfo(GetBandCount()-1,&info->info);
		info->bShow=TRUE;

		return bRet;
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


	//LRESULT OnNotify(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled);


	LRESULT OnCtrlColorStatic(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	void Init()
	{
		LockBands(m_bLock);
	}

	LRESULT OnLockBands(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		LockBands(m_bLock=!m_bLock);

		return 0;
	}


	LRESULT OnShowBandX(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		MY_REBARBANDINFO *mrbi= MenuID2BandInfo(wID);

		//if is existed
		UINT rebarID=mrbi->info.wID;
		mrbi->bShow=!mrbi->bShow;
		ShowBand(IdToIndex(rebarID), mrbi->bShow);

		//if not existed ,add it then.
		//todo


		return 0;
	}
	

	BOOL OnEraseBkgnd(HDC dc)
	{
		RECT rc;
		GetClientRect(&rc);

		HBRUSH hBrushB1=::CreateSolidBrush(RGB(242,247,253));
		HBRUSH hBrushB2=::CreateSolidBrush(RGB(235,243,252));
		HBRUSH hBrushB3=::CreateSolidBrush(RGB(227,238,251));
		HPEN  hPen=::CreatePen(PS_NULL,0,0);
		HBRUSH oldBrush;
		HPEN oldPen;
		int height=rc.bottom-rc.top;
		

		oldPen=(HPEN)::SelectObject(dc,hPen);
		oldBrush=(HBRUSH)::SelectObject(dc,hBrushB1);
		::Rectangle(dc,rc.left,rc.top,rc.right,rc.top+10);
		::SelectObject(dc,hBrushB2);
		::Rectangle(dc,rc.left,rc.top+10,rc.right,rc.top+13);
		::SelectObject(dc,hBrushB3);
		::Rectangle(dc,rc.left,rc.top+13,rc.right,rc.bottom);

		::SelectObject(dc,oldBrush);
		::SelectObject(dc,oldPen);

		DeleteObject(hBrushB1);
		DeleteObject(hBrushB2);
		DeleteObject(hBrushB3);

		return TRUE;
	}
};	