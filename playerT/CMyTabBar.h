class CMyTabBar
	:public CWTLTabViewCtrl
{

	DECLARE_WND_SUPERCLASS(NULL, CWTLTabViewCtrl::GetWndClassName())

	BOOL PreTranslateMessage(MSG* pMsg)
	{
		pMsg;
		return FALSE;
	}

	BEGIN_MSG_MAP_EX(CMyTabBar)
		//REFLECTED_NOTIFY_CODE_HANDLER_EX(TCN_SELCHANGE, OnSelectionChanged)
		CHAIN_MSG_MAP(CWTLTabViewCtrl)
	END_MSG_MAP()



	void OnSelectionChanged()
	{
		int curSel= GetCurSel( );
		SetActiveTab(curSel);
		



		return;
	}

};