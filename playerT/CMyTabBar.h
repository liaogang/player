class CMyTabBar:public CWTLTabViewCtrl
{
	DECLARE_WND_SUPERCLASS(NULL, CWTLTabViewCtrl::GetWndClassName())

	BOOL PreTranslateMessage(MSG* pMsg)
	{
		pMsg;
		return FALSE;
	}

	BEGIN_MSG_MAP_EX(CMyTabBar)
		//MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBackground)
		CHAIN_MSG_MAP(CWTLTabViewCtrl)
	END_MSG_MAP()
	
public:
	LRESULT OnEraseBackground(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
// 		HDC hdc=(HDC)wParam;
// 		RECT rc;
// 		HBRUSH newBrush,oldBrush;
// 
// 		newBrush=::CreateSolidBrush(RGB(242,244,243));
// 		//newBrush=GetSysColorBrush(COLOR_WINDOWFRAME);
// 		::GetClientRect(m_hWnd,&rc);
// 		oldBrush=(HBRUSH)::SelectObject(hdc,newBrush);
// 		::Rectangle(hdc,rc.left,rc.top,rc.right,rc.bottom);
// 		::SelectObject(hdc,oldBrush);

		// handled, no background painting needed
		SetMsgHandled(FALSE);
		return 1;
	}
	class CMainFrame* pMain;
	BOOL AddPlaylistTab(PlayList* ppl,BOOL inActiveFlag = TRUE, int inImage = -1);
};