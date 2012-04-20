#pragma once
class CMainFrame;
class CMyView:public CWindowImpl<CMyView,CListViewCtrl>
{
public:
	CMainFrame *m_pMainFrame;
public:
	DECLARE_WND_SUPERCLASS(NULL,CListViewCtrl::GetWndClassName())

	BEGIN_MSG_MAP_EX(CMyView)
		//MESSAGE_HANDLER(WM_SIZE, OnSize)
		MSG_WM_CREATE(OnCreate)
		MSG_WM_LBUTTONDBLCLK(OnDbClicked)
	END_MSG_MAP()
	LRESULT OnDbClicked(UINT i,CPoint pt);

	LRESULT OnCreate(LPCREATESTRUCT lpcs)
	{	
		tstring columnName[]={_T("     title       "),_T(" artist "),_T(" album "),_T(" year "),_T(" comment "),_T(" genre ")};
		for (int i=0;i<6;i++)
		{
			tstring str=columnName[i];
			AddColumn(str.c_str(),i,-1, LVCF_FMT| LVCF_WIDTH|LVCF_TEXT|LVCF_SUBITEM ,LVCFMT_CENTER);
		}


		UINT style;
		style=this->GetExtendedListViewStyle();
		style|= LVS_EX_FULLROWSELECT ;
		SetExtendedListViewStyle(style);

		SetMsgHandled(FALSE);
		return 0;
	}

	LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		int _width=LOWORD(lParam);
		int _hight=HIWORD(lParam);
		
		SetMsgHandled(FALSE);	
		return 0;
	}
};