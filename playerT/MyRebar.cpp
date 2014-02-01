#include "MyRebar.h"
using namespace std;
vector<MY_REBARBANDINFO*> CMySimpleRebar::m_vecBandInfos;

CMySimpleRebar::CMySimpleRebar():m_bLock(FALSE),
	bInitFromFile(FALSE)
{
	menu=::GetSubMenu(::LoadMenu(NULL,MAKEINTRESOURCE(IDR_MENU_REBAR)),0);
}

void CMySimpleRebar::RegisterRebarBand(  TCHAR* szBandClassName,
	CreateReBarBandFuns func,
	BOOL bRemovable ,
	BOOL bNewRow  , 
	int cxWidth  ,
	BOOL bFullWidthAlways )
{
	MY_REBARBANDINFO *rbBand = static_cast<MY_REBARBANDINFO*>(malloc(sizeof(MY_REBARBANDINFO)));
	memset(rbBand,0,sizeof(MY_REBARBANDINFO));
	_tcscpy(rbBand->szClassName,szBandClassName);
	rbBand->createFunc=func;
	rbBand->info.cbSize=sizeof(REBARBANDINFO);
	rbBand->bRemovable=bRemovable;		
 	if(bNewRow)
 		rbBand->info.fStyle=RBBS_BREAK;
	rbBand->info.cx=cxWidth;		
	rbBand->bFullWidthAlways=bFullWidthAlways;

	m_vecBandInfos.push_back(rbBand);
}

UINT CMySimpleRebar::MenuID2BandID(UINT menuID)
{
	return MenuID2BandInfo(menuID)->info.wID;
}


MY_REBARBANDINFO* CMySimpleRebar::MenuID2BandInfo(UINT menuID)
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

MY_REBARBANDINFO* CMySimpleRebar::ClassName2BandInfo(TCHAR *className)
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


void CMySimpleRebar::OnRButtonDown(UINT nFlags, CPoint point)
{
	SetCapture();
}



void CMySimpleRebar::OnRButtonUp(UINT nFlags, CPoint point)
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
			//add to menu item reversely.
			for(auto i=m_vecBandInfos.end()-1;;--i)
			{
				MY_REBARBANDINFO *mri=*i;
				if(mri->bRemovable)
				{
					MENUITEMINFO mi;
					mi.cbSize=sizeof(mi);
					mi.fMask=MIIM_ID|MIIM_STRING;
					mi.fState=MFS_ENABLED;
					mi.wID=mri->menuID=ID_REBAR_BASE+bRebarMenu++;
					mi.dwTypeData=mri->szClassName;
					mi.cch=_tcslen(mri->szClassName);

					InsertMenuItem(menu,2,TRUE,&mi);
				}

				if(i==m_vecBandInfos.begin()) break;
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


BOOL CMySimpleRebar::AddSimpleReBarBandCtrl(MY_REBARBANDINFO *mri)
{
	BOOL bRet = AddSimpleReBarBandCtrl(m_hWnd, 
		mri->createFunc(m_hWnd) 
		, 0,
		mri->info.lpText,
		mri->info.fStyle&RBBS_BREAK, 
		mri->info.cx,
		mri->info.cxMinChild,
		mri->bFullWidthAlways);

	mri->info.fMask=RBBIM_ID|RBBIM_SIZE|RBBIM_STYLE|RBBIM_TEXT|RBBIM_ID|RBBIM_CHILDSIZE;
	GetBandInfo(GetBandCount()-1,&mri->info);
	mri->bAdded=bRet;

	return bRet;
}

//copyed from atlframe.h
BOOL CMySimpleRebar::AddSimpleReBarBandCtrl(HWND hWndReBar, HWND hWndBand, int nID , LPCTSTR lpstrTitle , BOOL bNewRow , int cxWidth , int cxMinChild ,BOOL bFullWidthAlways)
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
	rbBand.cxMinChild=cxMinChild;
	// Calculate the size of the band
	BOOL bRet = FALSE;
	RECT rcTmp = { 0 };
	if(nBtnCount > 0)
	{
		bRet = (BOOL)::SendMessage(hWndBand, TB_GETITEMRECT, nBtnCount - 1, (LPARAM)&rcTmp);
		ATLASSERT(bRet);
		rbBand.cx = (cxWidth != 0) ? cxWidth : rcTmp.right;
		rbBand.cyMinChild = rcTmp.bottom - rcTmp.top;
		
		if(rbBand.cxMinChild==0)
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
		if(rbBand.cxMinChild==0)
			rbBand.cxMinChild = bFullWidthAlways ? rbBand.cx : 0;
		
		rbBand.cyMinChild = rcTmp.bottom - rcTmp.top;
	}

#if (_WIN32_IE >= 0x0400)
	if(cxMinChild!=0)
		rbBand.cxIdeal=cxMinChild;
	else
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


LRESULT CMySimpleRebar::ReflectNotifications(
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


LRESULT CMySimpleRebar::OnHscroll(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	::SendMessage((HWND)lParam,WM_HSCROLL,wParam,NULL);
	bHandled=FALSE;
	return 0;
}


LRESULT CMySimpleRebar::OnCtrlColorStatic(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LRESULT lResult=FALSE;
	HDC dc=(HDC)wParam;
	HWND wnd=(HWND)lParam;

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

	return lResult;
}


LRESULT CMySimpleRebar::OnLockBands(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	LockBands(m_bLock=!m_bLock);

	return 0;
}


LRESULT CMySimpleRebar::OnResetBands(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{	
	ResetBands();

	return 0;
}

void CMySimpleRebar::ResetBands(BOOL bAddAllBands)
{
	//Only add the removable bands
	for(auto i=m_vecBandInfos.begin();i!=m_vecBandInfos.end();++i)
		if((*i)->bRemovable)
			DeleteBand(IdToIndex( (*i)->info.wID ) );
	

	//Only add the removable bands.
	//But if is the first time , add it all .
	for(auto i=m_vecBandInfos.begin();i!=m_vecBandInfos.end();++i)
		if(bAddAllBands || (*i)->bRemovable  )
			(*i)->bShow=AddSimpleReBarBandCtrl(*i);
	
	if(m_bLock)
		LockBands(TRUE);
}

LRESULT CMySimpleRebar::OnShowBandX(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	MY_REBARBANDINFO *mrbi= MenuID2BandInfo(wID);

	//if is existed
	UINT rebarID=mrbi->info.wID;
	if(mrbi->bAdded)
	{
		if(ShowBand(IdToIndex(rebarID), !mrbi->bShow))
			mrbi->bShow=!mrbi->bShow;
	}
	//if not existed ,add it then.
	else	
		mrbi->bShow=AddSimpleReBarBandCtrl(mrbi);
	
	return 0;
}



void CMySimpleRebar::CollectInfo()
{
	for(auto i=m_vecBandInfos.begin();i!=m_vecBandInfos.end();++i)
	{
		MY_REBARBANDINFO *mri=*i;
		UINT rebarID=mri->info.wID;
		mri->info.fMask=RBBIM_SIZE;
		mri->bandIndex=IdToIndex(rebarID);
		ATLASSERT(mri->bandIndex >=0 );
		GetBandInfo(mri->bandIndex,&mri->info);

		BOOL bNewLine;
		GetRect(mri->bandIndex,&mri->rc);
		bNewLine = mri->rc.left < 4 ? TRUE : FALSE;
		if(bNewLine)
			mri->info.fStyle |= RBBS_BREAK;
		else
			mri->info.fStyle &= ~RBBS_BREAK;
	}
}




void CMySimpleRebar::Init()
{
	if(bInitFromFile)
	{
		//Add bands by band index as saved.
		int nBandCount=m_vecBandInfos.size();
		LPMY_REBARBANDINFO* arrayMri =(LPMY_REBARBANDINFO*) malloc( nBandCount *sizeof(void*) );
// 		free(arrayMri);
// 		return ;
		for(auto i=m_vecBandInfos.begin();i!=m_vecBandInfos.end();++i)
		{
			MY_REBARBANDINFO *mri=*i;
			arrayMri[mri->bandIndex]=mri;
		}


		for ( int i=0; i<nBandCount; ++i )
		{
			MY_REBARBANDINFO * mri= arrayMri[i];

			if(mri->bShow)
				AddSimpleReBarBandCtrl(mri);
		}

		free(arrayMri);
	}
	else
	{
		//Add All Bands by register index.
		ResetBands(TRUE);
	}


	LockBands(m_bLock);
}