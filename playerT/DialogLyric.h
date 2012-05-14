// aboutdlg.h : interface of the CAboutDlg class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once
#include "LrcMng.h"
class CDialogLyric : public CDialogImpl<CDialogLyric>
{
public:
	enum { IDD = IDD_DIALOGLRC};

	BEGIN_MSG_MAP(CDialogLyric)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(IDOK, OnCloseCmd)
		COMMAND_ID_HANDLER(IDCANCEL, OnCloseCmd)
		MESSAGE_HANDLER( WM_PAINT , OnPaint)
		MESSAGE_HANDLER(WM_TRACKPOS,OnPos)
		MESSAGE_HANDLER(WM_SIZE,OnSize)
	END_MSG_MAP()

	// Handler prototypes (uncomment arguments if needed):
	//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	RECT rc ,lrcRect;
	SIZE sz;
	std::tstring lrcText;
	int lrcLines,lrcTextHeight,lrcSpare,lrcHeight;
	

	LRESULT OnSize(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		GetClientRect(&rc);
		bHandled=FALSE;
		return 0;
	}
LrcMng *mng;
	double used,lefted;
	vector<LrcLine>::iterator preLine,lastLine;
	RECT tRc;
	LRESULT OnPos(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{

		used=wParam;
		lefted=lParam;
		
		//used= used%sz            //以行高为基数，取整。
		double lrcContent=used+lefted;
		lrcRect=rc;
		lrcRect.top=  (rc.bottom-rc.top)/2 - (used/lrcContent) * lrcLines*lrcHeight;



		//-----------------------------------------
		vector<LrcLine>::iterator i;
		for (i=mng->lib.begin(),lastLine=i,preLine=i,lrcLines=0;i!=mng->lib.end();preLine=i,++i,++lrcLines)
		{
			
			if (used < (*i).time.GetTotalSec() &&used > (*preLine).time.GetTotalSec() )
			{
				if (lastLine!=preLine)
				{
					Invalidate(TRUE);
					::DrawText(GetDC(),(*preLine).text.c_str(),(*preLine).text.length(),&tRc,DT_CENTER);
					lastLine=preLine;
				}
				break;
			}
			//lrcText+=(*i).text+_T("\n");
		}
		//-----------------------------------------

		bHandled=FALSE;
		return 0;
	}

	LRESULT OnPaint(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		::DrawText(GetDC(),(*preLine).text.c_str(),(*preLine).text.length(),&tRc,DT_CENTER);
		//::DrawText(GetDC(),lrcText.c_str(),lrcText.length(),&lrcRect,DT_CENTER);
		bHandled=FALSE;
		return 1;
	}

	
	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		CenterWindow(GetParent());
		GetTextExtentPoint32(GetDC(),_T("A"),1,&sz); 
		lrcTextHeight=sz.cy;
		lrcSpare=3;
		lrcHeight=lrcTextHeight+lrcSpare;


		mng=LrcMng::Get();
		mng->Open(_T("D:\\lrc\\林凡 - 一个人生活.lrc"));
		preLine=mng->lib.begin();
		GetClientRect(&tRc);
		
		

		lrcRect.bottom=0;
		lrcRect.left=0;
		lrcRect.top=0;
		lrcRect.right=0;

		rc.bottom=0;
		rc.left=0;
		rc.top=0;
		rc.right=0;

		return TRUE;
	}

	LRESULT OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		ShowWindow(SW_HIDE);
		return 0;
	}
};
