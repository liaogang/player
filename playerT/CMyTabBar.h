
inline HRGN CreateRectRgn(RECT &rc){return ::CreateRectRgn(rc.left,rc.top,rc.right,rc.bottom);}

class CMyTabBar:public CWTLTabViewCtrl
{
public:
	HPEN  newPen,oldPen; 
	HBRUSH brush;
	CMyTabBar()
	{
		brush=::GetSysColorBrush(COLOR_WINDOW);
		//brush=::CreateSolidBrush(RGB(255,122,255));
		newPen=(HPEN)::CreatePen(PS_SOLID,0,RGB(255,255,255));
	}

	~CMyTabBar()
	{
		//DeleteObject(brush);//can not delete sys brush
		DeleteObject(newPen);
	}

public:


	DECLARE_WND_SUPERCLASS(NULL, CWTLTabViewCtrl::GetWndClassName())

	BOOL PreTranslateMessage(MSG* pMsg)
	{
		pMsg;
		return FALSE;
	}

	BEGIN_MSG_MAP_EX(CMyTabBar)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBackground)
		CHAIN_MSG_MAP(CWTLTabViewCtrl)
	END_MSG_MAP()
	
public:
	LRESULT OnEraseBackground(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
	{
		HDC hdc=(HDC)wParam;
		RECT rcClient,rcLastItem,rcErase;

		int count=GetItemCount();


		GetClientRect(&rcClient);
		GetItemRect(count-1,&rcLastItem);

		rcErase=rcClient;
		//rcErase.right=;
		rcErase.bottom=rcLastItem.bottom;

		HRGN rgnAll=CreateRectRgn(rcErase);
		
		for (int i=0;i<count;i++)
		{
			if(GetItemRect(i,&rcErase) )
			{
				HRGN tmp=CreateRectRgn(rcErase);
				CombineRgn(rgnAll,rgnAll,tmp,RGN_DIFF); 
			}
		}
		

 		oldPen=(HPEN )::SelectObject(hdc,newPen);
		FillRgn(hdc,rgnAll,brush);
 		::SelectObject(hdc,oldPen);
	
		return 1;
	}

	class CMainFrame* pMain;
	BOOL AddPlaylistTab(PlayList* ppl,BOOL inActiveFlag = TRUE, int inImage = -1);
};