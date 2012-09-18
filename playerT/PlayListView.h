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
			menu=::LoadMenu(NULL,MAKEINTRESOURCE(IDR_MENU2));
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
					crText = RGB(249,249,249);
				else 
					crText = RGB(255,255,255);        

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

			int i=-1;
			i=GetNextItem(i,LVIS_FOCUSED|LVIS_SELECTED);
			if(i!=-1)
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
			if (bDeletable)
			{
				int i=-1;
				i=GetNextItem(i,LVIS_FOCUSED|LVIS_SELECTED);
				while(i!=-1)
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

					if(DeleteItem(i)) i--;
					i=GetNextItem(i,LVIS_FOCUSED|LVIS_SELECTED);
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


		void Init()
		{	
			SetExtendedListViewStyle(GetExtendedListViewStyle()|LVS_EX_FULLROWSELECT);

			TCHAR * columnName[]={
				_T("     title              "),
				_T(" artist      "),
				_T(" album       "),
				_T(" year "),
				_T(" comment "),
				_T(" genre      ")};

				for (int i=0;i<sizeof(columnName)/sizeof(int);i++)
					AddColumn(columnName[i],i,-1, LVCF_FMT| LVCF_WIDTH|LVCF_TEXT|LVCF_SUBITEM ,LVCFMT_CENTER);
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
};
