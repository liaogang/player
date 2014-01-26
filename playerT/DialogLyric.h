#include "CMyLyric.h"
#pragma once


class CDialogLyric : 
	public CDialogImpl<CDialogLyric>,
	public CDialogResize<CDialogLyric>,
	public CMessageFilter
{
public:
	CMyLyricWnd m_wndLyric;

	enum { IDD = IDD_DIALOGLRC};

	BEGIN_MSG_MAP_EX(CDialogLyric)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MSG_WM_SIZE(OnSize)
		COMMAND_ID_HANDLER(IDOK, OnCloseCmd)
		COMMAND_ID_HANDLER(IDCANCEL, OnCloseCmd)
		CHAIN_MSG_MAP(CDialogResize<CDialogLyric>)
	END_MSG_MAP()

	BEGIN_DLGRESIZE_MAP(CDialogResize<CDialogLyric>)
	END_DLGRESIZE_MAP()

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
	{
		DlgResize_Init(FALSE,FALSE);

		CMessageLoop* pLoop = _Module.GetMessageLoop();
		ATLASSERT(pLoop != NULL);
		pLoop->AddMessageFilter(this);

		RECT rc;
		GetClientRect(&rc);
		m_wndLyric.Create(m_hWnd,rc,NULL,WS_CHILD|WS_VISIBLE);
		//m_wndLyric.SubclassWindow(GetDlgItem(IDC_LYRIC_SHOW));
	
		foo();

		bHandled=FALSE;
		return 0;
	}

	virtual BOOL PreTranslateMessage(MSG* pMsg)
	{
		//让非模态对话框处理模态对话框的消息
		//如Tab,Esc,Enter...
		return IsDialogMessage(pMsg);
	}

	void OnSize(UINT nType, CSize size)
	{
		RECT rect;
		GetClientRect(&rect);
		::SetWindowPos(m_wndLyric, NULL, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top,SWP_NOZORDER);
	}

	void foo()
	{
		RECT rc;
		GetClientRect(&rc);

		InvalidateRect(&rc,1);

		COLORREF cl=RGB(0,78,152);

		SetWindowLong(GWL_EXSTYLE,GetWindowLong(GWL_EXSTYLE)|WS_EX_LAYERED);

		typedef BOOL (WINAPI *FSetLayeredWindowAttributes)(HWND,COLORREF,BYTE,DWORD);

		FSetLayeredWindowAttributes SetLayeredWindowAttributes;

		HINSTANCE hInst = LoadLibrary(_T("User32.DLL"));

		SetLayeredWindowAttributes = (FSetLayeredWindowAttributes)GetProcAddress(hInst,"SetLayeredWindowAttributes");

		if (SetLayeredWindowAttributes)
			SetLayeredWindowAttributes(m_hWnd,cl,200 ,LWA_COLORKEY|LWA_ALPHA);//|LWA_ALPHA

		FreeLibrary(hInst);
	}



	LRESULT OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		Hide();
		return 0;
	}

	void Hide()
	{
		ShowWindow(SW_HIDE);
		m_wndLyric.StopShowLyric();
	}

	void Show()
	{
		m_wndLyric.PrepareShowLyric();
		ShowWindow(SW_SHOW);
	}
};





// class CWndLyric:public CMyLyricImpl<CWndLyric>
// {
// public:
// 	DECLARE_WND_CLASS( _T( "MyLyricsView" ) )
// 	//DECLARE_WND_CLASS_EX(_T("MyLyricsView"), CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS,COLOR_BTNSHADOW)
// 
// 	BEGIN_MSG_MAP_EX(CWndLyric)
// 		CHAIN_MSG_MAP(CMyLyricImpl<CWndLyric>)
// 	END_MSG_MAP()
// };
