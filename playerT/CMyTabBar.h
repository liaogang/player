
inline HRGN CreateRectRgn(RECT &rc){return ::CreateRectRgn(rc.left,rc.top,rc.right,rc.bottom);}

#define ID_MENU_DELPL (0XF000-210)
#define STR_MENU_DELPL _T("É¾³ý")


#define ID_MENU_PL_RENAME  (0XF000-211)
#define STR_MENU_PL_RENAME  _T("ÖØÃüÃû")

#define ID_MENU_PL_SAVE (0XF000-212)
#define STR_MENU_PL_SAVE   _T("Áí´æÎª")
#include <map>

class CMyTabBar:public CWTLTabViewCtrl
{
public:
	HPEN  newPen,oldPen; 
	HBRUSH brush;
	HMENU  menu;
	CMyTabBar()
	{
		brush=::GetSysColorBrush(COLOR_3DFACE);
		newPen=(HPEN)::CreatePen(PS_SOLID,0,RGB(255,255,255));

		menu=::CreatePopupMenu();
		::InsertMenu(menu,0,MF_BYCOMMAND|MF_BYPOSITION,ID_MENU_DELPL,STR_MENU_DELPL);
		::InsertMenu(menu,0,MF_BYCOMMAND|MF_BYPOSITION,ID_MENU_PL_RENAME,STR_MENU_PL_RENAME);
		::InsertMenu(menu,0,MF_BYCOMMAND|MF_BYPOSITION,ID_MENU_PL_SAVE,STR_MENU_PL_SAVE);
	}

	~CMyTabBar()
	{
		//can not delete sys brush
		DeleteObject(newPen);
		::DestroyMenu(menu);
	}

public:


	DECLARE_WND_SUPERCLASS(NULL, CWTLTabViewCtrl::GetWndClassName())

	BOOL PreTranslateMessage(MSG* pMsg)
	{
		pMsg;
		return FALSE;
	}

	BEGIN_MSG_MAP_EX(CMyTabBar)
		MESSAGE_HANDLER(WM_RBUTTONUP,OnRButtonUp)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBackground)
		COMMAND_ID_HANDLER(ID_MENU_DELPL, OnPlDel)
		COMMAND_ID_HANDLER(ID_MENU_PL_RENAME, OnPlRename)
		COMMAND_ID_HANDLER(ID_MENU_PL_SAVE, OnPlSave)
		CHAIN_MSG_MAP(CWTLTabViewCtrl)
	END_MSG_MAP()
	
public:
	int RtBtnTab;
	LRESULT OnRButtonUp(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		TCHITTESTINFO info;
		info.pt.x=GET_X_LPARAM(lParam);
		info.pt.y=GET_Y_LPARAM(lParam);
		info.flags=TCHT_ONITEM ;

		RtBtnTab=HitTest(&info);
		if (RtBtnTab!=-1)
		{
			::ClientToScreen(m_hWnd,&info.pt);
			::TrackPopupMenu(menu,TPM_LEFTALIGN,info.pt.x,info.pt.y,0,m_hWnd,0);
		}

		bHandled=FALSE;
		return 1;
	}

	LRESULT OnPlDel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnPlRename(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnPlSave(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

	LRESULT OnEraseBackground(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
	{
		HDC hdc=(HDC)wParam;
		RECT rcClient,rcLastItem,rcErase;

		int count=GetItemCount();

		GetClientRect(&rcClient);
		GetItemRect(count-1,&rcLastItem);

		rcErase=rcClient;
		rcErase.bottom=rcLastItem.bottom;

		HRGN rgnAll=CreateRectRgn(rcErase);
		
		for (int i=0;i<count;i++)
		{
			if( GetItemRect(i,&rcErase) )
			{
				HRGN tmp=CreateRectRgn(rcErase);
				CombineRgn(rgnAll,rgnAll,tmp,RGN_DIFF); 
				DeleteObject(tmp);
			}
		}
		
 		oldPen=(HPEN )::SelectObject(hdc,newPen);
		FillRgn(hdc,rgnAll,brush);
 		::SelectObject(hdc,oldPen);
		DeleteObject(rgnAll);
		return 1;
	}

	PlayList* GetTabPlayList(int tabIndex)
	{
		return pplOnTag[tabIndex];
	}

	void *RemoveTabPlayList(int tabIndex)
	{
		vector<PlayList*>::iterator i;
		for (i=pplOnTag.begin();i!=pplOnTag.end();i++)
		{
			if (*i == pplOnTag[tabIndex])
			{
				pplOnTag.erase(i);
			}
		}

		
	}
	

	vector<PlayList*> pplOnTag;
	class CMainFrame* pMain;
	BOOL AddPlaylistTab(PlayList* ppl,BOOL inActiveFlag = TRUE, int inImage = -1);
};