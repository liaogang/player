#pragma once
#include "ProgertyDlg.h"


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
	,public CMyWindowMapImpl<CMyTreeCtrl,TRUE>
{
	//DECLARE_EMPTY_MSG_MAP()

	DECLARE_MYRUNTIMECLASS()
		ADD_MYRUNTIMECLASS(_T("summers"),CPropertyDlg1)
	END_MYRUNTIMECLASS()
public:
	BEGIN_MSG_MAP(CMyTreeCtrl)
		NOTIFY_HANDLER(IDC_TREE, TVN_SELCHANGED, OnTvnSelchangedTree)
	END_MSG_MAP()


public:
	HWND m_hwnd1,m_hwnd2;

public:
	LRESULT OnTvnSelchangedTree(int idCtrl, LPNMHDR pnmh, BOOL& bHandled)
	{
		LPNMTREEVIEW pNMTreeView=reinterpret_cast<LPNMTREEVIEW>(pnmh);
		HTREEITEM item;
		
		tstring str1=pNMTreeView->itemNew.pszText;
		if (str1.compare(_T("The Master"))==0)
		{
			::ShowWindow(m_hwnd1,SW_SHOW);
		}
		else if (str1.compare(_T("Darla"))==0)
		{
			::ShowWindow(m_hwnd2,SW_SHOW);
		}
		return 0;
	}
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
	END_MSG_MAP()

	BEGIN_DDX_MAP(CDialogConfig)
		DDX_CONTROL(IDC_TREE,m_tree)
	END_DDX_MAP()


public:
	HWND m_hwnd1,m_hwnd2;
	LRESULT OnTvnSelchangedTree(int idCtrl, LPNMHDR pnmh, BOOL& bHandled)
	{
		LPNMTREEVIEW pNMTreeView=reinterpret_cast<LPNMTREEVIEW>(pnmh);
		HTREEITEM item;

		tstring str1=pNMTreeView->itemNew.pszText;
		LPTSTR str=new TCHAR[125];
		m_tree.GetItemText(pNMTreeView->itemNew.hItem,str,125);
		//if (str==_T("The Master"))
		if (_tcscmp(str,_T("The Master"))==0)
		{
			::ShowWindow(m_hwnd2,SW_HIDE);
			::ShowWindow(m_hwnd1,SW_SHOW);
			::SetWindowPos(m_hwnd1,NULL,228,1,NULL,NULL,SWP_NOSIZE|SWP_NOZORDER);
		}
		else if (_tcscmp(str,_T("Darla"))==0)
		{
			::ShowWindow(m_hwnd1,SW_HIDE);
			::ShowWindow(m_hwnd2,SW_SHOW);
			::SetWindowPos(m_hwnd2,NULL,228,1,NULL,NULL,SWP_NOSIZE|SWP_NOZORDER);
		}
		return 0;
	}
	// Handler prototypes (uncomment arguments if needed):
	//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		CenterWindow(GetParent());

		//////////////////////////////////////////////////////////////////////////
		DoDataExchange(FALSE);
		HTREEITEM item1,item2;
		item1=m_tree.InsertItem(_T("summers"),TVI_ROOT,TVI_LAST);
			 m_tree.InsertItem ( _T("The Master"), item1, TVI_LAST );
			 m_tree.InsertItem ( _T("Darla"), item1, TVI_LAST );
			 m_tree.InsertItem ( _T("Angel"), item1, TVI_LAST );
			 m_tree.InsertItem ( _T("Drusilla"), item1, TVI_LAST );
			m_tree.InsertItem ( _T("Spike"), item1, TVI_LAST );

		item2 = m_tree.InsertItem ( _T("The Master"), TVI_ROOT, TVI_LAST );
			 m_tree.InsertItem ( _T("Darla"), item2, TVI_LAST );
			 m_tree.InsertItem ( _T("Angel"), item2, TVI_LAST );
			 m_tree.InsertItem ( _T("Drusilla"), item2, TVI_LAST );
			 m_tree.InsertItem ( _T("Spike"), item2, TVI_LAST );


			 CPropertyDlg* pDlg=new CPropertyDlg;
			 pDlg->Create(this->m_hWnd);

			 CPropertyDlg1* pDlg1=new CPropertyDlg1;
			 pDlg1->Create(this->m_hWnd);

			 m_hwnd1=pDlg->m_hWnd;

			 m_hwnd2=pDlg1->m_hWnd;

		return TRUE;
	}

	LRESULT OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		ShowWindow(SW_HIDE);
		return 0;
	}
protected:
	CMyTreeCtrl m_tree;
};