#pragma once
#include "ProgertyDlg.h"
#include "customMsg.h"

#include <vector>
using namespace std;


#define GETDLGWND(pDlg)\
	if (title==pDlg->title)\
{\
	if (!pDlg->IsWindow())\
		pDlg->Create(m_hWnd);\
	return pDlg->m_hWnd;\
}


class CMyTreeCtrl
	:public CWindowImpl<CMyTreeCtrl,CTreeViewCtrlEx>
{
public:
	BEGIN_MSG_MAP(CMyTreeCtrl)
	END_MSG_MAP()

};



class CDialogConfig :
	public CDialogImpl<CDialogConfig>,
	public CWinDataExchange<CDialogConfig>
{
public:
	enum { IDD = IDD_CONFIG };

	BEGIN_MSG_MAP(CDialogConfig)
		
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(IDOK, OnCloseCmd)
		COMMAND_ID_HANDLER(IDCANCEL, OnCloseCmd)
		NOTIFY_HANDLER(IDC_TREE, TVN_SELCHANGED, OnTvnSelchangedTree)
		COMMAND_ID_HANDLER(ID_PLAY,OnCfgToSave)		//cus msg
	END_MSG_MAP()

	BEGIN_DDX_MAP(CDialogConfig)
		DDX_CONTROL(IDC_TREE,m_tree)
	END_DDX_MAP()

public:
	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		CenterWindow(GetParent());
		DoDataExchange(FALSE);
		InitDlgTree();
		return TRUE;
	}

	LRESULT OnCfgToSave(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		HWND hWnd=(HWND)m_tree.GetItemData(m_tree.GetSelectedItem());
		if (::IsWindow(hWnd))
			::PostMessage(hWnd,WM_CONFIGTOSAVE,NULL,NULL);	
		return 0;
	}

	void InitDlgTree()
	{
		HTREEITEM firstItem;

		pDlg=new CPropertyDlg;
		pDlg->Init(_T("参数选项"),pDlg->m_hWnd);

		pDlg1=new CPropertyDlg1;
		pDlg1->Init(_T("The Master"),pDlg1->m_hWnd);

		pDlgMedia=new CPropertyDlgMediaLib;
		pDlgMedia->Init(_T("媒体库"),pDlgMedia->m_hWnd);
		
		pDlgLyrics=new CPropertyDlgLyricsLib;
		pDlgLyrics->Init(_T("歌词搜索"),pDlgLyrics->m_hWnd);

		pDlgLrcPane=new CPropertyDlgLrcPane;
		pDlgLrcPane->Init(_T("歌词面板"),pDlgLrcPane->m_hWnd);

		firstItem=AddDlg(pDlg,TVI_ROOT);
		AddDlg(pDlg1,TVI_ROOT);
		AddDlg(pDlgMedia,TVI_ROOT);
		AddDlg(pDlgLyrics,TVI_ROOT);
		AddDlg(pDlgLrcPane,TVI_ROOT);
		
		m_tree.Expand(TVI_ROOT);
	}


	HTREEITEM AddDlg(CDlgConfig *pdlg,HTREEITEM item)
	{
		return  m_tree.InsertItem (pdlg->title.c_str(), item, TVI_LAST );
	}

	//Make sure the windows is created
	HWND GetPropertyDlgHwnd(std::tstring &title)
	{
		GETDLGWND(pDlg)
		GETDLGWND(pDlg1)
		GETDLGWND(pDlgMedia)
		GETDLGWND(pDlgLyrics)
		GETDLGWND(pDlgLrcPane)
		return NULL;
	}

	LRESULT OnTvnSelchangedTree(int idCtrl, LPNMHDR pnmh, BOOL& bHandled)
	{
		LPNMTREEVIEW pNMTreeView=reinterpret_cast<LPNMTREEVIEW>(pnmh);
		HTREEITEM itemNew,itemOld;
		HWND hWndNew,hWndOld;

		itemNew=pNMTreeView->itemNew.hItem;
		itemOld=pNMTreeView->itemOld.hItem;

		hWndNew=(HWND)m_tree.GetItemData(itemNew);
		hWndOld=(HWND)m_tree.GetItemData(itemOld);

		if(!(hWndNew &&::IsWindow(hWndNew)))
		{
			TCHAR str[256];
			m_tree.GetItemText(pNMTreeView->itemNew.hItem,str,256);
			std::tstring str1(str);
			hWndNew=GetPropertyDlgHwnd(str1);
			if (hWndNew)
				m_tree.SetItemData(itemNew,(WORD)hWndNew);
		}

		
		SwitchPropertyDlg(hWndNew,hWndOld);
		
		return 0;
	}


	void ShowDlgByName(wstring &treeName)
	{
		HWND hWndNew=GetPropertyDlgHwnd(treeName);
		HWND hWndOld=(HWND)m_tree.GetItemData(m_tree.GetSelectedItem());

		SwitchPropertyDlg(hWndNew,hWndOld);


		//set focus in tree item to newItem
		m_treeName=treeName;
		GetAllChildItem(TVI_ROOT);
	}

	wstring m_treeName;

	HTREEITEM GetAllChildItem(HTREEITEM parent)
	{
		if(parent==NULL)return NULL;

		HTREEITEM item=NULL;
		item=m_tree.GetChildItem(parent);
		while(item!=NULL)
		{
			//-----------------------------------------//
			//do something
			TCHAR str[256];
			memset(str,0,sizeof(str));
			m_tree.GetItemText(item,str,256);
			if (_tcscmp(const_cast<const TCHAR*>(str),m_treeName.c_str())==0)
			{
				m_tree.SelectItem(item);
				return NULL;//break;
			}
			//-----------------------------------------//

			GetAllChildItem(item);
			item=m_tree.GetNextSiblingItem(item);
		}

		return item;
	}





	void SwitchPropertyDlg(HWND hWndNew,HWND hWndOld)
	{
		if(hWndOld &&::IsWindow(hWndOld))
			::ShowWindow(hWndOld,SW_HIDE);

		if(hWndNew &&::IsWindow(hWndNew))
		{
			::ShowWindow(hWndNew,SW_SHOW);
			::SetWindowPos(hWndNew,NULL,156,1,NULL,NULL,SWP_NOSIZE|SWP_NOZORDER);
		}
	}

	LRESULT OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{	
		if(wID==IDOK)
		{
			HWND hWnd=(HWND)m_tree.GetItemData(m_tree.GetSelectedItem());
			if (::IsWindow(hWnd))
				::PostMessage(hWnd,WM_CONFIGTOSAVE,NULL,NULL);	
		}

		ShowWindow(SW_HIDE);
		return 0;
	}

protected:
	CMyTreeCtrl m_tree;
	CPropertyDlg* pDlg;
	CPropertyDlg1* pDlg1;
	CPropertyDlgMediaLib* pDlgMedia;
	CPropertyDlgLyricsLib *pDlgLyrics;
	CPropertyDlgLrcPane   *pDlgLrcPane;
};