inline HRGN CreateRectRgn(RECT &rc){return ::CreateRectRgn(rc.left,rc.top,rc.right,rc.bottom);}

#include <map>

class CMyTabBar:public CWTLTabViewCtrl
{
public:
	HPEN  newPen,oldPen; 
	HBRUSH brush;
	HMENU  menu,subMenu;
	CMyTabBar()
	{
		brush=::GetSysColorBrush(COLOR_3DFACE);
		newPen=(HPEN)::CreatePen(PS_SOLID,0,RGB(255,255,255));

		menu=::LoadMenu(NULL,MAKEINTRESOURCE(IDR_MENU_TABBAR));
		subMenu=GetSubMenu(menu,0);
	}

	~CMyTabBar()
	{
		//can not delete sys brush
		DeleteObject(newPen);
		::DestroyMenu(menu);
	}

public:


	DECLARE_WND_SUPERCLASS(NULL, CWTLTabViewCtrl::GetWndClassName())

	BOOL PreTranslateMessage(MSG* pMsg)
	{
		pMsg;
		return FALSE;
	}

	BEGIN_MSG_MAP_EX(CMyTabBar)
		MESSAGE_HANDLER(WM_RBUTTONUP,OnRButtonUp)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBackground)
		COMMAND_ID_HANDLER(ID_TABBAR_DEL, OnPlDel)
		COMMAND_ID_HANDLER(ID_TABBAR_RENAME, OnPlRename)
		COMMAND_ID_HANDLER(ID_TABBAR_SAVEAS, OnPlSave)
		COMMAND_ID_HANDLER(ID_TABBAR_OPEN_FILEFOLDER, OnPlOpenFolder)
		REFLECTED_NOTIFY_CODE_HANDLER_EX(TCN_SELCHANGE, OnSelectionChanged)
		CHAIN_MSG_MAP(CWTLTabViewCtrl)
	END_MSG_MAP()
	
public:
	int RtBtnTab;
	LRESULT OnRButtonUp(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		TCHITTESTINFO info;
		info.pt.x=GET_X_LPARAM(lParam);
		info.pt.y=GET_Y_LPARAM(lParam);
		info.flags=TCHT_ONITEM ;

		RtBtnTab=HitTest(&info);
		if (RtBtnTab!=-1)
		{
			::ClientToScreen(m_hWnd,&info.pt);
			::TrackPopupMenu(subMenu,TPM_LEFTALIGN,info.pt.x,info.pt.y,0,m_hWnd,0);
		}

		bHandled=FALSE;
		return 1;
	}

	LRESULT OnPlDel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnPlRename(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnPlSave(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnPlOpenFolder(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

	LRESULT OnEraseBackground(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
	{
		//-----------------------------------------
		//the tab paint only the TabItem and a line beneath 
		//-----------------------------------------
		CRect theTabRect;
		GetItemRect( 0, &theTabRect );

		LONG theRowCount = GetRowCount( );
		//LONG theEdgeWidth = ( theTabRect.Width() * theRowCount ) + TABVIEW_EDGE;
		LONG theEdgeHeight = ( theTabRect.Height() * theRowCount ) + TABVIEW_EDGE;
		//-----------------------------------------
		HDC hdc=(HDC)wParam;
		RECT rcClient,rcLastItem,rcErase;

		int count=GetItemCount();
		int curItem=GetCurSel();
		BOOL xp_themed=FALSE;
		BOOL isVista=FALSE;

		GetClientRect(&rcClient);
		GetItemRect(count-1,&rcLastItem);

		rcErase=rcClient;
		rcErase.bottom=rcLastItem.bottom+TABVIEW_EDGE;

		HRGN rgnAll=CreateRectRgn(rcErase);
		//remove the line rect
		static const int lineHeight=1;
		RECT rcLine=rcClient;
		rcLine.top=rcLastItem.bottom;
		rcLine.bottom=rcLine.top+lineHeight;
		HRGN tmp=CreateRectRgn(rcLine);
		CombineRgn(rgnAll,rgnAll,tmp,RGN_DIFF); 
		DeleteObject(tmp);

		//remove the items rect
		for (int i=0;i<count;i++)
		{
			if( GetItemRect(i,&rcErase) )
			{
				if (i==curItem)
				{
					rcErase.left-=1;
					rcErase.right+=1;
					rcErase.top-=2;
					if (i==0)
					{
						rcErase.left-=1;
						if (!xp_themed)
							rcErase.right+=1;
					}
					else if (i==count-1)
					{rcErase.right+=1;}
				}else
				{
					rcErase.bottom-=2;
					rcErase.right-=1;
					if((xp_themed||isVista) && i==count-1)
						rcErase.right-=1;
				}


				HRGN tmp=CreateRectRgn(rcErase);
				CombineRgn(rgnAll,rgnAll,tmp,RGN_DIFF); 
				DeleteObject(tmp);
			}
		}
		
 		oldPen=(HPEN )::SelectObject(hdc,newPen);
		FillRgn(hdc,rgnAll,brush);
 		::SelectObject(hdc,oldPen);
		DeleteObject(rgnAll);
		return 1;
	}

	LRESULT OnSelectionChanged( LPNMHDR );
	
	PlayList* GetTabPlayList(int tabIndex){	return  (PlayList*)GetTabParam(tabIndex);}

	void *RemoveTabPlayList(int tabIndex)
	{
		ATLASSERT(FALSE);
	}
	
	void SetCurSel(PlayList* ppl,int index=-1);
	class CMainFrame* pMain;
	BOOL AddPlaylistTab(PlayList* ppl,BOOL inActiveFlag = TRUE, int inImage = -1);
};



BOOL CMyTabBar::AddPlaylistTab(PlayList* ppl,BOOL inActiveFlag , int inImage )
{
	//int nNewTab=AddTab(ppl->m_playlistName.c_str(),pMain->m_pPlaylistView->m_hWnd,inActiveFlag,inImage,(LPARAM)ppl);

	// 	pMain->m_pPlaylistView->Reload(ppl);
	// 	MyLib::shared()->SetSelPlaylist(ppl);

	return 0;
}

LRESULT CMyTabBar::OnPlDel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	PlayList* pl= GetTabPlayList(RtBtnTab);

	RemoveTab(RtBtnTab);
	//pMain->m_pPlaylistView->Reload(GetTabPlayList(GetCurSel()));

	MyLib::shared()->DeletePlayList(pl);

	return 0;
}
LRESULT CMyTabBar::OnPlRename(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	PlayList* pl= GetTabPlayList(RtBtnTab);

	// 	CFileDialog 
	// 	{
	// 		pl->Rename();
	// 	}

	return 0;
}
LRESULT CMyTabBar::OnPlSave(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	PlayList* pl= GetTabPlayList(RtBtnTab);
	if(pl)
	{
		const TCHAR szFilter[]=_T("playlist files(*.pl)\0*.pl\0");
		const TCHAR szDefaultExt[]=_T("pl");
		CFileDialog dlg(FALSE,szDefaultExt,NULL,OFN_OVERWRITEPROMPT,szFilter ,m_hWnd);
		if(dlg.DoModal()==IDOK)
			MyLib::SavePlaylist(pl,dlg.m_ofn.lpstrFile);
	}

	return 0;
}

LRESULT CMyTabBar::OnPlOpenFolder(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	PlayList* pl= GetTabPlayList(RtBtnTab);
	if(pl)
	{
		// 		std::tstring parameters(_T("/select,"));
		// 		parameters+=pl->m_saveLocation;
		// 
		// 		ShellExecute(NULL,
		// 			_T("open"),
		// 			_T("explorer"),
		// 			parameters.c_str(),
		// 			0,
		// 			SW_SHOWNORMAL
		// 			);
	}

	return 0;
}
