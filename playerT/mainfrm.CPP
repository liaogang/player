#include "StdAfx.h"
#include "MainFrm.h"
#include "PlayList.h"
#include "DialogFFT.h"
#include "DialogLyric.h"
#include "BasicPlayer.h"
#include "CAlbumCoverView.h"
#include "MyControls.h"
#include "CMyTabBar.h"
#include "DlgProcessing.h"

#include <mpg123.h>
#include <string>
using namespace std;

#define APPNAME "abc player"

BOOL CMainFrame::OnIdle()
{
	UIUpdateToolBar();
	BOOL flag=MyLib::shared()->SelPlaylist()!=NULL;
	UIEnable(ID_FILE_OPENDIRECTORY,flag);
	UIEnable(ID_FILE_OPEN,flag);
	UIEnable(ID_FILE_SAVEPLAYLIST,flag);
	return FALSE;
}

LRESULT CMainFrame::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	SetWindowText(_T(APPNAME));

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
	//stanted toobar 
	HWND hWndToolBar = CreateSimpleToolBarCtrl(m_hWnd,IDR_MAINFRAME, FALSE, ATL_SIMPLE_TOOLBAR_PANE_STYLE);
	CreateSimpleReBar(ATL_SIMPLE_REBAR_NOBORDER_STYLE);
	AddSimpleReBarBand(hWndCmdBar);
	AddSimpleReBarBand(hWndToolBar, NULL,TRUE,110,TRUE);

	//------------------------------------------------
	//progress track bar
	m_pTrackBar=new CMyTrackBar;
	UINT style=WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
	style|= TBS_TOOLTIPS  |TBS_NOTICKS;
	HWND hWndProgressBar=m_pTrackBar->Create(m_hWnd,NULL,NULL,style,0);
	m_pTrackBar->SetPageSize(1);
	m_pTrackBar->SetLineSize(1);
	m_pTrackBar->SetThumbLength(30);
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


	//-----------------------------------------
	//volume track bar
	m_pVolumeBar=new CMyVolumeBar;
	HWND hWndVolumeBar=m_pVolumeBar->Create(m_hWnd,NULL,NULL,style,0);
	m_pVolumeBar->SetPageSize(1);
	m_pVolumeBar->SetLineSize(1);
	m_pVolumeBar->SetThumbLength(10);
	m_pVolumeBar->SetRange(0,10000);
	m_pVolumeBar->SetPos(10000);
	AddSimpleReBarBand(hWndVolumeBar,NULL,FALSE,50,TRUE);


	//-------------------------------------------------
	//status bar  
	//add it all
	CreateSimpleStatusBar();
	m_pStatus=new CMyStatusBar;
	m_pStatus->SubclassWindow(m_hWndStatusBar);
		
	UIAddToolBar(hWndToolBar);
	UISetCheck(ID_VIEW_TOOLBAR, 1);
	UISetCheck(ID_VIEW_STATUS_BAR, 1);

	DWORD dwSplitStyle = WS_CHILD|WS_VISIBLE| WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
	DWORD dwSplitExStyle =WS_EX_CLIENTEDGE;

	split.Create(m_hWnd, rcDefault, NULL, 
		dwSplitStyle, dwSplitExStyle );

	//right pane
	//------------------------------------------------
	//tab bar
	m_pTabBar=new CMyTabBar;
	m_pTabBar->Create(split.m_hWnd,rcDefault,NULL,WS_CHILD|WS_VISIBLE,WS_EX_STATICEDGE);

	m_pPlaylistView=new CPlayListView;
	m_pPlaylistView->Create(m_pTabBar->m_hWnd, rcDefault, NULL, LVS_SINGLESEL | WS_CHILD | WS_VISIBLE |WS_CLIPSIBLINGS  | LVS_REPORT | LVS_SHOWSELALWAYS, /*WS_EX_CLIENTEDGE*/NULL);
		
	//left pane
	leftPane=new CHorSplitterWindow;
	leftPane->Create(split.m_hWnd,rcDefault,NULL,dwSplitStyle,NULL);
	albumView1=new CAlbumCoverView;
	albumView1->Create(leftPane->m_hWnd);
	leftPane->SetSplitterPane(0,albumView1->m_hWnd);
	
	//set left and right
	split.SetSplitterPanes(leftPane->m_hWnd,m_pTabBar->m_hWnd,TRUE);

	//-----------------------------------------
	m_hWndClient=split.m_hWnd;
	UpdateLayout();

	split.SetSplitterPos(250,TRUE);
	leftPane->SetSplitterPos(500,TRUE);

	// register object for message filtering and idle updates
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->AddMessageFilter(this);
	pLoop->AddIdleHandler(this);

	//set main pointer
	m_pPlaylistView->SetMain(this);
	CBasicPlayer::shared()->m_pMainFrame=this;
	m_pTabBar->pMain=this;
	m_pStatus->pMain=this;

	return 0;
}


LRESULT CMainFrame::OnNotify(int idCtrl, LPNMHDR pnmh)
{
	if (pnmh->code==TCN_SELCHANGE)
	{
		//drop list combo box , changed
		if (idCtrl==m_wndComboBox.GetDlgCtrlID())
		{
// 			int sel=m_pTabBar->GetCurSel();
// 			OnTabChanged(sel);
		}

		if (idCtrl==m_pTabBar->GetDlgCtrlID())
		{
			OnSelectionChanged();
		}
	}
	
	//track bar  mouse released
	if (pnmh->code==NM_RELEASEDCAPTURE && idCtrl==m_pTrackBar->GetDlgCtrlID())
	{
		int pos=m_pTrackBar->GetPos();
		int posMax=m_pTrackBar->GetRangeMax();

		CBasicPlayer::shared()->SetPos(pos,posMax);
		m_pTrackBar->m_bPressing=FALSE;
	}

	

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
	MyLib::play();
	return 0;
}

LRESULT CMainFrame::OnOpen(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	const TCHAR szFilter[]=_T("wave files(*.wav)\0*.wav\0mp3 files(*.mp3)\0*.mp3\0");
	CFileDialog dlg(TRUE,NULL,NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,szFilter,this->m_hWnd);
	if(dlg.DoModal()!=IDOK)return 0;

	LPTSTR filepath=dlg.m_ofn.lpstrFile;

	//g_pSharedPlayer->open(filepath);

	//g_pSharedPlayer->play();
	return 0;
}

LRESULT CMainFrame::OnPause(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	MyLib::pause();
	return 0;
}

LRESULT CMainFrame::OnStop(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	MyLib::stop();

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
		CProcessingDlg *pDlgProcess=NULL;
		pDlgProcess=new CProcessingDlg;
		pDlgProcess->Create(m_hWnd);
		pDlgProcess->ShowWindow(SW_SHOW);


		LPCTSTR path=dlg.GetFolderPath();
		TCHAR *tmp=new TCHAR[MAX_PATH];
		_tcscpy(tmp,path);
		MyLib::SetMain(m_hWnd);
		MyLib::AddFolderToCurrentPlayList(tmp);
	}
	return 0;
}

LRESULT CMainFrame::OnAddFolder(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	PlayList *playlist=MyLib::shared()->SelPlaylist();
	if (playlist)
		m_pPlaylistView->Reload(playlist);
	return 0;
}


LRESULT CMainFrame::OnShowLyric(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CDialogLyric *dlg=NULL;
	if (!dlg)
	{
		dlg=new CDialogLyric;
		dlg->Create(this->m_hWnd);
		m_dlgLrc=dlg;
		dlg->GetClientRect(&dlg->rc);
		dlg->TrackChanged();
	}

	dlg->ShowWindow(SW_SHOW);
	return 0;
}


LRESULT CMainFrame::OnNewTrackStarted(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	if (m_dlgLrc)
		m_dlgLrc->TrackChanged();

	return 0;
}


LRESULT CMainFrame::OnPos(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (!m_pTrackBar->m_bPressing)
	{
		double used,lefted;
		used=wParam;
		lefted=lParam;
		bHandled=FALSE;
		m_pTrackBar->SetRange(0,used+lefted);
		m_pTrackBar->SetPos(used);
	}

	if (m_dlgLrc && ::IsWindow(m_dlgLrc->m_hWnd))
		::PostMessage(m_dlgLrc->m_hWnd,WM_TRACKPOS,wParam,lParam);

	return 1;
}


LRESULT CMainFrame::OnCbnSelchanged(UINT,int id, HWND hWndCtl)
{
	if (::IsWindow(m_wndComboBox.m_hWnd) && hWndCtl==m_wndComboBox.m_hWnd)
	{
		int i=m_wndComboBox.GetCurSel();
		MyLib::shared()->ActivePlaylist()->SetPlayOrder(static_cast< PlayOrderIndex >( i ));
	}

	return 0;
}


LRESULT CMainFrame::OnTrackStopped(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	MyLib::playNext();

	SetWindowText(MyLib::shared()->ActivePlaylist()->curTrack()->GetTitle());

	bHandled=TRUE;
	return 0;
}


LRESULT CMainFrame::OnFileNewPlaylist(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	PlayList *pPl=MyLib::shared()->NewPlaylist();
	m_pTabBar->AddPlaylistTab(pPl,TRUE,-1);
	//tell the play list view to add this
	m_pPlaylistView->Reload(pPl);

	return 0;
}


LRESULT CMainFrame::OnOpenPlaylist(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	const TCHAR szFilter[]=_T("playlist files(*.pl)\0*.pl\0");

	CFileDialog dlg(TRUE,NULL,NULL,OFN_FILEMUSTEXIST|OFN_DONTADDTORECENT,szFilter,m_hWnd);
	if(dlg.DoModal()!=IDOK)return 0;

	LPTSTR filepath=dlg.m_ofn.lpstrFile;

	MyLib::shared()->LoadPlaylist(filepath);

	//tell the play list view to add this
	::PostMessage(m_hWnd,WM_ADDFOLDERED,NULL,NULL);
	
	return 0;
}


LRESULT CMainFrame::OnSavePlaylist(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	const TCHAR szFilter[]=_T("playlist files(*.pl)\0*.pl\0");

	CFileDialog dlg(FALSE,NULL,NULL,OFN_OVERWRITEPROMPT,szFilter ,m_hWnd);
	if(dlg.DoModal()!=IDOK)return 0;

	LPTSTR filepath=dlg.m_ofn.lpstrFile;
	
	//if isn't end with .pl , add it
	int len=_tcslen(filepath);
	int len1=len;
	TCHAR* p=(TCHAR*)filepath+len;
	while (p--)
	{
		if ((TCHAR)(*p)=='.')
		{
			break;
		}
		if (--len==0)
			break;
	}

	if (_tcscmp(p,_T(".pl"))!=0)
	{
		TCHAR *pStr=new TCHAR[len1+4];
		_tcsncpy(pStr,filepath,len1);
		_tcsncpy(pStr+len1,_T(".pl\0"),4);

		MyLib::shared()->SaveCurPlaylist(pStr);
	}

	return 0;
}


void CMyStatusBar::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	PlayList *curPl=MyLib::shared()->ActivePlaylist();
	if (!curPl)
		return;

	PlayListItem *track=curPl->curTrack();
	if (!track)
		return;

	int i=track->indexInListView;
	if (i!=-1)
	{
		pMain->m_pPlaylistView->SetFocus();
		pMain->m_pPlaylistView->SetItemState(i,LVIS_FOCUSED|
		LVIS_SELECTED,LVIS_FOCUSED|LVIS_SELECTED);	
		pMain->m_pPlaylistView->EnsureVisible(i,FALSE);
	}
}

void CMainFrame::OnSelectionChanged()
{
	int curSel=m_pTabBar->GetCurSel( );
	m_pTabBar->SetActiveTab(curSel);
	PlayList* pPl=(PlayList*)m_pTabBar->GetTabParam(curSel);
	m_pPlaylistView->Reload(pPl);

	MyLib::shared()->SetSelPlaylist(pPl);
	return;
}

void CPlayListView::SetMain(class CMainFrame *pMain){this->pMain=pMain;}

void CPlayListView::Reload(PlayList* pPl)
{
	list<PlayListItem>::iterator i;	 int j;

	DeleteAllItems();
	for (i=pPl->m_songList.begin(),j=0;i!=pPl->m_songList.end();i++,j++)
	{
		int k=0;

		InsertItem(j,i->url.c_str());
		SetItemText(j,k++,i->title.c_str());
		SetItemText(j,k++,i->artist.c_str());
		SetItemText(j,k++,i->album.c_str());

		TCHAR year[256]={};
#ifdef _UNICODE
		wsprintf(year,_T("%u"),i->year);
#else
		sprintf(year,_T("%u"),i->year);
#endif
		SetItemText(j,k++,(const TCHAR*)year);
		SetItemText(j,k++,i->comment.c_str());
		SetItemText(j,k++,i->genre.c_str());

		SetItemData(j,(DWORD_PTR)&(*i));
		(*i).indexInListView=j;
	}
}


void PlayListViewItem(int index)
{

}

void CPlayListView::PlaySelectedItem()
{
	LVITEM item;
	PlayListItem *curSelectedTrack;

	GetSelectedItem(&item);
	if (item.iItem==-1)
		return;

	curSelectedTrack=(PlayListItem*)GetItemData(item.iItem);
	if (!curSelectedTrack)
		return;

	MyLib* s=MyLib::shared();

	s->SetActivePlaylist(s->SelPlaylist());
	s->ActivePlaylist()->SetCurPlaying(curSelectedTrack);

	MyLib::stop();
	MyLib::play();

	::SetWindowText(pMain->m_hWnd,curSelectedTrack->GetTitle());

	pMain->albumView1->Invalidate(TRUE);	
}


void MyLib::SetMain(HWND hMain){shared()->hMain=hMain;};
HWND MyLib::GetMain(){return shared()->hMain;};

BOOL CMyTabBar::AddPlaylistTab(PlayList* ppl,BOOL inActiveFlag , int inImage )
{
	AddTab(ppl->m_playlistName.c_str(),pMain->m_pPlaylistView->m_hWnd,inActiveFlag,inImage,(LPARAM)ppl);
	MyLib::shared()->SetSelPlaylist(ppl);
	return 0;
}


LRESULT CMainFrame::OnExit(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	delete MyLib::shared();
	bHandled=FALSE;
	return 0;
}
