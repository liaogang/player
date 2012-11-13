#pragma once

unsigned int BKDRHash(char *str)
{
	unsigned int seed = 131; // 31 131 1313 13131 131313 etc..
	unsigned int hash = 0;

	while (*str){
		hash = hash * seed + (*str++);
	}

	return (hash & 0x7FFFFFFF);
}

HMENU LoadPlaylistMenu(BOOL bDestroy=FALSE)
{
	static HMENU menu=NULL;
	static HMENU subMenu=NULL;
	if (!bDestroy)
	{
		if (!menu)
		{	
			menu=::LoadMenu(NULL,MAKEINTRESOURCE(IDR_MENU_PLAYLIST));
			subMenu=::GetSubMenu(menu,0);
		}
	}
	else
	{
		if(menu)
		{
			::DestroyMenu(menu);
			menu=NULL;
		}
	}

	return subMenu;
}

class CPlayListView:
	public CWindowImpl<CPlayListView,CListViewCtrl>,
	public CMessageFilter
{
public:
	class CMainFrame *pMain;
	void SetMain(class CMainFrame *pMain);
	PlayList     *m_ppl;
	PlayListItem *m_pPlayTrack;

	BOOL bAuto,bDeletable;
	HMENU menu;
	BOOL m_bSearch;                  //是否为搜索列表

	COLORREF clText1,clText2;

	//奇,偶,颜色
	void ChangeColor(COLORREF one,COLORREF another)
	{
		clText1=one;
		clText2=another;
		int top=GetTopIndex();
		int countPerPage=GetCountPerPage();
		RedrawItems(top,top+countPerPage);
		::UpdateWindow(this->m_hWnd);
	}

	void ChangeColorDefault()
	{
		ChangeColor(RGB(249,249,249),RGB(255,255,255));
	}

	void ChangeColorBlue()
	{
		ChangeColor(RGB(230,244,255),RGB(145,200,255));
	}

	CPlayListView(BOOL bSearch=FALSE):
	m_bSearch(bSearch),
		bAuto(FALSE),
		bDeletable(!bAuto),
		m_pPlayTrack(NULL),m_bC(TRUE)
	{
		menu=LoadPlaylistMenu();
	}

	~CPlayListView()
	{
		LoadPlaylistMenu(TRUE);
	}

public:
	DECLARE_WND_SUPERCLASS(NULL,CListViewCtrl::GetWndClassName())


	virtual BOOL PreTranslateMessage(MSG* pMsg)
	{
		if (pMsg->message==WM_KEYDOWN)
		{	
			UINT nChar=(TCHAR)pMsg->wParam;

			if (nChar=='A' || nChar=='a')
			{
				if (GetKeyState(VK_CONTROL) &0x80)
					SelectAll();
			}
			else if (nChar==VK_DELETE)
			{
				DelSelectedItem(GetKeyState(VK_SHIFT) & 0x80);
			}

		}//if (pMsg->message!=WM_KEYDOWN)

		return FALSE;
	}

	BEGIN_MSG_MAP_EX(CPlayListView)
		MSG_WM_CREATE(OnCreate);
		MSG_WM_LBUTTONDBLCLK(OnDbClicked)
		MSG_WM_CHAR(OnChar)
		COMMAND_ID_HANDLER(ID_OPEN_FILE_PATH,OnOpenFilePath)

		REFLECTED_NOTIFY_CODE_HANDLER_EX(NM_RCLICK ,OnNotifyCodeHandlerEX)
		REFLECTED_NOTIFY_CODE_HANDLER(LVN_ITEMCHANGED,OnItemChanged)
		REFLECTED_NOTIFY_CODE_HANDLER(LVN_GETDISPINFO,OnGetdispInfo)
		REFLECTED_NOTIFY_CODE_HANDLER(NM_CUSTOMDRAW,OnCustomDraw)
	END_MSG_MAP()

		LRESULT OnCustomDraw(int idCtrl, LPNMHDR pnmh, BOOL& bHandled)
		{
			LPNMCUSTOMDRAW lpNMCustomDraw = (LPNMCUSTOMDRAW)pnmh;
			NMLVCUSTOMDRAW* pLVCD = reinterpret_cast<NMLVCUSTOMDRAW*>( lpNMCustomDraw );  

			DWORD dwRet = CDRF_DODEFAULT;
			switch(lpNMCustomDraw->dwDrawStage)
			{
			case CDDS_PREPAINT:
				dwRet=CDRF_NOTIFYITEMDRAW;
				break;
			case CDDS_ITEMPREPAINT:
				COLORREF crText;
				if ( (pLVCD->nmcd.dwItemSpec % 2) == 0 )
					crText = clText2; 
				else 
					crText = clText1; 

				// Store the color back in the NMLVCUSTOMDRAW struct.
				pLVCD->clrTextBk = crText;

				// Tell Windows to paint the control itself.
				dwRet= CDRF_DODEFAULT;
				break;
			}


			return dwRet;
		}

		LRESULT OnNotifyCodeHandlerEX(LPNMHDR pnmh)
		{
			POINT pt;
			GetCursorPos(&pt);

	
			if(GetNextItem(-1,LVIS_FOCUSED|LVIS_SELECTED)!=-1)
				::TrackPopupMenu(menu,TPM_LEFTALIGN,pt.x,pt.y,0,m_hWnd,0);


			return 1;
		}

		int OnCreate(LPCREATESTRUCT lpCreateStruct)
		{
			CMessageLoop* pLoop = _Module.GetMessageLoop();
			ATLASSERT(pLoop != NULL);
			pLoop->AddMessageFilter(this);
			SetMsgHandled(FALSE);
			return 0;
		}


		LRESULT OnGetdispInfo(int /**/,NMHDR *pNMHDR,BOOL bHandled);

		BOOL m_bC;
		LRESULT OnItemChanged(int /**/,LPNMHDR pnmh,BOOL bHandled)
		{
			NMLISTVIEW * pnml=(NMLISTVIEW *)pnmh;
			if(m_bC)
			{
				m_ppl->topVisibleIndex=pnml->iItem;
				m_ppl->selectedIndex=pnml->iItem;
				if (pnml->iItem!=-1)
				{
					m_ppl->SetSelectedItem(pnml->iItem);
				}
			}
			return 1;
		}

		inline void SelectAll(){SetItemState(-1, LVIS_SELECTED , LVIS_SELECTED );}

		void DelSelectedItem(BOOL bDelFile=FALSE)
		{
// 			if (bDeletable)
// 			{
// 				for(int i = GetItemCount()-1; i>=0;--i)
// 					if( LVIS_SELECTED==GetItemState(i, LVNI_ALL | LVNI_SELECTED) )
// 						DeleteItem(i);
// 			}
			//CListCtrl ctrl;ctrl.GetNextSelectedItem()GetNextItem((UINT)nOldPos, LVIS_SELECTED)
			if (!bDeletable)
				return;
			for(int i = GetItemCount()-1; i>=0;--i)
			{
				if(LVIS_SELECTED == GetItemState(i, LVIS_SELECTED) )
				{
					PlayListItem *track=(PlayListItem*)GetItemData(i);
					if(track )
					{
						if(bDelFile && IDYES==::MessageBox(m_hWnd,_T("这将会删除 1 文件 \n要继续吗?"),_T("确认删除文件"),MB_YESNO))
						{
							::DeleteFile(track->url.c_str());
						}

						track->m_pPL->DeleteTrack(track);
					}

					DeleteItem(i);
				}
			}	
			
		}

		void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
		{
			if (nChar==VK_RETURN)
			{
				PlayItem(GetFirstSelItem());
			}
		}

		BOOL GetFirstSelItem()
		{
			int nItem=-1;
			if (GetItemCount()>0)
				nItem=GetNextItem(nItem,LVNI_SELECTED);

			return nItem;
		}

		void PlayItem(int nItem);

		LRESULT OnDbClicked(UINT i,CPoint pt)
		{
			int k=GetFirstSelItem();
			if(k!=-1)
			{
				PlayItem(k);
				m_ppl->topVisibleIndex=k;
				m_ppl->selectedIndex=k;
			}

			SetMsgHandled(FALSE);
			return 0;
		} 



		enum{
			COLUMN_INDEX_INDEX,
			COLUMN_INDEX_TITLE,
			COLUMN_INDEX_ARTIST,
			COLUMN_INDEX_ALBUM,
			COLUMN_INDEX_YEAR,
			COLUMN_INDEX_GENRE,
			COLUMN_INDEX_COMMENT,
		};

		void Init()
		{	
			ChangeColorDefault();
			SetExtendedListViewStyle(GetExtendedListViewStyle()|LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP);

			const TCHAR * columnName[]={
				_T("Index"),
				_T("Title"),
				_T("Artist"),
				_T("Album"),
				_T("Year"),
				_T("Genre"),
				_T("Comment")};
				
			const UINT alignment[]={
				LVCFMT_RIGHT,
				LVCFMT_LEFT,
				LVCFMT_LEFT,
				LVCFMT_LEFT,
				LVCFMT_RIGHT,
				LVCFMT_RIGHT,
				LVCFMT_RIGHT};

			const TCHAR* columnNamePlaceHoder[]={
				_T("Index"),
				_T("Title                        "),
				_T("Artist"),
				_T("Album         "),
				_T("Year"),
				_T("Genre               "),
				_T("Comment")};

			const int cxOffset = 15;

			for (int i=0;i<sizeof(columnName)/sizeof(int);++i)
				AddColumn(columnName[i],i,-1, LVCF_FMT| LVCF_WIDTH|LVCF_TEXT|LVCF_SUBITEM ,alignment[i],GetStringWidth(columnNamePlaceHoder[i])+cxOffset);
		}

		int AddColumn(LPCTSTR strItem, int nItem, int nSubItem = -1,
			int nMask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM,
			int nFmt = LVCFMT_LEFT,int stringWidth=20)
		{
			
			ATLASSERT(::IsWindow(m_hWnd));
			LVCOLUMN lvc = { 0 };
			lvc.mask = nMask;
			lvc.fmt = nFmt;
			lvc.pszText = (LPTSTR)strItem;
			lvc.cx = stringWidth ;
			if(nMask & LVCF_SUBITEM)
				lvc.iSubItem = (nSubItem != -1) ? nSubItem : nItem;
			return InsertColumn(nItem, &lvc);
		}

		void ClearAllSel()
		{
			int i=-1;
			while ( (i=GetNextItem(i,LVIS_SELECTED)) != -1)
				SetItemState(i,0,LVNI_SELECTED|LVNI_FOCUSED );
		}

		void InsertTrackItem(PlayListItem &track,int item,BOOL SetIndex=TRUE);
		inline void InsertTrackItem(PlayListItem *track,int item,BOOL SetIndex=TRUE){InsertTrackItem(*track,item,SetIndex);}

		void Reload(PlayList* pPl,BOOL SetIndex=-1)
		{
			m_bC=FALSE;
			int songCount=pPl->m_songList.size();
			m_ppl=pPl;
			SetItemCount(songCount);
			ClearAllSel();

			if(SetIndex==-1)
			{
				int selItem;
				if(m_ppl->selectedIndex==-1)
				{
					selItem=0;
				}
				else
				{
					SetItemState(m_ppl->selectedIndex,LVIS_FOCUSED|
						LVIS_SELECTED,LVIS_FOCUSED|LVIS_SELECTED);
					selItem=m_ppl->selectedIndex;
				}

				EnsureVisibleAndCentrePos(selItem);
			}
			else
			{
				EnsureVisibleAndCentrePos(SetIndex);
				SetItemState(SetIndex,LVIS_FOCUSED|
					LVIS_SELECTED,LVIS_FOCUSED|LVIS_SELECTED);
			}//if(!SetIndex)


			m_bC=TRUE;
		}


		void EnsureVisibleAndCentrePos(int index)
		{
			EnsureVisible(index,FALSE);

			int top=GetTopIndex();
			int countPerPage=GetCountPerPage();

			RECT rc;
			GetItemRect(top,&rc,LVIR_BOUNDS);

			SIZE sz;
			sz.cx=0;
			sz.cy=(index-top-countPerPage/2)*(rc.bottom-rc.top);
			Scroll(sz);
		}


		LRESULT OnOpenFilePath(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
		{
			int nItem=GetNextItem(-1,LVIS_SELECTED);

			if(nItem!=-1)
			{
				PlayListItem *track=m_ppl->m_songList[nItem];

				std::tstring parameters=_T("/select,");
				parameters+=track->url;

				std::tstring dir;
				int index= track->url.find_last_of(_T("\\"));
				if (index!=track->url.npos)
					dir+=track->url.substr(0,index);

				/*   example
				*   "/select,C:\a.txt"
				*/
				ShellExecute(NULL,_T("open"),_T("explorer"),
					parameters.c_str(),dir.c_str(),SW_SHOW);
			}
			return 0;
		}

};
