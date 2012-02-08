#pragma once
class CMainFrame;
class CMyView:public CWindowImpl<CMyView,CListViewCtrl>
{
public:
	CMainFrame *m_pMainFrame;
public:
	DECLARE_WND_SUPERCLASS(NULL,CListViewCtrl::GetWndClassName())

	BEGIN_MSG_MAP_EX(CMyView)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		MSG_WM_CREATE(OnCreate)
		MSG_WM_LBUTTONDBLCLK(OnDbClicked)
	END_MSG_MAP()

	LRESULT OnDbClicked(UINT i,CPoint pt);

	LRESULT OnCreate(LPCREATESTRUCT lpcs)
	{	
		UINT style;
		style=this->GetExtendedListViewStyle();
		style|= LVS_EX_FULLROWSELECT ;
		SetExtendedListViewStyle(style);
		
		for (int i=0;i<1;i++)
		{
			tstring str(_T("          ¸èÇú         "));
			//str+=i;
			AddColumn(str.c_str(),i,-1,LVCF_FMT|LVCF_WIDTH|LVCF_TEXT|LVCF_SUBITEM,LVCFMT_CENTER);
		}
		//InsertItem(0,_T("sadf"));

		SetMsgHandled(FALSE);
		return 0;
	}

	LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		SetMsgHandled(FALSE);
		
		return 0;
	}

};