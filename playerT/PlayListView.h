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
			PlaySelectedItem();
		}
	}

	void PlaySelectedItem();

	LRESULT OnDbClicked(UINT i,CPoint pt)
	{
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

		std::tstring columnName[]={_T("     title       "),_T(" artist "),_T(" album "),_T(" year "),_T(" comment "),_T(" genre ")};
		for (int i=0;i<6;i++)
		{
			std::tstring str=columnName[i];
			AddColumn(str.c_str(),i,-1, LVCF_FMT| LVCF_WIDTH|LVCF_TEXT|LVCF_SUBITEM ,LVCFMT_CENTER);
		}


		SetMsgHandled(FALSE);
		return 0;
	}
	
	void Reload(class PlayList* pPl);

};
