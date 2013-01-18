class CDialogConsole : 
	public CDialogImpl<CDialogConsole>,
	public CDialogResize<CDialogConsole>,
	public CMessageFilter
{
public:

	virtual BOOL PreTranslateMessage(MSG* pMsg)
	{
		//让非模态对话框处理模态对话框的消息
		//如Tab,Esc,Enter...
		return IsDialogMessage(pMsg);
	}

	enum { IDD = IDD_DLG_CONSOLE};

	BEGIN_MSG_MAP_EX(CDialogConsole)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(IDOK, OnCloseCmd)
		COMMAND_ID_HANDLER(IDCANCEL, OnCloseCmd)
		CHAIN_MSG_MAP(CDialogResize<CDialogConsole>)
	END_MSG_MAP()

	BEGIN_DLGRESIZE_MAP(CDialogResize<CDialogConsole>)
		DLGRESIZE_CONTROL(IDC_EDIT,DLSZ_SIZE_X)
		DLGRESIZE_CONTROL(IDC_EDIT,DLSZ_SIZE_Y)
		DLGRESIZE_CONTROL(IDC_BTN_CLR,DLSZ_MOVE_Y)
		DLGRESIZE_CONTROL(IDC_CHECK,DLSZ_MOVE_Y)
	END_DLGRESIZE_MAP()

		LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
		{
			DlgResize_Init(FALSE,FALSE);
			CenterWindow(GetParent());

			CMessageLoop* pLoop = _Module.GetMessageLoop();
			ATLASSERT(pLoop != NULL);
			pLoop->AddMessageFilter(this);

			bHandled=FALSE;
			return 0;
		}


		LRESULT OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
		{
			ShowWindow(SW_HIDE);
			return 0;
		}
};