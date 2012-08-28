// aboutdlg.h : interface of the CAboutDlg class
//
/////////////////////////////////////////////////////////////////////////////
// #include <vector>
// #include "LrcMng.h"
// #include "MyLib.h"
// #include "PlayList.h"

//#include "MyControls.h"
#pragma once
#include "CMyLyric.h"

class CDialogLyric : 
	public CMyLyric<CDialogImpl<CDialogLyric>>,
	public CDialogResize<CDialogLyric>
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
		DlgResize_Init(TRUE,FALSE);
		bHandled=FALSE;
		return 0;
	}
};





class CWndLyric:
	public CMyLyric<CWindowImpl<CWndLyric>>
{
public:
	BEGIN_MSG_MAP_EX(CWndLyric)
		CHAIN_MSG_MAP(CMyLyric<CWindowImpl<CWndLyric>>)
	END_MSG_MAP()
};