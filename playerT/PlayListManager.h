#pragma once
class CMainFrame;
class CPlayListManager:
	public CWindowImpl<CPlayListManager,CListViewCtrl>
{
public:
	BEGIN_MSG_MAP_EX(CPlayListManager)
		MSG_WM_LBUTTONDBLCLK(OnDbClicked)
	END_MSG_MAP()

	CMainFrame *pMain;
    void ReFillPlaylist();
	void AddPlayList(PlayList *pPL);
	void DelPlayList(PlayList *pPL);
	void UpdateByPLTrack(PlayList *pPL,int trackNum);

	void ClearAllSel()
	{
		int i=-1;
		while ( (i=GetNextItem(i,LVIS_SELECTED)) != -1)
			SetItemState(i,0,LVNI_SELECTED|LVNI_FOCUSED );
	}

	void Init()
	{	
		SetExtendedListViewStyle(GetExtendedListViewStyle()|LVS_EX_FULLROWSELECT|LVS_EX_HEADERDRAGDROP);

		const TCHAR * columnName[]={
			_T("播放列表名称"),
			_T("项目"),
			_T("属性")};

		const UINT alignment[]={
			LVCFMT_LEFT,
			LVCFMT_LEFT,
			LVCFMT_RIGHT};

		const TCHAR* columnNamePlaceHoder[]={
			_T("                                   "),
			_T("        Artist"),
			_T("        属性")};

		const int cxOffset = 15;

		for (int i=0;i<sizeof(columnName)/sizeof(int);++i)
			AddColumn(columnName[i],i,-1, LVCF_FMT| LVCF_WIDTH|LVCF_TEXT|LVCF_SUBITEM ,alignment[i],GetStringWidth(columnNamePlaceHoder[i])+cxOffset);
	}

	int AddColumn(LPCTSTR strItem, int nItem, int nSubItem = -1,
		int nMask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM,
		int nFmt = LVCFMT_LEFT,int stringWidth=20)
	{

		ATLASSERT(::IsWindow(m_hWnd));
		LVCOLUMN lvc = { 0 };
		lvc.mask = nMask;
		lvc.fmt = nFmt;
		lvc.pszText = (LPTSTR)strItem;
		lvc.cx = stringWidth ;
		if(nMask & LVCF_SUBITEM)
			lvc.iSubItem = (nSubItem != -1) ? nSubItem : nItem;
		return InsertColumn(nItem, &lvc);
	}
	BOOL GetFirstSelItem()
	{
		int nItem=-1;
		if (GetItemCount()>0)
			nItem=GetNextItem(nItem,LVNI_SELECTED);

		return nItem;
	}

	LRESULT OnDbClicked(UINT i,CPoint pt);
	
};



class DialogPLManager :
	public CDialogImpl<DialogPLManager>,
	public CDialogResize<DialogPLManager>
{
public:
	enum { IDD = IDD_DLG_PL_MNG };
	BEGIN_MSG_MAP(DialogPLManager)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(IDOK, OnCloseCmd)
		COMMAND_ID_HANDLER(IDCANCEL, OnCloseCmd)
		CHAIN_MSG_MAP(CDialogResize<DialogPLManager>)
		REFLECT_NOTIFICATIONS()
	END_MSG_MAP()

	BEGIN_DLGRESIZE_MAP(DialogPLManager)
		DLGRESIZE_CONTROL(IDC_LIST,DLSZ_SIZE_X)
		DLGRESIZE_CONTROL(IDC_LIST,DLSZ_SIZE_Y)
	END_DLGRESIZE_MAP()

	CPlayListManager m_list;
	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		DlgResize_Init(FALSE,FALSE);
		CenterWindow(GetParent());
		m_list.SubclassWindow(::GetDlgItem(m_hWnd,IDC_LIST));
		m_list.Init();
		m_list.ReFillPlaylist();

		return TRUE;
	}


	LRESULT OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		HideSelf();
		return 0;
	}

	void HideSelf()
	{
		ShowWindow(SW_HIDE);
	}

	void ShowSelf()
	{	
		ShowWindow(SW_SHOW);
	}
};