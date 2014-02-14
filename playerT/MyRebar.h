#include "stdafx.h"

#include "MySerialize.h"
#include <vector>


#define ID_REBAR_BASE 42948

typedef  HWND (*CreateReBarBandFuns)(HWND hWndParent);

struct MY_REBARBANDINFO 
{
public:
	MY_REBARBANDINFO():bAdded(FALSE){}
	REBARBANDINFO info;
	CreateReBarBandFuns createFunc;
	TCHAR szClassName[MAX_PATH];

	INT bandIndex; 
	BOOL bFullWidthAlways;
	RECT rc;
	//if is not removable , the menuID and bShow is not used ,
	//and the is no show/hide option in the PoPup menu
	BOOL bRemovable;
	UINT menuID;    //menu id in PoPup menu
	BOOL bAdded;
	BOOL bShow;     //means nows show in REBAR ?
};
typedef MY_REBARBANDINFO *LPMY_REBARBANDINFO;


class CMySimpleRebar:
	public CWindowImpl<CMySimpleRebar,CReBarCtrl>
	,public SerializeObj<CMySimpleRebar>
{   
public:
	typedef CWindowImpl<CMySimpleRebar,CReBarCtrl> thebase;
	DECLARE_WND_SUPERCLASS(NULL,CReBarCtrl::GetWndClassName())

	BEGIN_MSG_MAP_EX(CMySimpleRebar)
		MESSAGE_HANDLER(WM_DESTROY,OnDestroy)
		COMMAND_ID_HANDLER(ID_REBAR_RESET,OnResetBands)
		COMMAND_ID_HANDLER(ID_REBAR_LOCK,OnLockBands)
		COMMAND_RANGE_HANDLER(ID_REBAR_BASE,ID_REBAR_BASE+m_vecBandInfos.size(),OnShowBandX)
		MESSAGE_HANDLER(WM_CTLCOLORSTATIC,OnCtrlColorStatic)
		MESSAGE_HANDLER(WM_HSCROLL,OnHscroll)//Reflect Scroll Message
		MSG_WM_RBUTTONDOWN(OnRButtonDown)
		MSG_WM_RBUTTONUP(OnRButtonUp)
		MSG_WM_MEASUREITEM(OnMeasureItem)
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()

public:
	HMENU menu;
	BOOL bInitFromFile;
	BOOL m_bLock;
	static std::vector<MY_REBARBANDINFO*> m_vecBandInfos;

public:
	FILE& operator<<(FILE& f);
	FILE& operator>>(FILE& f) const ;

	CMySimpleRebar();

	static void RegisterRebarBand(  TCHAR* szBandClassName,CreateReBarBandFuns func,BOOL bRemovable ,BOOL bNewRow = FALSE , int cxWidth = 0 ,BOOL bFullWidthAlways = TRUE);

	static MY_REBARBANDINFO* MenuID2BandInfo(UINT menuID);

	UINT MenuID2BandID(UINT menuID);

	static MY_REBARBANDINFO* ClassName2BandInfo(TCHAR *className);

	void OnRButtonUp(UINT nFlags, CPoint point);

	HWND CreateIsWnd(HWND hParent);

	BOOL AddSimpleReBarBandCtrl(MY_REBARBANDINFO *mri);

	static BOOL AddSimpleReBarBandCtrl(HWND hWndReBar, HWND hWndBand, int nID = 0, LPCTSTR lpstrTitle = NULL, BOOL bNewRow = FALSE, int cxWidth = 0, int cxMinChild = 0,BOOL bFullWidthAlways = FALSE );

	void OnRButtonDown(UINT nFlags, CPoint point);

	LRESULT OnHscroll(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	
	LRESULT ReflectNotifications(_In_ UINT uMsg,_In_ WPARAM wParam,_In_ LPARAM lParam,_Inout_ BOOL& bHandled);

	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled);

	LRESULT OnCtrlColorStatic(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	
	LRESULT OnResetBands(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	
	LRESULT OnLockBands(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

	LRESULT OnShowBandX(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

	void OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct)
	{
		if(lpMeasureItemStruct->CtlType == ODT_COMBOBOX && lpMeasureItemStruct->itemID !=(UINT)-1)
		{
			LPCTSTR lpszText = (LPCTSTR) lpMeasureItemStruct->itemData;
			
			HDC dc = ::GetDC(m_hWnd);

			SIZE sz;
			::GetTextExtentPoint( dc ,lpszText,_tcslen(lpszText),&sz);

			ReleaseDC(dc);

			lpMeasureItemStruct->itemHeight = 2*sz.cy;
		}
	}

	void ResetBands(BOOL bAddAllBands=FALSE);

	void CollectInfo();

	void Init();
};	