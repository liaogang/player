#include "stdafx.h"
#include "customMsg.h"
#include "globalStuffs.h"
#include "mysplit.h"

//基类属性面板树右边的对话框
#include <string>
class CDlgConfig
{
public:
	std::tstring title;
	HWND hWndCp;
	void Init(TCHAR *pTitle,HWND _hWndcp)
	{
		title=pTitle;
		hWndCp=_hWndcp;
	}
};

class CPropertyDlg:
	public CDlgConfig,
	public CDialogImpl<CPropertyDlg>,
	public CWinDataExchange<CPropertyDlg>	
{
public:
	enum { IDD = IDD_DLG_EMPTY };

	BEGIN_MSG_MAP(CPropertyDlg)
	END_MSG_MAP()
};

class CPropertyDlg1:
	public CDlgConfig,
	public CDialogImpl<CPropertyDlg1>,
	public CWinDataExchange<CPropertyDlg1>
{
public:
	enum { IDD = IDD_DLG_CFG_1 };
	BEGIN_MSG_MAP(CPropertyDlg1)
	END_MSG_MAP()


};



class CMyListViewCtrl:
	public CWindowImpl<CMyListViewCtrl,CListViewCtrl>
{
public:
	DECLARE_WND_SUPERCLASS(NULL,CListViewCtrl::GetWndClassName())

	BEGIN_MSG_MAP_EX(CMyListViewCtrl)
	END_MSG_MAP()


};

class CPropertyDlgMediaLib:
	public CDlgConfig,
	public CDialogImpl<CPropertyDlgMediaLib>,
	public CWinDataExchange<CPropertyDlgMediaLib>,
	public CMessageFilter
{
public:
	enum { IDD = IDD_DIALOG_MEDIAlIB };
	virtual BOOL PreTranslateMessage(MSG* pMsg)
	{
		return IsDialogMessage(pMsg);
	}
	BEGIN_MSG_MAP(CPropertyDlgMediaLib)
		MSG_WM_INITDIALOG(OnInitDialog)
		COMMAND_ID_HANDLER(IDC_BTN_ADD,OnBtnAdd)
		COMMAND_ID_HANDLER(IDC_BTN_DEL,OnBtnDel)
		NOTIFY_HANDLER_EX(IDC_MEDIA_LIST,LVN_ITEMCHANGED,OnItemChanged)
		MSG_WM_KILLFOCUS(OnKillFocus)
		MESSAGE_HANDLER(WM_CONFIGTOSAVE,OnCfgToSave)
	END_MSG_MAP()

	BEGIN_DDX_MAP(CPropertyDlgMediaLib)
		DDX_CONTROL(IDC_MEDIA_LIST,m_list)
	END_DDX_MAP()

	LRESULT OnItemChanged(LPNMHDR pnmh)
	{
		int index=m_list.GetSelectedIndex();
		::EnableWindow(GetDlgItem(IDC_BTN_DEL),index==-1?FALSE:TRUE);
		return 1;
	}

	void OnKillFocus(CWindow wndFocus)
	{
		//::EnableWindow(GetDlgItem(IDC_BTN_DEL),FALSE);
	}



	LRESULT OnBtnAdd(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBtnDel(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

	CMyListViewCtrl m_list;
	BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam)
	{
		DoDataExchange();
		UINT style;

		style=::GetWindowLong(m_list.m_hWnd,GWL_STYLE);
		style|=LVS_SINGLESEL;
		::SetWindowLong(m_list.m_hWnd,GWL_STYLE,style);

		style=m_list.GetExtendedListViewStyle();
		style|= LVS_EX_FULLROWSELECT;
		m_list.SetExtendedListViewStyle(style);

		m_list.InsertColumn(0,_T("路径"),LVCFMT_LEFT,220);
		m_list.InsertColumn(1,_T("状态"),LVCFMT_LEFT,80);


		CMessageLoop* pLoop = _Module.GetMessageLoop();
		ATLASSERT(pLoop != NULL);
		pLoop->AddMessageFilter(this);

		return TRUE;
	}

	LRESULT OnCfgToSave(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		return 0;
	}
};



class CPropertyDlgLyricsLib:
	public CDlgConfig,
	public CDialogImpl<CPropertyDlgLyricsLib>,
	public CWinDataExchange<CPropertyDlgLyricsLib>
{
public:
	enum { IDD = IDD_DIALOG_LYRICS };

	CMyListViewCtrl list;

	BEGIN_MSG_MAP(CPropertyDlgLyricsLib)
		MSG_WM_INITDIALOG(OnInitDialog)
		COMMAND_ID_HANDLER(IDC_BTN_ADD,OnBtnAdd)
		COMMAND_ID_HANDLER(IDC_BTN_DEL,OnBtnDel)
		NOTIFY_HANDLER_EX(IDC_MEDIA_LIST,LVN_ITEMCHANGED,OnItemChanged)
		MESSAGE_HANDLER(WM_CONFIGTOSAVE,OnCfgToSave)
	END_MSG_MAP()

	BEGIN_DDX_MAP(CPropertyDlgMediaLib)
		DDX_CONTROL(IDC_MEDIA_LIST,list)
	END_DDX_MAP()

	BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam);
	LRESULT OnBtnAdd(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBtnDel(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnCfgToSave(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnItemChanged(LPNMHDR pnmh)
	{
		int index=list.GetSelectedIndex();
		::EnableWindow(GetDlgItem(IDC_BTN_DEL),index==-1?FALSE:TRUE);
		return 1;
	}
};



class CPropertyDlgLrcPane:
	public CDlgConfig,
	public CDialogImpl<CPropertyDlgLrcPane>,
	public CWinDataExchange<CPropertyDlgLrcPane>
{
public:
	enum { IDD = IDD_DLG_LRC_PANE_PROPERTY };
	BEGIN_MSG_MAP(CPropertyDlgLrcPane)
	END_MSG_MAP()
};



class CPlaceHolderWnd :
	public CWindowImpl<CPlaceHolderWnd>
{
public:
	HWND child;	
public:
	DECLARE_WND_CLASS_EX(NULL,NULL,NULL)
		
	BEGIN_MSG_MAP_EX(CPlaceHolderWnd)
		//MSG_WM_NCPAINT(OnNcPaint)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBackground)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MESSAGE_HANDLER(WM_SIZE, OnSize)
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()
	
	void OnNcPaint(CRgnHandle rgn)
	{
		RECT r;
		GetWindowRect(&r);
		ScreenToClient(&r);

		HDC hdc;//=::GetDC(m_hWnd);
		int b=::IsWindow(m_hWnd);
		if((int)rgn.m_hRgn==1)
			hdc=::GetWindowDC(m_hWnd);
		else
			hdc = ::GetDCEx(m_hWnd,(HRGN)rgn.m_hRgn, DCX_WINDOW|DCX_INTERSECTRGN);

// 		r.bottom+=r.top;
// 		r.right+=r.left;
// 		r.top=0;
// 		r.left=0;

		// Paint into this DC
		DrawEdge(hdc,&r, EDGE_SUNKEN, BF_RECT | BF_ADJUST);
		//FillRect(hdc,&r, (HBRUSH)COLOR_APPWORKSPACE);

		ReleaseDC(hdc);
	}

	LRESULT OnSize(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
	{
		RECT rect;
		GetClientRect(&rect);
		
		//::SetWindowPos(child,NULL, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top,SWP_NOZORDER | SWP_NOACTIVATE);

		bHandled=FALSE;
		return 1;
	}

	void OnRButtonUp(UINT nFlags, CPoint point)
	{
		//::ClientToScreen(m_hWnd,&point);
		//::TrackPopupMenu(menu,TPM_LEFTALIGN,point.x,point.y,0,m_hWnd,0);
	}

	LRESULT OnEraseBackground(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		// handled, no background painting needed
		return 1;
	}




	LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		PAINTSTRUCT ps;
		::BeginPaint(m_hWnd,&ps);

		RECT r;
		GetClientRect(&r);

		//dc.DrawEdge(&r, EDGE_SUNKEN, BF_RECT | BF_ADJUST);

		::FillRect(ps.hdc,&r,(HBRUSH)LongToPtr(COLOR_APPWORKSPACE+ 1));

		SetBkMode(ps.hdc,TRANSPARENT);
		::TextOut(ps.hdc,r.left+30,r.top+30,L"place holder window",wcslen(L"place holder window"));

		::EndPaint(m_hWnd,&ps);	
		return 0;
	}
};




class CPlaceHolderWnd2 :
	public CWindowImpl<CPlaceHolderWnd2>
{
public:

public:
	DECLARE_WND_CLASS(NULL)

	BEGIN_MSG_MAP_EX(CPlaceHolderWnd2)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBackground)
		MESSAGE_HANDLER(WM_PAINT, OnPaint)
		MSG_WM_SIZE(OnSize)
	END_MSG_MAP()

	void OnSize(UINT nType, CSize size)
	{
		//GetClientRect(&rc);
		SetMsgHandled(FALSE);
	}

	void OnRButtonUp(UINT nFlags, CPoint point)
	{
		//::ClientToScreen(m_hWnd,&point);
		//::TrackPopupMenu(menu,TPM_LEFTALIGN,point.x,point.y,0,m_hWnd,0);
	}

	LRESULT OnEraseBackground(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		// handled, no background painting needed
		return 1;
	}




	LRESULT OnPaint(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		PAINTSTRUCT ps;
		::BeginPaint(m_hWnd,&ps);

		CDCHandle dc(ps.hdc);

		RECT r;
		GetClientRect(&r);

		dc.DrawEdge(&r, EDGE_SUNKEN, BF_RECT | BF_ADJUST);
		dc.FillRect(&r, COLOR_3DFACE);


		dc.TextOut(r.left+5,r.top+5,L"this is a placeholder window");

		::EndPaint(m_hWnd,&ps);	

		return 0;
	}
};



class CMyTreeView:public CWindowImpl<CTreeViewCtrl,CTreeViewCtrl>
{
public:
	DECLARE_WND_CLASS(NULL)

	BEGIN_MSG_MAP_EX(CMyTreeView)
		REFLECTED_NOTIFY_CODE_HANDLER_EX(NM_RCLICK ,OnNotifyCodeHandlerEX)

		COMMAND_ID_HANDLER(ID_INSERT_SPLIT_UD,OnAddSplitUpDown)		
		COMMAND_ID_HANDLER(ID_INSERT_SPLIT_LR,OnAddSplitLeftRight)

		COMMAND_ID_HANDLER(ID_PANE_PLAYLIST,OnAddPanePlaylist)		
		COMMAND_ID_HANDLER(ID_DEL_PANE,OnDelPane)
		COMMAND_ID_HANDLER(ID_ADD_EMPTY_PANE,OnAddPaneEmptyWnd)
		COMMAND_ID_HANDLER(ID_PANE_LYRIC,OnAddPaneLyric)
		COMMAND_ID_HANDLER(ID_PANE_COVER,OnAddPaneAlbum)
		COMMAND_ID_HANDLER(ID_PANE_SPE,OnAddPaneSpectrum)

		COMMAND_ID_HANDLER(ID_CHANGE_SPLIT_UD,OnChangeUpdown)
		COMMAND_ID_HANDLER(ID_CHANGE_SPLIT_LR,OnChangeLeftRight)


	END_MSG_MAP()

	HMENU menu,splitMenu,paneMenu;
	HTREEITEM rclickItem;


	LRESULT OnAddSplitUpDown(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		MYTREE *mytree=(MYTREE*)GetItemData(rclickItem);

		MYTREE *newSplit=new MYTREE;
		newSplit->data.SetSplitType(up_down);

		mytree->AddChild(newSplit);

		mytree->CalcChildsRect();
		MoveToNewRect(mytree);
		UpdateTree(mytree);

		return 0;
	}

	LRESULT OnAddSplitLeftRight(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		MYTREE *mytree=(MYTREE*)GetItemData(rclickItem);

		MYTREE *newSplit=new MYTREE;
		newSplit->data.SetSplitType(left_right);

		mytree->AddChild(newSplit);

		mytree->CalcChildsRect();
		MoveToNewRect(mytree);
		UpdateTree(mytree);

		return 0;
	}

	LRESULT OnAddPanePlaylist(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		MYTREE *mytree=(MYTREE*)GetItemData(rclickItem);

		MYTREE_Add_Playlist(mytree);

		MoveToNewRect(mytree);
		UpdateTree(mytree);
		GetSplitter()->Invalidate();
		return 0;
	}




	LRESULT OnAddPaneSpectrum(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		MYTREE *mytree=(MYTREE*)GetItemData(rclickItem);
		MYTREE_Add_SpectrumView(mytree);
		MoveToNewRect(mytree);
		UpdateTree(mytree);
		GetSplitter()->Invalidate();

		return 0;
	}
	LRESULT OnAddPaneAlbum(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		MYTREE *mytree=(MYTREE*)GetItemData(rclickItem);
		MYTREE_Add_AlbumView(mytree);
		MoveToNewRect(mytree);
		UpdateTree(mytree);

		return 0;
	}
	LRESULT OnAddPaneLyric(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		MYTREE *mytree=(MYTREE*)GetItemData(rclickItem);
		MYTREE_Add_LyricView(mytree);
		MoveToNewRect(mytree);
		UpdateTree(mytree);
		GetSplitter()->Invalidate();

		return 0;
	}


	LRESULT OnAddPaneEmptyWnd(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		MYTREE *mytree=(MYTREE*)GetItemData(rclickItem);
		MYTREE_Add_EmptyWnd(mytree);
		MoveToNewRect(mytree);
		UpdateTree(mytree);
		GetSplitter()->Invalidate();


		return 0;
	}


	LRESULT OnChangeLeftRight(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		MYTREE *mytree=(MYTREE*)GetItemData(rclickItem);
		if(mytree->data.SetSplitType(left_right))
		{
			mytree->CalcChildsRect();
			MoveToNewRect(mytree);
			UpdateTree(mytree);
			GetSplitter()->Invalidate();
		}

		return 0;
	}

	LRESULT OnChangeUpdown(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		MYTREE *mytree=(MYTREE*)GetItemData(rclickItem);

		if(mytree->data.SetSplitType(up_down))
		{
			mytree->CalcChildsRect();
			MoveToNewRect(mytree);
			UpdateTree(mytree);
			GetSplitter()->Invalidate();
		}

		return 0;
	}

	LRESULT OnDelPane(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		MYTREE *mytree=(MYTREE*)GetItemData(rclickItem);

		MYTREE *parent=MYTree_RemoveFromRoot(mytree);

		parent->CalcChildsRect();
		MoveToNewRect(parent);
		UpdateTree(parent);
		GetSplitter()->Invalidate();

		return 0;
	}


	LRESULT OnNotifyCodeHandlerEX(LPNMHDR pnmh)
	{
		POINT pt;
		GetCursorPos(&pt);

		POINT ptClient=pt;
		ScreenToClient(&ptClient);
		UINT flag=TVHT_ONITEMLABEL  ;
		rclickItem= HitTest(ptClient,&flag);

		if(rclickItem )
		{
			MYTREE* mytree=(MYTREE*)GetItemData(rclickItem);
			if(mytree->data.hWnd==0)//is split pane
			{
				//是根的话,禁用删除
				UINT flag= (mytree->isroot()?MF_DISABLED|MF_GRAYED:MF_ENABLED);
				::EnableMenuItem(splitMenu,ID_DEL_PANE,MF_BYCOMMAND |flag);	
				::TrackPopupMenu(splitMenu,TPM_LEFTALIGN,pt.x,pt.y,0,m_hWnd,0);
			}
			else
				::TrackPopupMenu(paneMenu,TPM_LEFTALIGN,pt.x,pt.y,0,m_hWnd,0);
		}

		return 1;
	}


	CMyTreeView()
	{
		menu=::LoadMenu(NULL,MAKEINTRESOURCE(IDR_MENU_EDIT_MODE));
		splitMenu=::GetSubMenu(menu,0);
		paneMenu=::GetSubMenu(menu,1);
	}

	void TraverseSplitTreeAndShowInTreeView(MYTREE *cur,HTREEITEM item);
	void UpdateTree(MYTREE *treeData);


};

class MYTREE;
class CPropertyDlgUILayout:
	public CDlgConfig,
	public CDialogImpl<CPropertyDlgUILayout>
{
public:
	enum { IDD = IDD_DLG_UI_CONFIG };

	CMyTreeView tree;

	BEGIN_MSG_MAP(CPropertyDlgUILayout)
		MSG_WM_INITDIALOG(OnInitDialog)
		//COMMAND_ID_HANDLER(IDC_BTN_ADD,OnBtnAdd)
		//COMMAND_ID_HANDLER(IDC_BTN_DEL,OnBtnDel)
		//NOTIFY_HANDLER_EX(IDC_MEDIA_LIST,LVN_ITEMCHANGED,OnItemChanged)
		//MESSAGE_HANDLER(WM_CONFIGTOSAVE,OnCfgToSave)
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()

	MYTREE *MyRoot;

	BOOL OnInitDialog(CWindow wndFocus, LPARAM lInitParam);
	//LRESULT OnBtnAdd(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	//LRESULT OnBtnDel(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

	void TraverseSplitTreeAndShowInTreeView(MYTREE *cur,HTREEITEM item)
	{
		tree.TraverseSplitTreeAndShowInTreeView(cur,item);
	}
	void UpdateTree(MYTREE *treeData)
	{
		if(tree.IsWindow())
			tree.UpdateTree(treeData);
	}


};
