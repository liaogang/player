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


	CMyLyric():bLrcReady(FALSE),track(NULL),m_nFontHeight(27)
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

		SIZE sz;
		BOOL bLrcReady;
		FileTrack* track;
		vector<LrcLine>::iterator curLine,nextLine;

		std::wstring title;

		RECT m_rcClient;

		void OnPaint(CDCHandle dc)
		{
			PAINTSTRUCT ps;
			::BeginPaint(m_hWnd,&ps);
			if(bLrcReady)
				mydraw(ps.hdc);
			::EndPaint(m_hWnd,&ps);	
		}

		BOOL m_bSized;

		//If an application processes this message, it should return zero. 
		LRESULT OnSize(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			GetClientRect(&m_rcClient);
			
			m_bSized=TRUE;

			bHandled=FALSE;
			return 1;
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
				{
					mydraw();

					++curLineInfo;
				}
				else
				{
					mydraw();
					bLrcReady=false;
				}
			}

			return 0;
		}


		LRESULT OnEraseBackground(UINT /*uMsg*/, WPARAM  wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
		{
			HDC dc=(HDC)wParam;

			oldBrush=(HBRUSH)::SelectObject(dc,brush);			
			oldPen=(HPEN)::SelectObject(dc,newPen);

			RECT rc;
			GetClientRect(&rc);
			::Rectangle(dc,rc.left-1, rc.top-1, rc.right+1 , rc.bottom+1);

			::SelectObject(dc,oldBrush);
			::SelectObject(dc,oldPen);

			return 1;
		}


		HDC m_memDCNormal,m_memDCHighlight;
		CFont m_Font;




		vector<lineinfo> veclineinfo;
		vector<lineinfo>::iterator curLineInfo;
		vector<LrcLine> lrcs;
		
			int m_nFontHeight;//字高



		//We will Create two memory drawing context
		//One is normal,another is highlighted
		//Then,we will cut on line space from the true display draw context
		//parse with the highlighted memory one
		//parse other with the normal memory one
		void CreateBackDC(BOOL bNewTrack=FALSE)
		{
			int lineSpacing;//行间隔
			int lineHeight;//行高=字高*几行字 + 行间隔
			int lineWidth;//每一行的宽度
			int lineWidthSpacing;//行与窗口的左右间隔
			int totalHeight;


			HDC dc=GetDC();

			//settings
			lineWidthSpacing=3;
			lineSpacing=3;
			lineWidth=m_rcClient.right-m_rcClient.left-3;
			lineHeight=+lineSpacing+m_nFontHeight;


			//
			RECT rcDest;
			rcDest.left=3;
			rcDest.top=3;
			rcDest.right=lineWidth+rcDest.left;
			rcDest.bottom=lrcs.size()*lineHeight+rcDest.top;


			//Draw In Normal and HighLight mode
			static COLORREF textColor=RGB(0,118,163);
			static COLORREF textColorHighlight=RGB(128,0,0);
			

			if(bNewTrack){
			if(m_memDCNormal!=NULL)::DeleteDC(m_memDCNormal);
			if(m_memDCHighlight!=NULL)::DeleteDC(m_memDCHighlight);

			
			m_memDCNormal=::CreateCompatibleDC(dc);
			HBITMAP tmp=::CreateCompatibleBitmap(dc,WIDTH(rcDest),HEIGHT(rcDest));
			::SelectObject(m_memDCNormal,tmp);
			::DeleteObject(tmp);

			m_memDCHighlight=::CreateCompatibleDC(dc);
			tmp=::CreateCompatibleBitmap(dc,WIDTH(rcDest),HEIGHT(rcDest));
			::SelectObject(m_memDCHighlight,tmp);
			::DeleteObject(tmp);
			

			//draw backgnd rectangle
			HBRUSH hOldBrush1=(HBRUSH) ::SelectObject(m_memDCNormal,brush);			
			HPEN hOldPen1=(HPEN) ::SelectObject(m_memDCNormal,newPen);
			
			
			HBRUSH hOldBrush2=(HBRUSH) ::SelectObject(m_memDCHighlight,brush);			
			HPEN hOldPen2=(HPEN)::SelectObject(m_memDCHighlight,newPen);
			}

			
			::Rectangle(m_memDCNormal,m_rcClient.left-1,m_rcClient.top,m_rcClient.right+1,m_rcClient.bottom+1);
			::Rectangle(m_memDCHighlight,m_rcClient.left-1,m_rcClient.top,m_rcClient.right+1,m_rcClient.bottom+1);


			//now draw text
			if(bNewTrack)
			{
				SetBkMode(m_memDCNormal,TRANSPARENT);
				SetBkMode(m_memDCHighlight,TRANSPARENT);
			
				SetTextColor(m_memDCNormal,textColor);
				SetTextColor(m_memDCHighlight,textColorHighlight);
			}


			lineinfo info;
			memset(&info,0,sizeof(info));
			if(bNewTrack)
				veclineinfo.clear();
			
			for (auto i=lrcs.begin();i!=lrcs.end();++i)
			{
				info.yPos=info.yPos+info.nStrLines*m_nFontHeight;
				info.nStrLines=GetStrSizeX(dc,(TCHAR*)i->text.c_str())/(int)lineWidth+1;

				if(bNewTrack)
					veclineinfo.push_back(info);
				
				RECT rcString;
				rcString.left=lineWidthSpacing;
				rcString.right=rcString.left+lineWidth;
				rcString.top=info.yPos;
				rcString.bottom=rcString.top+info.nStrLines*m_nFontHeight;
				::DrawText(m_memDCNormal,i->text.c_str(),i->text.length(),&rcString,DT_CENTER);//DT_CALCRECT|
				::DrawText(m_memDCHighlight,i->text.c_str(),i->text.length(),&rcString,DT_CENTER);//DT_CALCRECT|
			}

			
			if(bNewTrack)
			{
				curLineInfo=veclineinfo.begin();
				totalHeight=info.yPos+info.nStrLines*m_nFontHeight;
			


				//::SelectObject(m_memDCNormal,hOldBrush1);
				//::SelectObject(m_memDCNormal,hOldPen1);
				//::SelectObject(m_memDCHighlight,hOldBrush2);
				//::SelectObject(m_memDCHighlight,hOldPen2);
			}

			ReleaseDC(dc);
		}

		void mydraw(HDC dc_=NULL)
		{
			HDC dc=dc_==NULL?GetDC():dc_;
			
			if(m_bSized)
			{
				m_bSized=FALSE;
				CreateBackDC();
			}
			
			RECT rcDest;
			GetClientRect(&rcDest);
			int y;
			if(!bLrcReady)return;
			y=curLineInfo->yPos-(m_rcClient.bottom-m_rcClient.top)/2;
			 
			RECT rcLine=rcDest;
			rcLine.top=HEIGHT(rcDest) /2;
			rcLine.bottom=rcLine.top+curLineInfo->nStrLines*m_nFontHeight;

			::BitBlt(dc,rcDest.left,rcDest.top,rcDest.right-rcDest.left,rcDest.bottom-rcDest.top,
				m_memDCNormal,0,y, SRCCOPY);
			
			::BitBlt(dc,rcLine.left,rcLine.top,rcLine.right-rcLine.left,rcLine.bottom-rcLine.top,
				m_memDCHighlight,0,y+rcLine.top, SRCCOPY);

			if(dc_==NULL)
				ReleaseDC(dc);
		}



		void TrackChanged()
		{
			bLrcReady=FALSE;
			
			if(!MyLib::shared()->isPlaying())
				return;

			track=GetPlayingItem().GetFileTrack();

			LrcMng *sLM=LrcMng::Get();
			if( track->GetLrcFileFromLib(TRUE) )
			{
				lrcs=LrcMng::Get()->lib;
				veclineinfo.clear();

				CreateBackDC(TRUE);

				//del lyricfromlrcfile
				//use global
				nextLine=lrcs.begin();
				bLrcReady=TRUE;

				Invalidate();
			}


			if(track->lycPath.empty())
				ResetTitle();
			else
			{
				title=track->lycPath;
				ChangedTitle();
			}

		}

		void Init()
		{
			SetFont(m_Font.m_hFont);

			//GetTextExtentPoint32(GetDC(),_T("A"),1,&sz); 
			sz.cy=19;

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

