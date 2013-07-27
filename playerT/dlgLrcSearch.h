#include "SearchLyric.h"

#pragma once
class CDlgLrcSearch;
class CListLrcReportView:
	public CWindowImpl<CListLrcReportView,CListViewCtrl>
{
public:
	CDlgLrcSearch *parent;

	BEGIN_MSG_MAP_EX(CListLrcReportView)
		MSG_WM_LBUTTONDBLCLK(OnDbClicked)
	END_MSG_MAP()

		
 		void ReFillPlaylist();
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
				_T("标题"),
				_T("艺术家")};

				const UINT alignment[]={
					LVCFMT_LEFT,
					LVCFMT_RIGHT};

					const TCHAR* columnNamePlaceHoder[]={
						_T("标题标题标题标题标题题标题标题标题"),
						_T("艺术家艺术家")};

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




class CDlgLrcSearch :
	public CDialogImpl<CDlgLrcSearch>,	
	public CMessageFilter
{
public:
	enum { IDD = IDD_DLG_LRC_SEARCH };


	virtual BOOL PreTranslateMessage(MSG* pMsg)
	{
		//让非模态对话框处理模态对话框的消息
		//如Tab,Esc,Enter...
		return IsDialogMessage(pMsg);
	}

	BEGIN_MSG_MAP(CDlgLrcSearch)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(IDOK, OnOK)
		COMMAND_ID_HANDLER(IDCANCEL, OnCloseCmd)
		COMMAND_ID_HANDLER(IDC_BTN_SEARCH,OnSearch)
	END_MSG_MAP()

	
	CListLrcReportView m_list;
	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		CenterWindow(GetParent());

		m_list.SubclassWindow(::GetDlgItem(m_hWnd,IDC_LIST));
		m_list.Init();
		m_list.parent=this;
		savePath=NULL;

		::SendMessage(GetDlgItem(IDC_CHECK), BM_SETCHECK, BST_CHECKED, 0L);

		CMessageLoop* pLoop = _Module.GetMessageLoop();
		ATLASSERT(pLoop != NULL);
		pLoop->AddMessageFilter(this);

		return TRUE;
	}
	
	

	LRESULT OnOK(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		HWND hwnd=GetFocus();
		if (hwnd==GetDlgItem(IDC_EDIT_TI) ||
			hwnd==GetDlgItem(IDC_EDIT_AR))
		{
			int i;
			OnSearch(0,0,0,i);
		}
		else
			DownLoad();


		return 0;
	}


	LRESULT OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		savePath=NULL;
		ShowWindow(SW_HIDE);
		return 0;
	}

	SearchLyric searchTool;
	BOOL searchSuccess;
	LRESULT OnSearch(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		const WCHAR searching[]={L"..."};
		const WCHAR search[]={L"搜 索"};

		::EnableWindow(GetDlgItem(IDC_BTN_SEARCH),FALSE);
		::SetWindowText(GetDlgItem(IDC_BTN_SEARCH),searching);

		m_list.DeleteAllItems();

		searchTool.Init();

		WCHAR artist[MAX_PATH],title[MAX_PATH];
		::GetWindowText(GetDlgItem(IDC_EDIT_AR),artist,sizeof(artist));
		::GetWindowText(GetDlgItem(IDC_EDIT_TI),title,sizeof(title));
		if (artist[0]=='\0' &&title[0]=='\0')
		{
			::SetWindowText(GetDlgItem(IDC_EDIT_INFO),L"请填写艺术家与标题栏");
			return 0;
		}

		::SetWindowText(GetDlgItem(IDC_EDIT_INFO),L"正在搜索歌词");
		if(searchSuccess=searchTool.Search(artist,title))
		{
			if(searchSuccess=searchTool.ParseResult())
			{
				m_list.ReFillPlaylist();
				::SetWindowText(GetDlgItem(IDC_EDIT_INFO),L"请选择歌词文件进行下载");
			}
			else
			{
				::SetWindowText(GetDlgItem(IDC_EDIT_INFO),L"未发现相匹配的歌词");
			}
		}
		else
		{
			::SetWindowText(GetDlgItem(IDC_EDIT_INFO),L"搜索失败,请检查网络");
		}

		::SetWindowText(GetDlgItem(IDC_BTN_SEARCH),search);
		::EnableWindow(GetDlgItem(IDC_BTN_SEARCH),TRUE);
		return 0;
	}
	
	void DownLoad();

	void ReInit(WCHAR *ar,WCHAR *ti,WCHAR *savePath)
	{
		m_list.DeleteAllItems();

		if(savePath)
			{this->savePath=new WCHAR[wcslen(savePath)];
			 wcscpy(this->savePath,savePath);}

	
		::SetWindowText(GetDlgItem(IDC_EDIT_AR),ar);
		::SetWindowText(GetDlgItem(IDC_EDIT_TI),ti);
		::SetWindowText(GetDlgItem(IDC_EDIT_INFO),L"");
	}

	inline void HideSelf(){ShowWindow(SW_HIDE);}
	void ShowSelf()
	{
		ShowWindow(SW_SHOW);
	}

	WCHAR *savePath;
};
