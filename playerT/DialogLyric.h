#include "CMyLyric.h"
#pragma once


class CDialogLyric : 
	public CMyLyric<CDialogImpl<CDialogLyric>>,
	public CDialogResize<CDialogLyric>,
	public CMessageFilter
{
public:
	enum { IDD = IDD_DIALOGLRC};

	BEGIN_MSG_MAP_EX(CDialogLyric)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		CHAIN_MSG_MAP(CMyLyric<CDialogImpl<CDialogLyric>>)
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

		TrackChanged();
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

};





class CWndLyric:
	public CMyLyric<CWindowImpl<CWndLyric>>
{
public:
	DECLARE_WND_CLASS_EX(_T("MyLyricsView"),CS_DBLCLKS,COLOR_BTNSHADOW)

	BEGIN_MSG_MAP_EX(CWndLyric)
		CHAIN_MSG_MAP(CMyLyric<CWindowImpl<CWndLyric>>)
	END_MSG_MAP()
};