

#include "LrcMng.h"
#include "PlayList.h"

#define  ID_MENU_FOLDER_OPEN (0XF000-202)
class PlayListItem;
template <typename T>
class CMyLyric:public T
{
public:
	HMENU menu,trackMenu;
	HBRUSH brush,oldBrush;
	HPEN  newPen,oldPen; 
	CMyLyric()
	{
		brush=::GetSysColorBrush(/*COLOR_3DFACE*/COLOR_BTNSHADOW);
		newPen=(HPEN)::CreatePen(PS_NULL,0,RGB(255,255,255));

		

		menu=::LoadMenu(NULL,MAKEINTRESOURCE (IDR_MENU_LRC) );
		trackMenu=::GetSubMenu(menu,0);
	}

	~CMyLyric()
	{
		::DestroyMenu(menu);
		DeleteObject(newPen);
	}

public:
	BEGIN_MSG_MAP_EX(CMyLyric)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MSG_WM_PAINT(OnPaint)
		MESSAGE_HANDLER(WM_TRACKPOS,OnPos)
		MESSAGE_HANDLER(WM_SIZE,OnSize)
		MESSAGE_HANDLER(WM_LYRIC_RELOAD,OnLyricReload)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBackground)
		MSG_WM_RBUTTONUP(OnRButtonUp)
		COMMAND_ID_HANDLER(IDOK, OnCloseCmd)
		COMMAND_ID_HANDLER(IDCANCEL, OnCloseCmd)
		COMMAND_ID_HANDLER(ID_OPEN_LRC_PATH, OnMenuFolderOpen)
		COMMAND_ID_HANDLER(ID_OPEN_LRCFILE, OnOpenLrcFile)
	END_MSG_MAP()

		RECT rc ,lrcRect;
		RECT rcPre,rcCur,rcNext;
		SIZE sz;
		std::tstring lrcText;
		int lrcLines,lrcTextHeight,lrcSpare,lrcHeight;
		BOOL bLrcReady;
		PlayListItem* track;
		vector<LrcLine>::iterator preLine,curLine,nextLine;
		RECT tRc;
		
		std::wstring title;
		LRESULT OnSize(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			const int linesSpace=5;
			GetClientRect(&rc);

			int stringHeight=sz.cy;
			rcPre=rc;
			rcPre.bottom=rcPre.top+stringHeight;

			rcCur=rcPre;
			rcCur.top+=stringHeight+linesSpace;
			rcCur.bottom=rcCur.top+stringHeight;

			rcNext=rcCur;
			rcNext.top+=stringHeight+linesSpace;
			rcNext.bottom=rcNext.top+stringHeight;

			lrcRect=rcPre;
			lrcRect.bottom=rcNext.bottom;


			bHandled=FALSE;
			return 0;
		}


		LRESULT OnLyricReload(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			TrackChanged();
			return 0;
		}

		void OnRButtonUp(UINT nFlags, CPoint point)
		{
			::ClientToScreen(m_hWnd,&point);
			::TrackPopupMenu(trackMenu,TPM_LEFTALIGN,point.x,point.y,0,m_hWnd,0);
		}

		LRESULT OnPos(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			bHandled=FALSE;
			if(!::IsWindowVisible(m_hWnd))
				return 0;

			if(!track || !track->m_bLrcFromLrcFile)
				return 0;
			if (!bLrcReady)
				return 0;

			trackPosInfo *posInfo=(trackPosInfo*)wParam;

			if (posInfo->left==0)
				ResetTitle();
			
			
			//used= used%sz            //以行高为基数，取整。
			//double lrcContent=used+lefted;
			//lrcRect=rc;
			//lrcRect.top=  (rc.bottom-rc.top)/2 - (used/lrcContent) * lrcLines*lrcHeight;
			
			//-----------------------------------------
			LrcLine nextLrcLine=*nextLine;
			double totalsec=nextLrcLine.time.GetTotalMillisecond()/100;
			if (posInfo->used > totalsec)
			{
				//change line
				preLine=curLine;
				curLine=nextLine;
				nextLine++;

				InvalidateRect(&lrcRect);

				if (nextLine==track->lyricFromLrcFile.end())
					bLrcReady=FALSE;
			}
			/*
			//-----------------------------------------
			//-----------------------------------------
			vector<LrcLine>::iterator i;
			for (i=track->lyricFromLrcFile.begin(),lastLine=i,preLine=i,curLine=i,nextLine=i,lrcLines=0;
				i!=track->lyricFromLrcFile.end();
				preLine=i,++i,++lrcLines)
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
						lastLine=preLine;
					}

					break;
				}
				//lrcText+=(*i).text+_T("\n");
			}//for
			//-----------------------------------------
			*/

			return 0;
		}


		LRESULT OnEraseBackground(UINT /*uMsg*/, WPARAM  wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
		{
			HDC dc=(HDC)wParam;
			RECT rc;

			GetClientRect(&rc);

			oldBrush=(HBRUSH)::SelectObject(dc,brush);			
			oldPen=(HPEN)::SelectObject(dc,newPen);

			::Rectangle(dc,rc.left-1,rc.top,rc.right+1,rc.bottom+1);

			::SelectObject(dc,oldBrush);
			::SelectObject(dc,oldPen);

			return 1;
		}

		void OnPaint(CDCHandle dc)
		{
			PAINTSTRUCT ps;
			::BeginPaint(m_hWnd,&ps);
			int oldBgkMode=::SetBkMode(ps.hdc,TRANSPARENT);

			if(!bLrcReady);
			else if(track->m_bLrcFromLrcFile)
			{
				//LrcLine l=*preLine;
				//if (!l.text.empty())
				{
					::DrawText(ps.hdc,preLine->text.c_str(),preLine->text.length(),&rcPre,DT_CENTER|DT_NOCLIP);
					
					
					LOGFONT lf;
					memset(&lf, 0, sizeof(LOGFONT));       // zero out structure
					lf.lfHeight = 19;                      // request a 12-pixel-height font
					_tcscpy(lf.lfFaceName, _T("Arial"));        // request a face name "Arial"
					

					HFONT newFont=::CreateFontIndirect(&lf);

					COLORREF oldTextColor= GetTextColor(ps.hdc);
					SetTextColor(ps.hdc,RGB(255,0,0));
					HFONT oldFont=(HFONT) SelectObject(ps.hdc,newFont);
					::DrawText(ps.hdc,curLine->text.c_str(),curLine->text.length(),&rcCur,DT_CENTER|DT_NOCLIP);	
					SetTextColor(ps.hdc,oldTextColor);
					::SelectObject(ps.hdc,oldFont);

					::DrawText(ps.hdc,nextLine->text.c_str(),nextLine->text.length(),&rcNext,DT_CENTER|DT_NOCLIP);
				}
							
			}
			else if(track->m_bLrcInner)
			{
				::DrawText(ps.hdc,track->lyricInner.c_str(),track->lyricInner.length(),&rc,DT_CENTER);
			}


			 ::SetBkMode(ps.hdc,oldBgkMode);

			::EndPaint(m_hWnd,&ps);	
		}


		void TrackChanged()
		{
			bLrcReady=FALSE;
			PlayList *playlist=MyLib::shared()->ActivePlaylist();
			if(!playlist) return;
			track=playlist->curTrack();
			if(!track) return;



			LrcMng *sLM=LrcMng::Get();
			if( track->GetLrcFileFromLib() )
			{
				LrcLines *emptyVec=new LrcLines;
				emptyVec->push_back(lineEmpty);
				preLine=emptyVec->begin();
				curLine=track->lyricFromLrcFile.begin();
				nextLine=curLine;
				nextLine++;
				bLrcReady=TRUE;
			}
			

			if(track->lycPath.empty())
				ResetTitle();
			else{
				title=track->lycPath;
				ChangedTitle();
			}
			//无内嵌歌词,搜索*.LRCY文件
			//track->m_bLrcInner
		}

		void Init()
		{
			GetTextExtentPoint32(GetDC(),_T("A"),1,&sz); 
			lrcTextHeight=sz.cy;
			lrcSpare=3;
			lrcHeight=lrcTextHeight+lrcSpare;

		

			track=NULL;
			bLrcReady=FALSE;
		}

		LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
		{
			CenterWindow(GetParent());
			Init();
			return TRUE;
		}

		void ChangedTitle()
		{
			SetWindowText((LPTSTR)title.c_str());
		}

		void ResetTitle()
		{
			SetWindowText(_T("歌词"));
		}

		
		LRESULT OnOpenLrcFile(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
		{
			if(track->m_bLrcFromLrcFile)
			{
				ShellExecute(NULL,L"open",
					L"explorer",
					track->lycPath.c_str(),
					L"",
					SW_SHOW);
			}

			return 0;
		}

		LRESULT OnMenuFolderOpen(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
		{
			if(track->m_bLrcFromLrcFile){
				std::tstring tmp=L"/select,";
				tmp+=track->lycPath.c_str();

				ShellExecute(NULL,L"open",
				L"explorer",
				tmp.c_str(),
				L"",
				SW_SHOW);
			}

			return 0;
		}

		LRESULT OnCloseCmd(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
		{
			ShowWindow(SW_HIDE);
			return 0;
		}
};

