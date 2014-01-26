#include "CMyLyric.h"
#include "forwardMsg.h"
#include "PlayList.h"
#include "MyLib.h"

CMyLyricWnd::CMyLyricWnd():bLrcReady(FALSE),track(NULL),m_nFontHeight(20),m_nIDEvent(0),
	m_memDCNormal(NULL),m_memDCHighlight(NULL),m_bInitSize(FALSE),m_memDCReady(FALSE),bNewTrack(FALSE)
{
	//brush=::GetSysColorBrush(COLOR_WINDOW);
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
		FW_NORMAL,                 // nWeight
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

CMyLyricWnd::~CMyLyricWnd()
{
	::DestroyMenu(menu);
	DeleteObject(newPen);
}



void CMyLyricWnd::OnTimer(UINT_PTR /*nIDEvent*/)
{
	if(m_bPaused || !bLrcReady)
		return;

	m_uCurrTime+=m_uElapse;

	UpdateTime();
	if(UpdateCurrLine())
		mydraw();
}

void CMyLyricWnd::UpdateTime()
{
	double usedtotalSec=getTrackPosInfo()->used;

	m_uCurrTime=usedtotalSec*1000;
}


BOOL CMyLyricWnd::UpdateCurrLine()
{
	int old=m_iCurLine;
	int k=0;
	for (auto i=lrcs.begin();i!=lrcs.end();++i,++k)
	{
		if (m_uCurrTime <= i->time.GetTotalMillisec())
			break;
		else
			m_iCurLine=k;
	}

	return m_iCurLine!=old;
}

void CMyLyricWnd::OnDestroy()
{
	IDonotWantToReceiveMessage(WM_TRACK_POS_CHANGED);
	IDonotWantToReceiveMessage(WM_LYRIC_RELOAD);
	IDonotWantToReceiveMessage(WM_NEW_TRACK_STARTED);


	IDonotWantToReceiveMessage(WM_PAUSED);
	IDonotWantToReceiveMessage(WM_PAUSE_START);
	IDonotWantToReceiveMessage(WM_TRACKSTOPPED);

	if(m_nIDEvent!=0)
		KillTimer((UINT_PTR)&m_nIDEvent);	

	SetMsgHandled(FALSE);
}







LRESULT CMyLyricWnd::OnShowDlgLrcList(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	DialogLrcList dlg;
	dlg.DoModal();

	return 0;
}


LRESULT CMyLyricWnd::OnShowDlgLrcSearch(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	if (!dlgLrcSearch.IsWindow())
		dlgLrcSearch.Create(m_hWnd);

	WCHAR  path[MAX_PATH]={};


	if(MyLib::shared()->isPlaying())
	{
		_songContainerItem playlistitem=MyLib::shared()->GetPlayingPL()->GetPlayingItem();
		FileTrack* track=playlistitem->GetFileTrack();
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


void CMyLyricWnd::CreateBackDC()
{
	int lineSpacing;//行间隔
	int lineHeight;//行高=字高*几行字 + 行间隔
	int lineWidth;//每一行的宽度
	int lineWidthSpacing;//行与窗口的左右间隔
	int totalHeight;
	m_iESize=500;

	HDC dc=GetDC();
	GetClientRect(&m_rcClient);

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

	static RECT rcDesktop;
	if(bNewTrack){

		HWND destop=GetDesktopWindow();
		::GetClientRect(destop,&rcDesktop);
		rcDesktop.bottom=HEIGHT(rcDesktop)*5;
		rcDesktop.top-=500;

		if(m_memDCNormal!=NULL)::DeleteDC(m_memDCNormal);
		if(m_memDCHighlight!=NULL)::DeleteDC(m_memDCHighlight);


		m_memDCNormal=::CreateCompatibleDC(dc);
		HBITMAP tmp=::CreateCompatibleBitmap(dc,WIDTH(rcDesktop),HEIGHT(rcDesktop));
		::SelectObject(m_memDCNormal,tmp);
		::DeleteObject(tmp);
		::SelectObject(m_memDCNormal,m_Font);

		m_memDCHighlight=::CreateCompatibleDC(dc);
		tmp=::CreateCompatibleBitmap(dc,WIDTH(rcDesktop),HEIGHT(rcDesktop));
		::SelectObject(m_memDCHighlight,tmp);
		::DeleteObject(tmp);
		::SelectObject(m_memDCHighlight,m_Font);

		//draw backgnd rectangle
		HBRUSH hOldBrush1=(HBRUSH) ::SelectObject(m_memDCNormal,brush);			
		HPEN hOldPen1=(HPEN) ::SelectObject(m_memDCNormal,newPen);


		HBRUSH hOldBrush2=(HBRUSH) ::SelectObject(m_memDCHighlight,brush);			
		HPEN hOldPen2=(HPEN)::SelectObject(m_memDCHighlight,newPen);
	}


	::Rectangle(m_memDCNormal,rcDesktop.left-1,rcDesktop.top,rcDesktop.right+1,rcDesktop.bottom+1);
	::Rectangle(m_memDCHighlight,rcDesktop.left-1,rcDesktop.top,rcDesktop.right+1,rcDesktop.bottom+1);

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

	info.yPos=m_iESize;

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
		totalHeight=info.yPos+info.nStrLines*m_nFontHeight;

	ReleaseDC(dc);

	if(bNewTrack)
		bNewTrack=FALSE;

	m_memDCReady=TRUE;
}



void CMyLyricWnd::PrepareShowLyric()
{
	if(bLrcReady==TRUE)
		return;

	if(!MyLib::shared()->isPlaying())
		return;

	track=MyLib::shared()->GetPlayingPL()->GetPlayingItem()->GetFileTrack();

	LrcMng *sLM=LrcMng::Get();
	if( track->GetLrcFileFromLib(TRUE) )
	{
		lrcs=LrcMng::Get()->lib;
		veclineinfo.clear();
		m_iCurLine=0;

		bNewTrack=TRUE;
		if(m_rcClient.right!=0 &&m_rcClient.bottom!=0)
			CreateBackDC();
		else
			m_memDCReady=FALSE;

		m_bPaused=false;


		if(m_nIDEvent!=0)
			KillTimer((UINT_PTR)&m_nIDEvent);
		SetTimer((UINT_PTR)&m_nIDEvent,m_uElapse,NULL);


		//del lyricfromlrcfile
		UpdateTime();
		UpdateCurrLine();
		bLrcReady=TRUE;
	}


	Invalidate();

	if(track->lycPath.empty())
		ResetTitle();
	else
	{
		title=track->lycPath;
		ChangedTitle();
	}
}



void CMyLyricWnd::Init()
{
	SetFont(m_Font.m_hFont);

	//GetTextExtentPoint32(GetDC(),_T("A"),1,&sz); 
	sz.cy=19;

	track=NULL;
	bLrcReady=FALSE;


	IWantToReceiveMessage(WM_TRACK_POS_CHANGED);
	IWantToReceiveMessage(WM_LYRIC_RELOAD);

	IWantToReceiveMessage(WM_NEW_TRACK_STARTED);
	IWantToReceiveMessage(WM_PAUSED);
	IWantToReceiveMessage(WM_PAUSE_START);
	IWantToReceiveMessage(WM_TRACKSTOPPED);

	//TrackChanged();
}


LRESULT CMyLyricWnd::OnOpenLrcFile(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
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


LRESULT CMyLyricWnd::OnMenuFolderOpen(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
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

