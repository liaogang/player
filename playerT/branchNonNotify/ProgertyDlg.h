#include "stdafx.h"
//基类属性面板树右边的对话框
class CPropertyDlg:
	public CDialogImpl<CPropertyDlg>,
	public CWinDataExchange<CPropertyDlg>	
{
public:
	enum { IDD = IDD_DLG_EMPTY };

	BEGIN_MSG_MAP(CPropertyDlg)
		COMMAND_ID_HANDLER(IDCANCEL,OnCloseCmd)
	END_MSG_MAP()

	BEGIN_DDX_MAP(CPropertyDlg)
	END_DDX_MAP()

	LRESULT OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		ShowWindow(SW_HIDE);
		return 0;
	}
};

class CPropertyDlg1:public CDialogImpl<CPropertyDlg1>
	,public CWinDataExchange<CPropertyDlg1>
{
public:
	enum { IDD = IDD_DLG_CFG_1 };
	BEGIN_MSG_MAP(CPropertyDlg1)
		COMMAND_ID_HANDLER(IDCANCEL,OnCloseCmd)
	END_MSG_MAP()

	BEGIN_DDX_MAP(CPropertyDlg1)
	END_DDX_MAP()

	LRESULT OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		ShowWindow(SW_HIDE);
		return 0;
	}
};