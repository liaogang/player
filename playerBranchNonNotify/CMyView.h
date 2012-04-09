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

		SetMsgHandled(FALSE);
		return 0;
	}

	LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		SetMsgHandled(FALSE);	
		return 0;
	}

};