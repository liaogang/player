#include "LrcMng.h"
#include "PlayList.h"
#include "dlgLrcSearch.h"
#include "forwardMsg.h"
#include "DialogLrcList.h"
using namespace std;

#define WIDTH(rc) ((rc).right-(rc.left))
#define HEIGHT(rc) ((rc).bottom-(rc.top))

class PlayListItem;
class lineinfo{
public:
	LONG yPos;
	int nStrLines;//几行字?
};

template <typename T>
class CMyLyric:public T
{
public:
	HMENU menu,trackMenu;
	HBRUSH brush,oldBrush;
	HPEN  newPen,oldPen; 


	CMyLyric():bLrcReady(FALSE),track(NULL),m_nFontHeight(25)
	{
		brush=::GetSysColorBrush(/*COLOR_3DFACE*/COLOR_BTNSHADOW);
		newPen=(HPEN)::CreatePen(PS_NULL,0,RGB(255,255,255));

		menu=::LoadMenu(NULL,MAKEINTRESOURCE (IDR_MENU_LRC) );
		trackMenu=::GetSubMenu(menu,0);


		if (!m_Font.IsNull())
			m_Font.DeleteObject();

		m_Font.CreateFont(
			m_nFontHeight,                        // nHeight
			0,                         // nWidth
			0,                         // nEscapement
			0,                         // nOrientation
			FW_BOLD,                 // nWeight
			FALSE,                     // bItalic
			FALSE,                     // bUnderline
			0,                         // cStrikeOut
			ANSI_CHARSET,              // nCharSet
			OUT_DEFAULT_PRECIS,        // nOutPrecision
			CLIP_DEFAULT_PRECIS,       // nClipPrecision
			DEFAULT_QUALITY,           // nQuality
			DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
			_T("Arial"));                 // lpszFacename
		

	}

	~CMyLyric()
	{
		::DestroyMenu(menu);
		DeleteObject(newPen);


		IDonotWantToReceiveMessage(WM_TRACKPOS);
		IDonotWantToReceiveMessage(WM_LYRIC_RELOAD);
		IDonotWantToReceiveMessage(WM_NEW_TRACK_STARTED);
	}

public:
	BEGIN_MSG_MAP_EX(CMyLyric)
		MESSAGE_HANDLER(WM_TRACKPOS,OnPos)
		MESSAGE_HANDLER(WM_LYRIC_RELOAD,OnLyricReload)
		MESSAGE_HANDLER(WM_NEW_TRACK_STARTED,OnNewTrackStarted)

		MSG_WM_CREATE(OnCreate)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MSG_WM_PAINT(OnPaint)
		MESSAGE_HANDLER(WM_SIZE,OnSize)
		MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBackground)
		MSG_WM_RBUTTONUP(OnRButtonUp)
		COMMAND_ID_HANDLER(IDOK, OnCloseCmd)
		COMMAND_ID_HANDLER(IDCANCEL, OnCloseCmd)
		COMMAND_ID_HANDLER(ID_OPEN_LRC_PATH, OnMenuFolderOpen)
		COMMAND_ID_HANDLER(ID_OPEN_LRCFILE, OnOpenLrcFile)
		COMMAND_ID_HANDLER(ID_MENU_LRC_PANE_PROPERTY,OnProperty)
		COMMAND_ID_HANDLER(ID_MENU_SEARCH_ONLINE,OnShowDlgLrcSearch)
		COMMAND_ID_HANDLER(ID_SHOW_LRC_LIST,OnShowDlgLrcList)
		END_MSG_MAP()

		int OnCreate(LPCREATESTRUCT lpCreateStruct)
		{
			Init();
			SetMsgHandled(FALSE);
			return 0;
		}

		LRESULT OnNewTrackStarted(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
		{
			TrackChanged();
			return 0;
		}

		RECT rc ,lrcRect;
		RECT rcPre,rcCur,rcNext;
		SIZE sz;
		std::tstring lrcText;
		int lrcLines,lrcTextHeight,lrcSpare,lrcHeight;
		BOOL bLrcReady;
		FileTrack* track;
		vector<LrcLine>::iterator curLine,nextLine;
		RECT tRc;

		std::wstring title;


		void OnPaint(CDCHandle dc)
		{
			PAINTSTRUCT ps;
			::BeginPaint(m_hWnd,&ps);
			if(bLrcReady)
				mydraw(ps.hdc);
			::EndPaint(m_hWnd,&ps);	
		}


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

		LRESULT OnProperty(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
		{
			// 			CColorDialog dlg;
			// 			dlg.DoModal(m_hWnd);
			return 0;
		}



		LRESULT OnShowDlgLrcList(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
		{
			DialogLrcList dlg;
			dlg.DoModal();

			return 0;
		}


		CDlgLrcSearch dlgLrcSearch;
		LRESULT OnShowDlgLrcSearch(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
		{
			if (!dlgLrcSearch.IsWindow())
				dlgLrcSearch.Create(T::m_hWnd);

			WCHAR  path[MAX_PATH]={};


			if(MyLib::shared()->isPlaying())
			{
				_songContainerItem playlistitem=GetPlayingItem();
				FileTrack* track=playlistitem.GetFileTrack();
				if (track)
				{
					if(MyLib::shared()->lrcDirs.size()>0)
						wcscpy(path,(WCHAR*)(*(MyLib::shared()->lrcDirs.begin())).c_str());
					else
					{
						int idx=track->url.find_last_of('\\');
						ATLASSERT(idx!=track->url.npos);

						if (idx!=track->url.npos)
							wcscpy(path,track->url.c_str()+idx+1);
					}

					//we use path\artist - title.lrc
					wcscat(wcscat(path,L"\\"),track->artist.c_str());
					wcscat(wcscat(path,L" - "),track->title.c_str());
					wcscat(path,L".lrc");

					dlgLrcSearch.ReInit((WCHAR*)track->artist.c_str(),(WCHAR*)track->title.c_str(),path);
				}//if(item)
			}


			if(!path)
				dlgLrcSearch.ReInit(NULL,NULL,NULL);

			dlgLrcSearch.ShowWindow(SW_SHOW);

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
			if(!track /*|| !track->m_bLrcFromLrcFile*/)
				return 0;

			if (!bLrcReady)
				return 0;

			trackPosInfo *posInfo=(trackPosInfo*)wParam;
			if (posInfo->left==0)
				ResetTitle();
			//-----------------------------------------
			LrcLine nextLrcLine=*nextLine;
			double totalsec=nextLrcLine.time.GetTotalMillisecond()/100;
			if (posInfo->used > totalsec)
			{
				++nextLine;

				auto k=curLineInfo;
				if(++k!=veclineinfo.end())
					//if (curLineInfo!=veclineinfo.end())
				{
					mydraw(GetDC());

					++curLineInfo;
				}
				else
				{
					mydraw(GetDC());
					bLrcReady=false;
				}
			}

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


		HDC m_memDCNormal,m_memDCHighlight;
		CFont m_Font;
		int m_nFontHeight;//字高

		int lineSpacing;//行间隔
		int lineHeight;//行高=字高*几行字 + 行间隔
		int lineWidth;//每一行的宽度
		int lineWidthSpacing;//行与窗口的左右间隔

		int totalHeight;



		vector<lineinfo> veclineinfo;
		vector<lineinfo>::iterator curLineInfo;
		vector<LrcLine> lrcs;
		
		void CreateBackDC()
		{
			HDC dc=GetDC();

			//settings
			lineWidthSpacing=3;
			
			lineSpacing=3;
			lineWidth=200;
			lineHeight=+lineSpacing+m_nFontHeight;


			//
			RECT rcDest;
			rcDest.left=0;
			rcDest.top=0;
			rcDest.right=lineWidth;
			rcDest.bottom=lrcs.size()*lineHeight;




			//Draw In Normal and HighLight mode
			COLORREF textColor=RGB(0,118,163);
			COLORREF textColorHighlight=RGB(128,0,0);
				

			m_memDCNormal=::CreateCompatibleDC(dc);
			m_memDCHighlight=::CreateCompatibleDC(dc);
			HBITMAP tmp=::CreateCompatibleBitmap(dc,WIDTH(rcDest),HEIGHT(rcDest));
			::SelectObject(m_memDCNormal,tmp);
			tmp=::CreateCompatibleBitmap(dc,WIDTH(rcDest),HEIGHT(rcDest));
			::SelectObject(m_memDCHighlight,tmp);
			

			//draw backgnd rectangle
			RECT rc=rcDest;
			::SelectObject(m_memDCNormal,brush);			
			::SelectObject(m_memDCNormal,newPen);
			::Rectangle(m_memDCNormal,rc.left-1,rc.top,rc.right+1,rc.bottom+1);
			
			::SelectObject(m_memDCHighlight,brush);			
			::SelectObject(m_memDCHighlight,newPen);
			::Rectangle(m_memDCHighlight,rc.left-1,rc.top,rc.right+1,rc.bottom+1);

			
			//::SelectObject(m_memDCNormal,oldBrush);
			//::SelectObject(m_memDCNormal,oldPen);


			//now draw text
			//int oldBgkMode=::
			SetBkMode(m_memDCNormal,TRANSPARENT);
			SetBkMode(m_memDCHighlight,TRANSPARENT);
			
			//COLORREF oldTextColor= GetTextColor(m_memDCNormal);
			SetTextColor(m_memDCNormal,textColor);
			SetTextColor(m_memDCHighlight,textColorHighlight);
			//::SelectObject(m_memDCNormal,m_Font.m_hFont);

			veclineinfo.clear();
			lineinfo info;
			info.yPos=0;
			info.nStrLines=0;
			for (auto i=lrcs.begin();i!=lrcs.end();++i)
			{
				info.yPos=info.yPos+info.nStrLines*m_nFontHeight;
				info.nStrLines=GetStrSizeX(dc,(TCHAR*)i->text.c_str())/(int)lineWidth+1;

				veclineinfo.push_back(info);

				RECT rcString;
				rcString.left=lineWidthSpacing;
				rcString.right=rcString.left+lineWidth;
				rcString.top=info.yPos;
				rcString.bottom=rcString.top+info.nStrLines*m_nFontHeight;
				::DrawText(m_memDCNormal,i->text.c_str(),i->text.length(),&rcString,DT_CENTER);//DT_CALCRECT|
				::DrawText(m_memDCHighlight,i->text.c_str(),i->text.length(),&rcString,DT_CENTER);//DT_CALCRECT|
			}

			

			totalHeight=info.yPos+info.nStrLines*m_nFontHeight;
			curLineInfo=veclineinfo.begin();
		}


		void mydraw(HDC dc)
		{
			RECT rcDest;
			GetClientRect(&rcDest);
			int y;
			if(!bLrcReady)return;
			y=curLineInfo->yPos-(rc.bottom-rc.top)/2;
			 
			RECT rcLine=rcDest;
			rcLine.top=HEIGHT(rcDest) /2;
			rcLine.bottom=rcLine.top+curLineInfo->nStrLines*m_nFontHeight;

			::BitBlt(dc,rcDest.left,rcDest.top,rcDest.right-rcDest.left,rcDest.bottom-rcDest.top,
				m_memDCNormal,0,y, SRCCOPY);
			
			::BitBlt(dc,rcLine.left,rcLine.top,rcLine.right-rcLine.left,rcLine.bottom-rcLine.top,
				m_memDCHighlight,0,y+rcLine.top, SRCCOPY);
		}



		void TrackChanged()
		{

			int ii=0;
			OnSize(0,0,0,ii);
			InvalidateRect(&lrcRect);

			bLrcReady=FALSE;
			//PlayList *playlist=MyLib::shared()->ActivePlaylist();
			//if(!playlist) return;
			if(!MyLib::shared()->isPlaying())
				return;

			track=GetPlayingItem().GetFileTrack();


			LrcMng *sLM=LrcMng::Get();
			if( track->GetLrcFileFromLib(TRUE) )
			{
				lrcs=LrcMng::Get()->lib;
				veclineinfo.clear();


				CreateBackDC();

				

				//del lyricfromlrcfile
				//use global
				nextLine=lrcs.begin();
				bLrcReady=TRUE;

				int i=0;
				OnSize(0,0,0,i);
				InvalidateRect(&lrcRect);
			}


			if(track->lycPath.empty())
				ResetTitle();
			else
			{
				title=track->lycPath;
				ChangedTitle();
			}
			//无内嵌歌词,搜索*.LRCY文件
			//track->m_bLrcInner


		}

		void Init()
		{
			SetFont(m_Font.m_hFont);

			//GetTextExtentPoint32(GetDC(),_T("A"),1,&sz); 
			sz.cy=19;
			lrcTextHeight=sz.cy;
			lrcSpare=3;
			lrcHeight=lrcTextHeight+lrcSpare;



			track=NULL;
			bLrcReady=FALSE;


			IWantToReceiveMessage(WM_TRACKPOS);
			IWantToReceiveMessage(WM_LYRIC_RELOAD);
			IWantToReceiveMessage(WM_NEW_TRACK_STARTED);

			TrackChanged();
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

