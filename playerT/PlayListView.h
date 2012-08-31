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

class CPlayListView:
	public CWindowImpl<CPlayListView,CListViewCtrl>
{

public:
	class CMainFrame *pMain;
	void SetMain(class CMainFrame *pMain);
	PlayListItem *m_pPlayTrack;

	BOOL bAuto,bDeletable;
	CPlayListView():bAuto(FALSE),bDeletable(!bAuto)
	{
	}

	~CPlayListView()
	{

	}

public:
	DECLARE_WND_SUPERCLASS(NULL,CListViewCtrl::GetWndClassName())


	virtual BOOL PreTranslateMessage(MSG* pMsg)
	{
		if (pMsg->hwnd!=m_hWnd)
			return FALSE;

		
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
		MSG_WM_LBUTTONDBLCLK(OnDbClicked)
		NOTIFY_HANDLER_EX(GetDlgCtrlID(),LVN_ITEMCHANGED,OnItemChanged)
		MSG_WM_CHAR(OnChar)
	END_MSG_MAP()

    LRESULT OnItemChanged(LPNMHDR pnmh)
	{
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
			if(GetSelTrack())
				PlaySelectedItem();
		}
	}

	BOOL GetSelTrack()
	{
		int nItem=-1;
		if (GetItemCount()>0)
		{
			nItem=GetNextItem(nItem,LVNI_SELECTED);
			m_pPlayTrack=(PlayListItem*)GetItemData(nItem);
			return TRUE;
		}
		return FALSE;
	}


	void PlaySelectedItem();

	LRESULT OnDbClicked(UINT i,CPoint pt)
	{
		if(GetSelTrack())
			PlaySelectedItem();

		SetMsgHandled(FALSE);
		return 0;
	} 


	void Init()
	{	
		SetExtendedListViewStyle(GetExtendedListViewStyle()|LVS_EX_FULLROWSELECT);

		TCHAR * columnName[]={_T("     title              "),_T(" artist      "),_T(" album       "),_T(" year "),_T(" comment "),_T(" genre      ")};
		for (int i=0;i<sizeof(columnName)/sizeof(int);i++)
			AddColumn(columnName[i],i,-1, LVCF_FMT| LVCF_WIDTH|LVCF_TEXT|LVCF_SUBITEM ,LVCFMT_CENTER);
		
	}
	
	void ClearAllSel()
	{
		int i=-1;
		while ( (i=GetNextItem(i,LVIS_SELECTED)) != -1)
			SetItemState(i,0,LVNI_SELECTED );
	}


	void InsertTrackItem(PlayListItem &track,int item,BOOL SetIndex=TRUE);
	inline void InsertTrackItem(PlayListItem *track,int item,BOOL SetIndex=TRUE){InsertTrackItem(*track,item,SetIndex);}

	template<class _InIt> inline
		void Reload(_InIt _First, _InIt _Last,BOOL SetIndex=TRUE)
	{
		DeleteAllItems();

		int j;_InIt i;
		for (i=_First,j=0;i!=_Last;i++,j++)
			InsertTrackItem(*i ,j,SetIndex);
	}

	void Reload(PlayList* pPl,BOOL SetIndex=TRUE)
	{
		Reload(pPl->m_songList.begin(),pPl->m_songList.end(),SetIndex);
	}

};
