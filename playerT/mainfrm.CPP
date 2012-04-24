#include "StdAfx.h"
#include "MainFrm.h"
#include "PlayList.h"
#include "DialogFFT.h"
#include "DialogLyric.h"
#include "BasicPlayer.h"
#include "CAlbumCoverView.h"

#include <mpg123.h>

#include <string>
using namespace std;


LRESULT CMainFrame::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	// create command bar window
	HWND hWndCmdBar = m_CmdBar.Create(m_hWnd, rcDefault, NULL, ATL_SIMPLE_CMDBAR_PANE_STYLE);
	// attach menu
	CMenu *pMenu=new CMenu;
	pMenu->LoadMenu(IDR_MAINFRAME);
	m_CmdBar.AttachMenu(pMenu->m_hMenu);
	// load command bar images
	m_CmdBar.LoadImages(IDR_MAINFRAME);
	// remove old menu
	SetMenu(NULL);


	//-----------------------------------------
	//toobar 
	HWND hWndToolBar = CreateSimpleToolBarCtrl(m_hWnd,IDR_MAINFRAME, FALSE, ATL_SIMPLE_TOOLBAR_PANE_STYLE);
	CreateSimpleReBar(ATL_SIMPLE_REBAR_NOBORDER_STYLE);
	AddSimpleReBarBand(hWndCmdBar);
	AddSimpleReBarBand(hWndToolBar, NULL,TRUE,110,TRUE);



	SetWindowText(_T(APPNAME));
	//------------------------------------------------
	//progress track bar
	UINT style=WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
	style|= TBS_TOOLTIPS  |TBS_NOTICKS    ;
	//style=style|~TBS_NOTICKS;
	HWND hWndProgressBar=m_trackBar.Create(this->m_hWnd,NULL,NULL,style,0);
	m_trackBar.SetPageSize(1);
	m_trackBar.SetLineSize(1);
	m_trackBar.SetThumbLength(30);
	AddSimpleReBarBand(hWndProgressBar,NULL,FALSE,150,TRUE);


	//-------------------------------------------------
	//combo box
	HWND comboBar=m_wndComboBox.Create(m_hWnd,NULL,NULL,WS_CHILD|WS_VISIBLE|WS_VSCROLL|CBS_DROPDOWNLIST);
	for (int i=0;i<PLAYORDERCOUNT;++i)
		m_wndComboBox.AddString(gPlayOrderStr[i]);
	m_wndComboBox.SetCurSel(0);
	
// 	CFont font;
// 	LOGFONT lf;
// 	memset(&lf, 0, sizeof(LOGFONT));       // zero out structure
// 	//lf.lfHeight = 20;                      // request a 12-pixel-height font
// 	lf.lfItalic=TRUE;
// 	//lf.lfCharSet=
// 	lf.lfUnderline=TRUE;
// 	_tcscpy((TCHAR*)lf.lfFaceName, _T("Arial"));        // request a face name "Arial"
// 	font.CreateFontIndirect(&lf);
// 	//m_wndComboBox.GetFont();
// 	m_wndComboBox.SetFont(font,TRUE);
	AddSimpleReBarBand(comboBar,NULL,FALSE,100,TRUE);


	//-------------------------------------------------
	//status bar  
	//add it all
	CreateSimpleStatusBar();

	UIAddToolBar(hWndToolBar);
	UISetCheck(ID_VIEW_TOOLBAR, 1);
	UISetCheck(ID_VIEW_STATUS_BAR, 1);

	// register object for message filtering and idle updates
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->AddMessageFilter(this);
	pLoop->AddIdleHandler(this);


	g_pSharedPlayer=CBasicPlayer::shared();	
	g_pSharedPlayer->m_pMainFrame=this;
	//-----------------------------------------
	leftPane=new CHorSplitterWindow;
	//CSplitterWindow *rightPane=new CSplitterWindow;
	const DWORD dwSplitStyle = WS_CHILD | WS_VISIBLE | 
		WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
		dwSplitExStyle = WS_EX_CLIENTEDGE;
	split.Create (m_hWnd, rcDefault, NULL, 
		dwSplitStyle, dwSplitExStyle );
	m_hWndClient =split;

	//right pane
	m_view.Create(split.m_hWnd, rcDefault, NULL, LVS_SINGLESEL | WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | LVS_REPORT | LVS_SHOWSELALWAYS, WS_EX_CLIENTEDGE);
	m_view.m_pMainFrame=this;
	
	
	//left pane
	leftPane->Create(split.m_hWnd,rcDefault,NULL,dwSplitStyle,dwSplitExStyle);
	albumView1=new CAlbumCoverView;
	albumView2=new CAlbumCoverView;
	albumView1->Create(leftPane->m_hWnd);
	albumView2->Create(leftPane->m_hWnd);
	leftPane->SetSplitterPanes(albumView1->m_hWnd,albumView2->m_hWnd);

	//set left and right
	split.SetSplitterPanes(leftPane->m_hWnd,m_view.m_hWnd,TRUE);
	UpdateLayout();

	//set the splitter pos
	split.SetSplitterPos(250,TRUE);
	leftPane->SetSplitterPos(500,TRUE);
	return 0;
}


 LRESULT CMainFrame::OnFftDialog(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
 {
//  	static DialogFFT *dlg=0;
//  	if (!dlg)
//  	{
//  		dlg=new DialogFFT;
//  		dlg->dscrl=m_pDsoundControl;
//  		dlg->Create(this->m_hWnd);
//  		m_pDsoundControl->SetShowWindow(&(dlg->m_hWnd));
//  	}
//  
//  
//  	dlg->ShowWindow(SW_SHOW);
 	return 0;
 }

LRESULT CMainFrame::OnPlay(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	g_pSharedPlayer->play();
	return 0;
}

LRESULT CMainFrame::OnOpen(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CFileDialog dlg(TRUE,NULL,NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,_T("wave files(*.wav) \0 *.wav\0 mp3 files\0 *.mp3"),this->m_hWnd);
	if(dlg.DoModal()!=IDOK)return 0;

	LPTSTR filepath=dlg.m_ofn.lpstrFile;

	g_pSharedPlayer->open(filepath);

	g_pSharedPlayer->play();
	return 0;
}

LRESULT CMainFrame::OnPause(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	g_pSharedPlayer->pause();
	return 0;
}

LRESULT CMainFrame::OnStop(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	g_pSharedPlayer->stop();

	return 0;
}

LRESULT CMainFrame::OnConfig(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	if (!::IsWindow(m_dlgConfig.m_hWnd))
		m_dlgConfig.Create(m_hWnd);
	else
		m_dlgConfig.SetFocus();
	
	m_dlgConfig.ShowWindow(SW_SHOW);
	return 0;
}


LRESULT CMainFrame::OnFileOpendirectory(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	UINT uFlags=BIF_RETURNONLYFSDIRS|BIF_NEWDIALOGSTYLE;
	uFlags&=~BIF_NONEWFOLDERBUTTON ;
	CFolderDialog dlg(m_hWnd,_T("请选择要添加的文件夹"),uFlags);
	if (dlg.DoModal()==IDOK)
	{
		LPCTSTR path=dlg.GetFolderPath();
		TCHAR *tmp=new TCHAR[MAX_PATH];
		_tcscpy(tmp,path);
		PlayList::AddFolderToCurrentPlayList(tmp);
		
		list<PlayListItem> *songList=&(MyLib::GetPlayListObj().m_songList);
		list<PlayListItem>::iterator i;
		int j;

		for (j=0,i=songList->begin();i!=songList->end();i++,j++)
		{
			int k=0;
			m_view.InsertItem(j,i->url.c_str());

			m_view.SetItemText(j,k++,i->title.c_str());
			m_view.SetItemText(j,k++,i->album.c_str());
			m_view.SetItemText(j,k++,i->artist.c_str());
			m_view.SetItemText(j,k++,i->genre.c_str());
			m_view.SetItemText(j,k++,i->comment.c_str());
		
			m_view.SetItemData(j,(DWORD_PTR)&(*i));
		}
	}
	return 0;
}



LRESULT CMyView::OnDbClicked(UINT i,CPoint pt)
{
	LVITEM item;
	GetSelectedItem(&item);
	PlayListItem *listItem=(PlayListItem*)GetItemData(item.iItem);
	if (!listItem)
		return 0;

	if (!g_pSharedPlayer->stoped())
		g_pSharedPlayer->stop();
	if(g_pSharedPlayer->open((LPTSTR)listItem->url.c_str())==TRUE)
		g_pSharedPlayer->play();

	::SetWindowText(m_pMainFrame->m_hWnd,listItem->title.c_str());
	

	MyLib::GetPlayListObj().SetCurPlaying(listItem);
	m_pMainFrame->albumView1->Invalidate(TRUE);

	SetMsgHandled(FALSE);
	return 0;
}


LRESULT CMainFrame::OnShowLyric(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	static CDialogLyric *dlg=NULL;
	if (!dlg)
	{
		dlg=new CDialogLyric;
		dlg->Create(this->m_hWnd);
		m_dlgLrc=dlg;
		dlg->GetClientRect(&dlg->rc);
	}

	dlg->ShowWindow(SW_SHOW);
	return 0;
}

LRESULT CMainFrame::OnPos(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	double used,lefted;
	used=wParam;
	lefted=lParam;
	bHandled=FALSE;
	m_trackBar.SetRange(0,used+lefted);
	m_trackBar.SetPos(used);
	if (m_dlgLrc && ::IsWindow(m_dlgLrc->m_hWnd))
		::PostMessage(m_dlgLrc->m_hWnd,WM_TRACKPOS,wParam,lParam);

	return 1;
}


LRESULT CMainFrame::OnCbnSelchanged(UINT,int id, HWND hWndCtl)
{
	if (::IsWindow(m_wndComboBox.m_hWnd) && hWndCtl==m_wndComboBox.m_hWnd)
	{
		int i=m_wndComboBox.GetCurSel();
		MyLib::GetPlayListObj().SetPlayOrder(static_cast< PlayOrderIndex >( i ));
	}

	return 0;
}


LRESULT CMainFrame::OnTrackStopped(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	if (g_pSharedPlayer->stoped())
	{
		const TCHAR* tmp= g_pSharedPlayer->playNextPlaylistItem();
		if(tmp)
			SetWindowText(tmp);
	}
	return 0;
}