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
		CHAIN_MSG_MAP(CWTLTabViewCtrl)
	END_MSG_MAP()
	
public:
	class CMainFrame* pMain;
	BOOL AddPlaylistTab(PlayList* ppl,BOOL inActiveFlag = TRUE, int inImage = -1);
};