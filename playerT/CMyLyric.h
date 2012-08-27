

#include "LrcMng.h"
#include "PlayList.h"

#define  ID_MENU_FOLDER_OPEN (0XF000-202)
class PlayListItem;
template <typename T>
class CMyLyric:public T
{
public:
	HMENU menu;
	CMyLyric()
	{
		menu=::CreatePopupMenu();
		::InsertMenu(menu,0,MF_BYCOMMAND|MF_BYPOSITION,ID_MENU_FOLDER_OPEN,_T("打开所在文件夹"));
	}

	~CMyLyric()
	{
		
	}

public:
	BEGIN_MSG_MAP_EX(CDialogLyric)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MSG_WM_PAINT(OnPaint)
		MESSAGE_HANDLER(WM_TRACKPOS,OnPos)
		MESSAGE_HANDLER(WM_SIZE,OnSize)
		MESSAGE_HANDLER(WM_LYRIC_RELOAD,OnLyricReload)
		MSG_WM_RBUTTONUP(OnRButtonUp)
		COMMAND_ID_HANDLER(IDOK, OnCloseCmd)
		COMMAND_ID_HANDLER(IDCANCEL, OnCloseCmd)
		COMMAND_ID_HANDLER(ID_MENU_FOLDER_OPEN, OnMenuFolderOpen)
		END_MSG_MAP()

		RECT rc ,lrcRect;
		SIZE sz;
		std::tstring lrcText;
		int lrcLines,lrcTextHeight,lrcSpare,lrcHeight;
		BOOL bLrcReady;
		PlayListItem* track;
		double used,lefted;
		vector<LrcLine>::iterator preLine,lastLine;
		RECT tRc;
		BOOL bNoChange;
		
		std::wstring title;
		LRESULT OnSize(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			::GetClientRect(T::m_hWnd,&rc);
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
			::TrackPopupMenu(menu,TPM_LEFTALIGN,point.x,point.y,0,m_hWnd,0);
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

			if (lefted=0)
				ResetTitle();
			
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
						bNoChange=FALSE;
					}
					else
						bNoChange=TRUE;

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

			if(!bLrcReady)
			{
			}
			else if(track->m_bLrcInner)
			{
				::DrawText(ps.hdc,track->lyricInner.c_str(),track->lyricInner.length(),&rc,DT_CENTER);
			}
			else if(track->m_bLrcFromLrcFile)
			{
				if(!bNoChange)
				{
					LrcLine l=*preLine;
					if (!l.text.empty() )
						::DrawText(ps.hdc,(*preLine).text.c_str(),(*preLine).text.length(),&rc,DT_CENTER);	
				}
			}

			::EndPaint(m_hWnd,&ps);	
		}


		void TrackChanged()
		{
			bLrcReady=FALSE;
			PlayList *playlist=MyLib::shared()->ActivePlaylist();
			if(!playlist) return;
			track=playlist->curTrack();
			if(!track) return;

			if(track->lycPath.empty())
				ResetTitle();
			else
			{
				title=track->lycPath;
				ChangedTitle();
			}


			//无内嵌歌词,搜索*.LRCY文件
			if (!track->m_bLrcInner){
				LrcMng *sLM=LrcMng::Get();
				if( track->GetLrcFileFromLib() )
				{
					if(sLM->OpenTrackPath(track))
					{
						preLine=track->lyricFromLrcFile.begin();
						bLrcReady=TRUE;
					}
				}
			}
		}

		void Init()
		{
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
			bNoChange=FALSE;
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


		//track change , to get current song lyric


		
		LRESULT OnMenuFolderOpen(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
		{
			if(track->m_bLrcFromLrcFile)
			{
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

