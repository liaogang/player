// aboutdlg.h : interface of the CAboutDlg class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once
class CMainFrame;
class CProcessingDlg : public CDialogImpl<CProcessingDlg>
{
public:
	enum { IDD = IDD_PROCESSING };

	BEGIN_MSG_MAP(CProcessingDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(IDOK, OnCloseCmd)
		COMMAND_ID_HANDLER(IDCANCEL, OnCloseCmd)
		COMMAND_ID_HANDLER(IDC_BUTTON_ABORT,OnAbort)
	END_MSG_MAP()

	// Handler prototypes (uncomment arguments if needed):
	//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)
	
	//IDC_PROGRESS
	//IDC_EDIT
	//IDC_STATIC1



	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		CenterWindow(GetParent());
		
		::ShowWindow(GetDlgItem(IDC_STATIC1),SW_HIDE);

		return TRUE;
	}

	LRESULT OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		ShowWindow(SW_HIDE);
		return 0;
	}


	PlayList*   pList;
	LRESULT OnAbort(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);

	void ShowFileName(TCHAR* filename,BOOL started)
	{
		if (started==1)
		{
			::ShowWindow(GetDlgItem(IDC_STATIC1),SW_SHOW);
		}
		else if (started==0)
		{
			::ShowWindow(GetDlgItem(IDC_STATIC1),SW_HIDE);
			::SetWindowText(GetDlgItem(IDC_EDIT),NULL);
			ShowWindow(SW_HIDE);
		}
		else if(started==2)
		{
			::SetWindowText(GetDlgItem(IDC_EDIT),filename);	
		}

	}




};
