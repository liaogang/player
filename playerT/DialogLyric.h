// aboutdlg.h : interface of the CAboutDlg class
//
/////////////////////////////////////////////////////////////////////////////
#include <vector>
#include "LrcMng.h"
#include "MyLib.h"
#include "PlayList.h"

#pragma once
class CDialogLyric : public CDialogImpl<CDialogLyric>
{
public:
	enum { IDD = IDD_DIALOGLRC};

	BEGIN_MSG_MAP_EX(CDialogLyric)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		COMMAND_ID_HANDLER(IDOK, OnCloseCmd)
		COMMAND_ID_HANDLER(IDCANCEL, OnCloseCmd)
		MSG_WM_PAINT(OnPaint)
		MESSAGE_HANDLER(WM_TRACKPOS,OnPos)
		MESSAGE_HANDLER(WM_SIZE,OnSize)
		MESSAGE_HANDLER(WM_TRACKPOS)
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
	
	double used,lefted;
	vector<LrcLine>::iterator preLine,lastLine;
	RECT tRc;
	LRESULT OnPos(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled=FALSE;
		if(!::IsWindowVisible(m_hWnd))
			return 0;

		if(!bLrcReady)
			return 0;
		LrcMng *mng=LrcMng::Get();

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
					RECT eraseRC={};
					eraseRC.left=tRc.left;
					eraseRC.top=tRc.top;
					eraseRC.right=rc.right;
					eraseRC.bottom=sz.cy+eraseRC.top;
					InvalidateRect(&eraseRC);
					::DrawText(GetDC(),(*preLine).text.c_str(),(*preLine).text.length(),&tRc,DT_CENTER);
					lastLine=preLine;
				}
				break;
			}
			//lrcText+=(*i).text+_T("\n");
		}
		//-----------------------------------------

		
		return 0;
	}

	void OnPaint(CDCHandle dc)
	{
		PAINTSTRUCT ps;
		::BeginPaint(m_hWnd,&ps);
		if( bLrcReady)
			::DrawText(ps.hdc,(*preLine).text.c_str(),(*preLine).text.length(),&tRc,DT_CENTER);

		::EndPaint(m_hWnd,&ps);	
	}

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		CenterWindow(GetParent());
		GetTextExtentPoint32(GetDC(),_T("A"),1,&sz); 
		lrcTextHeight=sz.cy;
		lrcSpare=3;
		lrcHeight=lrcTextHeight+lrcSpare;

		GetClientRect(&tRc);
		

		lrcRect.bottom=0;
		lrcRect.left=0;
		lrcRect.top=0;
		lrcRect.right=0;

		rc.bottom=0;
		rc.left=0;
		rc.top=0;
		rc.right=0;

		bLrcReady=FALSE;
		return TRUE;
	}

	BOOL bLrcReady;
	//void TrackChanged();
	void TrackChanged()
	{
		PlayListItem* track=MyLib::shared()->ActivePlaylist()->curTrack();
		track->GetLrcFileFromLib();
		if (!track->lyric.empty())
		{
			LrcMng *sLM=LrcMng::Get();
			sLM->Open((LPTSTR)track->lyric.c_str());

			SetWindowText((LPTSTR)track->lyric.c_str());
			bLrcReady=TRUE;
		}
	}

	LRESULT OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		ShowWindow(SW_HIDE);
		return 0;
	}
};
