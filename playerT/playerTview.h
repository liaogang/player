// playerTView.h : interface of the CPlayerTView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_PLAYERTVIEW_H__D65C5661_5A18_4B78_84DC_AC92BF42C671__INCLUDED_)
#define AFX_PLAYERTVIEW_H__D65C5661_5A18_4B78_84DC_AC92BF42C671__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CPlayerTView : public CWindowImpl<CPlayerTView, CListBox>
{
public:
	DECLARE_WND_SUPERCLASS(NULL, CListBox::GetWndClassName())

	BOOL PreTranslateMessage(MSG* pMsg)
	{
		pMsg;
		return FALSE;
	}


	 void OnInitialUpdate() // called first time after construct
	 {


		TextOut(GetDC(),50,50,_T("Hello world"),11); 


	 }
	
	BEGIN_MSG_MAP(CPlayerTView)
	END_MSG_MAP()

	
	

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PLAYERTVIEW_H__D65C5661_5A18_4B78_84DC_AC92BF42C671__INCLUDED_)
