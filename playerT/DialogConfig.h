#pragma once
#include "ProgertyDlg.h"
#include "customMsg.h"

#include <vector>
#include <map>
using namespace std;

//---------------------------------------
#define DECLARE_MYRUNTIMECLASS() \
	CWindow* CreateObject(tstring str)\
{
#define ADD_MYRUNTIMECLASS(str,classname)\
	if (str==_T("##classname##"))\
{\
	return new classname##();\
}
#define END_MYRUNTIMECLASS() \
}
//---------------------------------------

//in the  class T
//a function replace() must be implement
//a rountine to 用CRuntimeClass* plcass 创建窗口返回其指针
template <class T,BOOL f>
class CMyWindowMapImpl
{
protected:
	map<tstring,CWindow*> _datamap;
	CWindow*              pCurrentWnd;

	//void InsertToDataMap(tstring str,CRuntimeClass* pClass)
	//{
	//	_datamap.insert(str,pClass);
	//}

	void InsertToDataMap(tstring str,CWindow* pClass)
	{
		_datamap.insert(str,pClass);
	}

	CWindow* GetMapWindow(tstring& str)
	{
		//WinMData d;
		//d=_datamap.find(str);
		//{
		//	assert(IsWindow(d.pWnd->m_hWnd));
		//	return d.pWnd;
		//}
	}


};



class CMyTreeCtrl
	:public CWindowImpl<CMyTreeCtrl,CTreeViewCtrlEx>
	//,public CMyWindowMapImpl<CMyTreeCtrl,TRUE>
{
	//DECLARE_EMPTY_MSG_MAP()

	// 	DECLARE_MYRUNTIMECLASS()
	// 		ADD_MYRUNTIMECLASS(_T("summers"),CPropertyDlg1)
	// 	END_MYRUNTIMECLASS()
public:
	BEGIN_MSG_MAP(CMyTreeCtrl)
		//NOTIFY_HANDLER(IDC_TREE, TVN_SELCHANGED, OnTvnSelchangedTree)
	END_MSG_MAP()

	// 
	// public:
	// 	HWND m_hwnd1,m_hwnd2;
	// 
	// public:
	// 	LRESULT OnTvnSelchangedTree(int idCtrl, LPNMHDR pnmh, BOOL& bHandled)
	// 	{
	// 		LPNMTREEVIEW pNMTreeView=reinterpret_cast<LPNMTREEVIEW>(pnmh);
	// 		//HTREEITEM item;
	// 		
	// 		tstring str1=pNMTreeView->itemNew.pszText;
	// 		if (str1.compare(_T("The Master"))==0)
	// 		{
	// 			::ShowWindow(m_hwnd1,SW_SHOW);
	// 		}
	// 		else if (str1.compare(_T("Darla"))==0)
	// 		{
	// 			::ShowWindow(m_hwnd2,SW_SHOW);
	// 		}
	// 		return 0;
	// 	}
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
		//cus msg
		COMMAND_ID_HANDLER(ID_PLAY,OnCfgToSave)
	END_MSG_MAP()

	BEGIN_DDX_MAP(CDialogConfig)
		DDX_CONTROL(IDC_TREE,m_tree)
	END_DDX_MAP()

public:

	// Handler prototypes (uncomment arguments if needed):
	//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)


	
	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		CenterWindow(GetParent());
		DoDataExchange(FALSE);
		InitDlgTree();
		return TRUE;
	}

	CPropertyDlg* pDlg;
	CPropertyDlg1* pDlg1;
	CPropertyDlgMediaLib* pDlgMedia;
	CPropertyDlgLyricsLib *pDlgLyrics;

	LRESULT OnCfgToSave(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		HWND hWnd=(HWND)m_tree.GetItemData(m_tree.GetSelectedItem());
		if (::IsWindow(hWnd))
			::PostMessage(hWnd,WM_CONFIGTOSAVE,NULL,NULL);	
		return 0;
	}

	void InitDlgTree()
	{
		HTREEITEM item;

		std::tstring tmp(_T("summers"));
		pDlg=new CPropertyDlg;
		pDlg->title=tmp;

		std::tstring tmp1(_T("The Master"));
		pDlg1=new CPropertyDlg1;
		pDlg1->title=tmp1;

		std::tstring tmp2(_T("媒体库"));
		pDlgMedia=new CPropertyDlgMediaLib;
		pDlgMedia->title=tmp2;
		

		std::tstring tmp3(_T("歌词"));
		pDlgLyrics=new CPropertyDlgLyricsLib;
		pDlgLyrics->title=tmp3;

		item=m_tree.InsertItem(tmp.c_str(),TVI_ROOT,TVI_LAST);
			m_tree.InsertItem (tmp1.c_str(), item, TVI_LAST );
			m_tree.InsertItem (tmp2.c_str(), item, TVI_LAST );
			m_tree.InsertItem (tmp3.c_str(), item, TVI_LAST );
		
		
		item=m_tree.GetRootItem();
		m_tree.Expand(item);
	}




#define GETDLGWND(pDlg)\
	if (title==pDlg->title)\
	{\
		if (!pDlg->IsWindow())\
			pDlg->Create(m_hWnd);\
		return pDlg->m_hWnd;\
	}

	HWND GetPropertyDlgHwnd(std::tstring &title)
	{
		GETDLGWND(pDlg)
		GETDLGWND(pDlg1)
		GETDLGWND(pDlgMedia)
		GETDLGWND(pDlgLyrics)
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

		if(hWndOld &&::IsWindow(hWndOld))
			::ShowWindow(hWndOld,SW_HIDE);

		if(!(hWndNew &&::IsWindow(hWndNew)))
		{
			LPTSTR str=new TCHAR[256];
			m_tree.GetItemText(pNMTreeView->itemNew.hItem,str,256);
			std::tstring str1(str);
			hWndNew=GetPropertyDlgHwnd(str1);
			if (hWndNew)
				m_tree.SetItemData(itemNew,(WORD)hWndNew);
		}


		if(hWndNew &&::IsWindow(hWndNew))
		{
			::ShowWindow(hWndNew,SW_SHOW);
			::SetWindowPos(hWndNew,NULL,156,1,NULL,NULL,SWP_NOSIZE|SWP_NOZORDER);
		}
		
		return 0;
	}


	LRESULT OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		ShowWindow(SW_HIDE);
		return 0;
	}

protected:
	CMyTreeCtrl m_tree;
};