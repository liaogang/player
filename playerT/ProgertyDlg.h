#include "stdafx.h"
#include "customMsg.h"
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
	public CWinDataExchange<CPropertyDlgMediaLib>
{
public:
	enum { IDD = IDD_DIALOG_MEDIAlIB };

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