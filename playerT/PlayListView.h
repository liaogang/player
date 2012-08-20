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
public:
	DECLARE_WND_SUPERCLASS(NULL,CListViewCtrl::GetWndClassName())

	BEGIN_MSG_MAP_EX(CPlayListView)
		MSG_WM_LBUTTONDBLCLK(OnDbClicked)
		NOTIFY_HANDLER_EX(GetDlgCtrlID(),LVN_ITEMCHANGED,OnItemChanged)
		MSG_WM_CHAR(OnChar)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBackground)
	END_MSG_MAP()
	
	LRESULT OnEraseBackground(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
	{
		
		HDC hdc=(HDC)wParam;
		RECT rcClient,rcSubItem,rcErase1,rcErase2;
		GetClientRect(&rcClient);

		HWND hWndHeader=ListView_GetHeader(m_hWnd);
		int columnNum=(int)::SendMessage(hWndHeader, HDM_GETITEMCOUNT, 0, 0L);
		ListView_GetSubItemRect(m_hWnd,GetItemCount()-1,columnNum-1,LVIR_BOUNDS,&rcSubItem);

// 		if (rcSubItem.bottom)
// 		{
// 		}

		rcErase1=rcClient;
		rcErase1.left=rcSubItem.right;
		
		rcErase2=rcClient;
		rcErase2.top=rcSubItem.bottom;
		rcErase2.right=rcSubItem.right;

		HBRUSH newBrush,oldBrush;
		//newBrush=::CreateSolidBrush(RGB(242,244,243));
		//newBrush=GetSysColorBrush(COLOR_WINDOWFRAME);
		newBrush=(HBRUSH)COLOR_WINDOW+1;
		oldBrush=(HBRUSH)::SelectObject(hdc,newBrush);
		
		HPEN  newPen,oldPen; 
		newPen=(HPEN)::CreatePen(PS_SOLID,0,RGB(255,255,255));
		oldPen=(HPEN )::SelectObject(hdc,newPen);

		::Rectangle(hdc,rcErase1.left,rcErase1.top,rcErase1.right,rcErase1.bottom);
		::Rectangle(hdc,rcErase2.left,rcErase2.top,rcErase2.right,rcErase2.bottom);
		
		::SelectObject(hdc,oldPen);
		::SelectObject(hdc,oldBrush);
		
		// handled, no background painting needed
		//bHandled=FALSE;
		return 1;
	}

    LRESULT OnItemChanged(LPNMHDR pnmh)
	{
		return 1;
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

		std::tstring columnName[]={_T("     title              "),_T(" artist      "),_T(" album       "),_T(" year "),_T(" comment "),_T(" genre      ")};
		for (int i=0;i<6;i++)
		{ 
			std::tstring str=columnName[i];
			AddColumn(str.c_str(),i,-1, LVCF_FMT| LVCF_WIDTH|LVCF_TEXT|LVCF_SUBITEM ,LVCFMT_CENTER);
		}

	}
	


	void InsertTrackItem(PlayListItem &track,int item,BOOL SetIndex=TRUE);
	void InsertTrackItem(PlayListItem *track,int item,BOOL SetIndex=TRUE)
	{
		InsertTrackItem(*track,item,SetIndex);
	}

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
