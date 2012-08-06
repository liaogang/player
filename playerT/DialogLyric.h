// aboutdlg.h : interface of the CAboutDlg class
//
/////////////////////////////////////////////////////////////////////////////
#include <vector>
#include "LrcMng.h"
#include "MyLib.h"
#include "PlayList.h"

#pragma once
class CDialogLyric : 
	public CDialogImpl<CDialogLyric>,
	public CDialogResize<CDialogLyric>
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
		CHAIN_MSG_MAP(CDialogResize<CDialogLyric>)
	END_MSG_MAP()


	BEGIN_DLGRESIZE_MAP(CDialogLyric)
		DLGRESIZE_CONTROL(IDC_BUTTON1,DLSZ_SIZE_X)
		DLGRESIZE_CONTROL(IDC_BUTTON2,DLSZ_MOVE_X)
	END_DLGRESIZE_MAP()

	// Handler prototypes (uncomment arguments if needed):
	//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)
	RECT rc ,lrcRect;
	SIZE sz;
	std::tstring lrcText;
	int lrcLines,lrcTextHeight,lrcSpare,lrcHeight;
	BOOL bLrcReady;
	PlayListItem* track;
	double used,lefted;
	vector<LrcLine>::iterator preLine,lastLine;
	RECT tRc;


	LRESULT OnSize(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		GetClientRect(&rc);
		bHandled=FALSE;
		return 0;
	}
	

	LRESULT OnPos(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled=FALSE;
		if(!::IsWindowVisible(m_hWnd))
			return 0;

		if(!track || !track->m_bLrcFromLrcFile)
			return 0;
		

		used=wParam;
		lefted=lParam;
		
		//used= used%sz            //以行高为基数，取整。
		double lrcContent=used+lefted;
		lrcRect=rc;
		lrcRect.top=  (rc.bottom-rc.top)/2 - (used/lrcContent) * lrcLines*lrcHeight;

		//-----------------------------------------
		vector<LrcLine>::iterator i;
		for (i=track->lyricFromLrcFile.begin(),lastLine=i,preLine=i,lrcLines=0;
			i!=track->lyricFromLrcFile.end();
			preLine=i,++i,++lrcLines)
		{
			if (used < (*i).time.GetTotalSec() &&used > (*preLine).time.GetTotalSec() ){
				if (lastLine!=preLine){
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

		if(!track)
			return;

		if (!track->m_bLrcInner && !track->m_bLrcFromLrcFile)
			return;

		if(track->m_bLrcInner)
			::DrawText(ps.hdc,track->lyricInner.c_str(),track->lyricInner.length(),&rc,DT_CENTER);
		else if(track->m_bLrcFromLrcFile)
			::DrawText(ps.hdc,(*preLine).text.c_str(),(*preLine).text.length(),&tRc,DT_CENTER);

		::EndPaint(m_hWnd,&ps);	
	}

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		DlgResize_Init();

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

		track=NULL;
		bLrcReady=FALSE;
		return TRUE;
	}

	
	//track change , to get current song lyric
	void TrackChanged()
	{
		bLrcReady=FALSE;
		PlayList *playlist=MyLib::shared()->ActivePlaylist();
		if(!playlist) return;
		track=playlist->curTrack();
		if(!track) return;
		

		SetWindowText((LPTSTR)track->lycPath.c_str());
		//无内嵌歌词,搜索*.LRCY文件
		if (!track->m_bLrcInner){
			LrcMng *sLM=LrcMng::Get();
			if( track->GetLrcFileFromLib() ){
				sLM->OpenTrackPath(track);
				bLrcReady=TRUE;
			}
		}
	}

	LRESULT OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
	{
		ShowWindow(SW_HIDE);
		return 0;
	}
};
