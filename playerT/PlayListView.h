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
		MSG_WM_CREATE(OnCreate)
		MSG_WM_LBUTTONDBLCLK(OnDbClicked)
		NOTIFY_HANDLER_EX(GetDlgCtrlID(),LVN_ITEMCHANGED,OnItemChanged)
		MSG_WM_CHAR(OnChar)
	END_MSG_MAP()
	
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


	LRESULT OnCreate(LPCREATESTRUCT lpcs)
	{	
		//ModifyStyle( 0, LVS_REPORT|LVS_REPORT );

		UINT style;
		style=this->GetExtendedListViewStyle();
		style|= LVS_EX_FULLROWSELECT ;
		SetExtendedListViewStyle(style);

		std::tstring columnName[]={_T("     title              "),_T(" artist      "),_T(" album       "),_T(" year "),_T(" comment "),_T(" genre      ")};
		for (int i=0;i<6;i++)
		{
			std::tstring str=columnName[i];
			AddColumn(str.c_str(),i,-1, LVCF_FMT| LVCF_WIDTH|LVCF_TEXT|LVCF_SUBITEM ,LVCFMT_CENTER);
		}


		SetMsgHandled(FALSE);
		return 0;
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
		{
			InsertTrackItem(*i ,j,SetIndex);
		}
	}

	void Reload(PlayList* pPl,BOOL SetIndex=TRUE)
	{
		Reload(pPl->m_songList.begin(),pPl->m_songList.end(),SetIndex);
	}

};
