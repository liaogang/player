#include "StdAfx.h"
#include "MainFrm.h"
#include "MyControls.h"
#include "PlayList.h"
#include "BasicPlayer.h"
#include "CAlbumCoverView.h"
#include "CMyTabBar.h"
#include "DlgProcessing.h"
//#include "PlayListView.h"
#include "PlayListViewMng.h"
#include "DialogSearch.h"
#include "DialogLyric.h"
#include "DialogFFTOutline.h"
#include "PlayListManager.h"
#include "glassWnd.h"
#include <mpg123.h>
#include <string>

using namespace std;



BOOL CMainFrame::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message==WM_KEYDOWN)
	{
		UINT nChar=(TCHAR)pMsg->wParam;
		if(nChar==VK_F3)
			ShowSearchDialog();
		if(nChar=='f' ||nChar=='F')
			if (GetKeyState(VK_CONTROL) &0x80)
				ShowSearchDialog();
	}

	return CFrameWindowImpl<CMainFrame>::PreTranslateMessage(pMsg);
}


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
	SetWindowText(GetAppName());

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
	//standed toobar 
	HWND hWndToolBar = CreateSimpleToolBarCtrl(m_hWnd,IDR_MAINFRAME, FALSE, ATL_SIMPLE_TOOLBAR_PANE_STYLE);

	CreateSimpleReBar(ATL_SIMPLE_REBAR_NOBORDER_STYLE);
	m_pRebar=new CMySimpleRebar;
	m_pRebar->SubclassWindow(m_hWndToolBar);


	AddSimpleReBarBand(hWndCmdBar);
	AddSimpleReBarBand(hWndToolBar, NULL,TRUE,0,TRUE);
	
	
	//------------------------------------------------
	//progress track bar
	m_pTrackBar=new CMyTrackBar;
	UINT style=WS_CHILD | WS_TABSTOP | WS_VISIBLE /*| WS_CLIPCHILDREN | WS_CLIPSIBLINGS*/;
	style|=TBS_TOOLTIPS  |TBS_NOTICKS |TBS_AUTOTICKS  | TBS_BOTH ; 
	UINT styleEx=0; 
	HWND hWndProgressBar=m_pTrackBar->Create( m_hWndToolBar,NULL,NULL,style,styleEx);
	m_pTrackBar->SetPageSize(1);
	m_pTrackBar->SetLineSize(1);
	m_pTrackBar->SetThumbLength(30);
	m_pRebar->pTrack=m_pTrackBar;


	//HRESULT result=EnableThemeDialogTexture(hWndProgressBar,ETDT_ENABLE);

	//result=EnableThemeDialogTexture(hWndToolBar,ETDT_ENABLE);

	AddSimpleReBarBand(hWndProgressBar,NULL,FALSE,150,TRUE);

	//-------------------------------------------------
	//combo box
	HWND comboBar=m_wndComboBox.Create(m_hWndToolBar,NULL,NULL,WS_CHILD|WS_VISIBLE|WS_VSCROLL|CBS_DROPDOWNLIST);

	for (int i=0;i<sizeof(gPlayOrderStr)/sizeof(int);++i)
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
	HWND hWndVolumeBar=m_pVolumeBar->Create(m_hWndToolBar,NULL,NULL,style,styleEx);
	m_pVolumeBar->SetPageSize(1);
	m_pVolumeBar->SetLineSize(1);
	m_pVolumeBar->SetThumbLength(2);
	m_pVolumeBar->SetRange(0,100);
	m_pVolumeBar->SetPos(100);
	m_pRebar->pVolume=m_pVolumeBar;
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


	//-----------------------------------------
	//split
	DWORD dwSplitStyle = WS_CHILD|WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
	DWORD dwSplitExStyle =0;

	m_pSplit=new CMySplitterWindow;
	m_pSplit->Create(m_hWnd, rcDefault, NULL, 
		dwSplitStyle, dwSplitExStyle );

	//right pane
	//------------------------------------------------
	//tab bar
	m_pTabBar=new CMyTabBar;
	//m_pTabBar->Create(m_pSplit->m_hWnd,rcDefault,NULL,WS_CHILD/*|WS_VISIBLE*/|WS_CLIPCHILDREN | WS_CLIPSIBLINGS,0/*WS_EX_STATICEDGE*/);

	
	m_pPlaylistView=new CPlayListView;
	m_pPlaylistView->Create(m_pSplit->m_hWnd, rcDefault, NULL, /*LVS_OWNERDRAWFIXED|*/ WS_CHILD | WS_VISIBLE /*|WS_CLIPCHILDREN | WS_CLIPSIBLINGS*/| LVS_REPORT | LVS_SHOWSELALWAYS |LVS_OWNERDATA, 0);
	m_pPlaylistView->Init();	


	//left pane
	leftPane=new CHorSplitterWindow;
	leftPane->Create(m_pSplit->m_hWnd,rcDefault,0,dwSplitStyle);


	albumView1=new CAlbumCoverView;
	albumView1->Create(leftPane->m_hWnd,0,0,dwSplitStyle);
	lyricView=new CWndLyric;
	lyricView->enableDebug=1;
	lyricView->Create(leftPane->m_hWnd);
	lyricView->Init();

	leftPane->SetSplitterPanes(albumView1->m_hWnd,lyricView->m_hWnd);
	
	//set left and right
	//m_pSplit->SetSplitterPanes( m_pTabBar->m_hWnd,leftPane->m_hWnd);
	m_pSplit->SetSplitterPanes( m_pPlaylistView->m_hWnd,leftPane->m_hWnd);
	
	//-----------------------------------------
	//EnableThemeDialogTexture(m_pTabBar->m_hWnd,ETDT_ENABLE);
	//EnableThemeDialogTexture(m_hWnd,ETDT_ENABLE);
	m_hWndClient=m_pSplit->m_hWnd;

	UpdateLayout();
	leftPane->SetSplitterPos(130);
	m_pSplit->SetSplitterPos(482);
	

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


	InitData();


	return 0;
}



LRESULT CMainFrame::OnNotify(int idCtrl, LPNMHDR pnmh)
{
	if (pnmh->code==TCN_SELCHANGE){
		//drop list combo box , changed
		if (pnmh->hwndFrom==m_wndComboBox.m_hWnd){
// 			int sel=m_pTabBar->GetCurSel();
// 			OnTabChanged(sel);
		}

// 		if (pnmh->hwndFrom==m_pTabBar->m_hWnd){
// 			OnSelectionChanged();
// 		}
	}
	
	//track bar  mouse released
	if (pnmh->code==NM_RELEASEDCAPTURE && pnmh->hwndFrom==m_pTrackBar->m_hWnd){
		int pos=m_pTrackBar->GetPos();
		int posMax=m_pTrackBar->GetRangeMax();

		CBasicPlayer::shared()->SetPos(pos,posMax);
		m_pTrackBar->m_bPressing=FALSE;
	}


	//when rebar height changed ,we update the layout
	if(pnmh->code==RBN_HEIGHTCHANGE && pnmh->hwndFrom==m_hWndToolBar)
		UpdateLayout();
	
	return 0;
} 



 LRESULT CMainFrame::OnFftDialog(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
 {
 	if (!m_pDlgFFTOutline)
 	{
 		m_pDlgFFTOutline=new DialogFFTOutline;
 		m_pDlgFFTOutline->Create(m_hWnd);
 	}
 
 
 	m_pDlgFFTOutline->Show();

 	return 0;
 }

LRESULT CMainFrame::OnPlay(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	MyLib::stop();
	MyLib::playAfterSlowDown();
	return 0;
}

LRESULT CMainFrame::OnOpen(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	const TCHAR szFilter[]=_T("mp3 files(*.mp3)\0*.mp3\0wave files(*.wav)\0*.wav\0");
	CFileDialog dlg(TRUE,NULL,NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,szFilter,this->m_hWnd);
	if(dlg.DoModal()==IDOK)
	{
		PlayList* selPl=MyLib::shared()->SelPlaylist();
		if(selPl)
		{
			selPl->AddFile(dlg.m_ofn.lpstrFile);
			AllPlayListViews()->Reload(selPl);
			//m_pPlaylistView->Reload(selPl);
		}
	}
	
	return 0;
}

LRESULT CMainFrame::OnPause(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	MyLib::pause();
	ChangeTitle();
	return 0;
}

LRESULT CMainFrame::OnStop(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	MyLib::stop();
	ChangeTitle();
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
		ShowDlgProcessFile();

		LPCTSTR path=dlg.GetFolderPath();
		TCHAR *tmp=new TCHAR[MAX_PATH];
		_tcscpy(tmp,path);
		MyLib::SetMain(m_hWnd);
		PlayList* pList=MyLib::AddFolderToCurrentPlayList(tmp);
		pDlgProcess->pList=pList;
	}
	return 0;
}

LRESULT CMainFrame::OnAddFolder(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	PlayList *playlist=MyLib::shared()->SelPlaylist();

	if (playlist){
		//m_pTabBar->AddPlaylistTab(playlist,TRUE,-1);
		AllPlayListViews()->Reload(playlist);
		//m_pPlaylistView->Reload(playlist);
	}
	return 0;
}


LRESULT CMainFrame::OnConfigLyric(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	if (!::IsWindow(m_dlgConfig.m_hWnd))
		m_dlgConfig.Create(m_hWnd);

	m_dlgConfig.ShowWindow(SW_SHOW);
	std::wstring strLyricDlg(L"歌词搜索");
	m_dlgConfig.ShowDlgByName(strLyricDlg);
	return 0;
}


LRESULT CMainFrame::OnConfigMediaLib(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	if (!::IsWindow(m_dlgConfig.m_hWnd))
		m_dlgConfig.Create(m_hWnd);

	m_dlgConfig.ShowWindow(SW_SHOW);
	std::wstring strLyricDlg(L"媒体库");
	m_dlgConfig.ShowDlgByName(strLyricDlg);
	
	return 0;
}

SearchLyric search1;
LRESULT CMainFrame::OnShowLyric(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	static CDialogLyric *dlg=NULL;
	if (!dlg){
		dlg=new CDialogLyric;
		dlg->Create(this->m_hWnd);
		m_dlgLrc=dlg;
		dlg->GetClientRect(&dlg->rc);
		dlg->TrackChanged();
		dlg->foo();
	}

	dlg->ShowWindow(SW_SHOW);
	



	//test
// 	GlassWnd *pGlassWnd=new GlassWnd;
// 	RECT rc;
// 	GetClientRect(&rc);
// 	pGlassWnd->Create(this->m_hWnd,rc,0,WS_CHILD|WS_VISIBLE);
// 	pGlassWnd->foo();
	//test
	

	return 0;
}

void CMainFrame::lrcChanged()
{
	if (m_dlgLrc)
		m_dlgLrc->TrackChanged();

	//::PostMessage(m_hWnd,WM_LYRIC_RELOAD,0,0);

	if(lyricView)
		lyricView->TrackChanged();
}


LRESULT CMainFrame::OnNewTrackStarted(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	ChangeTitle();

	lrcChanged();

	albumView1->TrackChanged();

	return 0;
}

LRESULT CMainFrame::OnPlayDirectly(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	MyLib::stop();
	MyLib::play();
	return 0;
}

LRESULT CMainFrame::OnPos(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	trackPosInfo *posInfo=(trackPosInfo*)wParam;

	if (!m_pTrackBar->m_bPressing)
	{
		bHandled=FALSE;
		m_pTrackBar->SetRange(0,(int)(posInfo->used+posInfo->left));
		m_pTrackBar->SetPos((int)posInfo->used);
	}

	if (m_dlgLrc && ::IsWindow(m_dlgLrc->m_hWnd))
		::PostMessage(m_dlgLrc->m_hWnd,WM_TRACKPOS,wParam,lParam);
	if (lyricView)
		::PostMessage(lyricView->m_hWnd,WM_TRACKPOS,wParam,lParam);
	

	//AtlTrace("delete %p",posInfo);
	//delete posInfo;
	return 1;
}


LRESULT CMainFrame::OnCbnSelchanged(UINT,int id, HWND hWndCtl)
{
	if (::IsWindow(m_wndComboBox.m_hWnd) && hWndCtl==m_wndComboBox.m_hWnd){
		int i=m_wndComboBox.GetCurSel();
		MyLib::shared()->SetPlayOrder(static_cast<MyLib::PlayOrder>( i ));
	}

	return 0;
}


LRESULT CMainFrame::OnTrackStopped(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	MyLib::playNext();

	ChangeTitle();

	bHandled=TRUE;
	return 0;
}


LRESULT CMainFrame::OnFileNewPlaylist(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	PlayList *pPl=MyLib::shared()->NewPlaylist();
	
	AllPlayListViews()->Reload(pPl);
	//m_pPlaylistView->Reload(pPl);
	MyLib::shared()->SetSelPlaylist(pPl);

	//m_pTabBar->AddPlaylistTab(pPl,TRUE,-1);

	return 0;
}


LRESULT CMainFrame::OnOpenPlaylist(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	const TCHAR szFilter[]=_T("playlist files(*.pl)\0*.pl\0");
	const TCHAR szDefaultExt[]=_T("pl");
	CFileDialog dlg(TRUE,szDefaultExt,NULL,OFN_FILEMUSTEXIST|OFN_DONTADDTORECENT,szFilter,m_hWnd);
	if(dlg.DoModal()==IDOK)
	{
		PlayList* playlist =MyLib::shared()->LoadPlaylist(dlg.m_ofn.lpstrFile);
		m_pTabBar->AddPlaylistTab(playlist ,TRUE,-1);

		////tell the play list view to add this
		//::PostMessage(m_hWnd,WM_ADDFOLDERED,NULL,NULL);
	}

	return 0;
}


LRESULT CMainFrame::OnSavePlaylist(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	PlayList* pl=MyLib::shared()->SelPlaylist();
	if(pl)
	{
		const TCHAR szFilter[]=_T("playlist files(*.pl)\0*.pl\0");
		const TCHAR szDefaultExt[]=_T("pl");
		CFileDialog dlg(FALSE,szDefaultExt,NULL,OFN_OVERWRITEPROMPT,szFilter ,m_hWnd);
		if(dlg.DoModal()==IDOK)
			MyLib::SavePlaylist(pl,dlg.m_ofn.lpstrFile);
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

	int k=0;
	PlayList::_songContainer::iterator i=curPl->m_songList.begin();
	for (i=curPl->m_songList.begin();((*i)!=track) && i!=curPl->m_songList.end();i++,k++);


	WCHAR tmp[MAX_PATH]={};
	wcscpy(tmp,track->artist.c_str());
	wcscat(tmp,L" - ");
	wcscat(tmp,track->title.c_str());
	GetMainFrame()->m_pStatus->SetText(0,tmp);

	//TODO
	//pMain->m_pTabBar->SetCurSel(curPl,k);
	
	AllPlayListViews()->Reload(curPl,k);
	//pMain->m_pPlaylistView->Reload(curPl,k);
	pMain->m_pPlaylistView->SetFocus();

// 	int i=track->indexInListView;
// 	pMain->m_pPlaylistView->ClearAllSel();
// 
// 	if (i!=-1){
// 		pMain->m_pPlaylistView->SetFocus();
// 		pMain->m_pPlaylistView->SetItemState(i,LVIS_FOCUSED|
// 			LVIS_SELECTED,LVIS_FOCUSED|LVIS_SELECTED);	
// 		pMain->m_pPlaylistView->EnsureVisible(i,FALSE);
// 	}
}


//no used
void CMainFrame::OnSelectionChanged()
{
	ATLASSERT(FALSE);

	int curSel=m_pTabBar->GetCurSel( );
	m_pTabBar->SetActiveTab(curSel);
	PlayList* pPl=(PlayList*)m_pTabBar->GetTabParam(curSel);
	m_pPlaylistView->Reload(pPl);

	MyLib::shared()->SetSelPlaylist(pPl);
	return;
}

void CPlayListView::SetMain(class CMainFrame *pMain){this->pMain=pMain;}

void CPlayListView::InsertTrackItem(PlayListItem &track,int item,BOOL SetIndex)
{
	//for visual list
	//InsertItem(item,NULL);

// 	int k=0;
// 	InsertItem(item,track.url.c_str());
// 	SetItemText(item,k++,track.title.c_str());
// 	SetItemText(item,k++,track.artist.c_str());
// 	SetItemText(item,k++,track.album.c_str());
// 
// 	TCHAR year[256]={};
// 	wsprintf(year,_T("%u"),track.year);
// 
// 	SetItemText(item,k++,(const TCHAR*)year);
// 	SetItemText(item,k++,track.comment.c_str());
// 	SetItemText(item,k++,track.genre.c_str());
// 
// 	SetItemData(item,(DWORD_PTR)&track);

	if(SetIndex)
		track.indexInListView=item;
}




void PlayListViewItem(int index)
{

}

void CPlayListView::PlayItem(int nItem)
{
	MyLib* s=MyLib::shared();
	//搜索列表,实际上是用的tab某个列表的数据
	PlayListItem *track;
	if(nItem==-1)return;

	track=GetPlayList()->m_songList[nItem];	

	s->SetActivePlaylist(track->m_pPL);
	s->ActivePlaylist()->SetCurPlaying(track,FALSE);

	MyLib::stop();
	MyLib::playAfterSlowDown();

	pMain->ChangeTitle();

	pMain->albumView1->Invalidate(TRUE);	
}


void MyLib::SetMain(HWND hMain){shared()->hMain=hMain;};
HWND MyLib::GetMain(){return shared()->hMain;};

LRESULT CMainFrame::OnExit(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	delete MyLib::shared();
	bHandled=FALSE;
	return 0;
}



void CMainFrame::ChangeTitle()
{
	PlayList *ppl=MyLib::shared()->ActivePlaylist();
	if(!ppl)return;
	PlayListItem* track=ppl->curTrack();
	if(!track)return;
	const TCHAR *trackName=track->GetTitle();
	
	std::tstring title;
	if(CBasicPlayer::shared()->m_bStopped)
	{
		title+=GetAppName();
	}
	else if (CBasicPlayer::shared()->m_bPaused)
	{
		title+=const_cast<TCHAR*>( trackName);
		title+=_T(" (Paused)");	
	}
	else 
	{
		title+=const_cast<TCHAR*>(trackName);
	}

	SetWindowText(title.c_str());
}


void CMainFrame::ShowSearchDialog()
{
	if (!m_pDlgSearch)
	{
		m_pDlgSearch=new DialogSearch;
		m_pDlgSearch->pM=this;
		m_pDlgSearch->Create(m_hWnd);
	}
	
	m_pDlgSearch->ShowSelf();
}

void CMainFrame::InitData()
{
	//mylib init will load cfg files 
	//include playlist and lrc search paths
	MyLib *s=MyLib::shared();

	if (s->m_playLists.size()>0)
	{
		MyLib::PLList::iterator i;
		for (i=s->m_playLists.begin();i!=s->m_playLists.end();++i)
		{;
				//m_pTabBar->AddPlaylistTab(*i,TRUE,-1);
		}

		i--;
		m_pPlaylistView->Reload(*i);
	}

	s->InitLrcLib();
}


void CMainFrame::ShowDlgProcessFile()
{
	if (!pDlgProcess)
	{
		pDlgProcess=new CProcessingDlg;
		pDlgProcess->Create(m_hWnd);
	}

	pDlgProcess->ShowWindow(SW_SHOW);
}


LRESULT CMainFrame::OnFileFinded(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	pDlgProcess->ShowFileName((TCHAR*)wParam,(BOOL)lParam);
	return 0;
}

LRESULT CMainFrame::OnChangePLColorDefault(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	m_pPlaylistView->ChangeColorDefault();
	return 0;
}

LRESULT CMainFrame::OnChangePLColorBlue(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	m_pPlaylistView->ChangeColorBlue();
	return 0;
}

LRESULT CMainFrame::OnViewPlaylistManager(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	static DialogPLManager *dlgPLMng=NULL;
	if(!dlgPLMng){
	dlgPLMng=new DialogPLManager;
	dlgPLMng->m_list.pMain=this;
	dlgPLMng->Create(m_hWnd);
	}

	dlgPLMng->ShowSelf();	
	return 0;
}

void CPlayListManager::ReFillPlaylist()
{
	ClearAllSel();

	MyLib *s=MyLib::shared();
	int item=0;
	if (s->m_playLists.size()>0)
	{
		MyLib::PLList::iterator i;
		for (i=s->m_playLists.begin();i!=s->m_playLists.end();++i)
		{
			PlayList *l=*i;
			TCHAR *strCount=new TCHAR[256];
			_itow(l->m_songList.size(),strCount,10);

			InsertItem(item,l->m_playlistName.c_str());
			SetItemText(item,1,strCount);
			SetItemData(item,(DWORD_PTR)l);
			++item;

			delete[] strCount;
		}
	}
}

LRESULT  CProcessingDlg::OnAbort(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	pList->TerminateAddDirThread();
	return 0;
}



void CMyVolumeBar::OnPos(int pos)
{
	CBasicPlayer::shared()->SetVolumeByEar(pos);
}




LRESULT CPlayListView::OnGetdispInfo(int /**/,NMHDR *pNMHDR,BOOL bHandled)
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	LV_ITEM* pItem= &(pDispInfo)->item;
	int iItemIndx= pItem->iItem;

	PlayListItem *track=GetPlayList()->m_songList[iItemIndx];
	if(!track) return 0;
	TCHAR year[25]={0};
	TCHAR strIndex[5]={0};
	if (pItem->mask & LVIF_TEXT) //valid text buffer?
	{
		switch(pItem->iSubItem){
		case COLUMN_INDEX_INDEX:
			wsprintf(strIndex,_T("%d"),track->itemIndex);
			lstrcpy(pItem->pszText,strIndex);
			break;
		case COLUMN_INDEX_TITLE: //fill in main text
			lstrcpy(pItem->pszText, 
				track->title.c_str());
			break;
		case COLUMN_INDEX_ARTIST: //fill in sub item 1 text
			lstrcpy(pItem->pszText,
				track->artist.c_str());
			break;
		case COLUMN_INDEX_ALBUM: //fill in sub item 2 text
			lstrcpy(pItem->pszText,
				track->album.c_str());
			break;
		case COLUMN_INDEX_YEAR:
			wsprintf(year,_T("%u"),track->year);
			lstrcpy(pItem->pszText,year);
			break;
		case COLUMN_INDEX_COMMENT:
			lstrcpy(pItem->pszText,track->comment.c_str());
			break;
		case COLUMN_INDEX_GENRE:
			lstrcpy(pItem->pszText,track->genre.c_str());
			break;
		}
	}

	return 1;
}


LRESULT CMyTabBar::OnSelectionChanged( LPNMHDR )
{
	int curSel=GetCurSel( );

	PlayList* pPl=GetTabPlayList(curSel);
	MyLib::shared()->SetSelPlaylist(pPl);

	SetActiveTab( curSel);
	pMain->m_pPlaylistView->Reload(pPl);
	pMain->m_pPlaylistView->SetFocus();
	return 0;
}

void CMyTabBar::SetCurSel(PlayList* ppl,int index)
{
	int nItem;

	for (nItem=0;GetItemCount();nItem++)
	{
		PlayList* pplOnTab=(PlayList*)GetTabParam(nItem);
		if(pplOnTab==ppl)
		{
			SetActiveTab(nItem);
			pMain->m_pPlaylistView->Reload(ppl,index);
			break;
		}
	}
}


LRESULT  CPlayListManager::OnDbClicked(UINT i,CPoint pt)
	{
		int k=GetFirstSelItem();
		if(k!=-1)
		{
			PlayList *l=(PlayList*)GetItemData(k);

			MyLib::shared()->SetSelPlaylist(l);

			
			pMain->m_pPlaylistView->Reload(l);
			pMain->m_pPlaylistView->SetFocus();
		}

		SetMsgHandled(FALSE);
		return 0;
	} 

BOOL CMyTabBar::AddPlaylistTab(PlayList* ppl,BOOL inActiveFlag , int inImage )
{
	//int nNewTab=AddTab(ppl->m_playlistName.c_str(),pMain->m_pPlaylistView->m_hWnd,inActiveFlag,inImage,(LPARAM)ppl);

// 	pMain->m_pPlaylistView->Reload(ppl);
// 	MyLib::shared()->SetSelPlaylist(ppl);

	return 0;
}

LRESULT CMyTabBar::OnPlDel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	PlayList* pl= GetTabPlayList(RtBtnTab);

	RemoveTab(RtBtnTab);
	pMain->m_pPlaylistView->Reload(GetTabPlayList(GetCurSel()));

	MyLib::shared()->DeletePlayList(pl);

	return 0;
}
LRESULT CMyTabBar::OnPlRename(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	PlayList* pl= GetTabPlayList(RtBtnTab);

	// 	CFileDialog 
	// 	{
	// 		pl->Rename();
	// 	}

	return 0;
}
LRESULT CMyTabBar::OnPlSave(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	PlayList* pl= GetTabPlayList(RtBtnTab);
	if(pl)
	{
		const TCHAR szFilter[]=_T("playlist files(*.pl)\0*.pl\0");
		const TCHAR szDefaultExt[]=_T("pl");
		CFileDialog dlg(FALSE,szDefaultExt,NULL,OFN_OVERWRITEPROMPT,szFilter ,m_hWnd);
		if(dlg.DoModal()==IDOK)
			MyLib::SavePlaylist(pl,dlg.m_ofn.lpstrFile);
	}

	return 0;
}

LRESULT CMyTabBar::OnPlOpenFolder(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	PlayList* pl= GetTabPlayList(RtBtnTab);
	if(pl)
	{
		std::tstring parameters(_T("/select,"));
		parameters+=pl->m_saveLocation;

		ShellExecute(NULL,
			_T("open"),
			_T("explorer"),
			parameters.c_str(),
			0,
			SW_SHOWNORMAL
			);
	}

	return 0;
}




LRESULT DialogFFT::OnFullScreen(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	parent->OnFullScreen();

// 	if (!bFullScreen)
// 	{
// 		RECT windowRC;
// 		RECT clientRC;
// 
// 		GetWindowPlacement(&oldWpm);
// 
// 		GetWindowRect(&windowRC);	
// 		GetClientRect(&clientRC);
// 		ClientToScreen(&clientRC);
// 
// 
// 		int nFullWidth=GetSystemMetrics(SM_CXSCREEN);
// 		int nFullHeight=GetSystemMetrics(SM_CYSCREEN);
// 
// 
// 		fullScreecRC.left=windowRC.left-clientRC.left;
// 		fullScreecRC.top=windowRC.top-clientRC.top;
// 		fullScreecRC.right=nFullWidth+(windowRC.right-clientRC.right);
// 		fullScreecRC.bottom=nFullHeight+(windowRC.bottom-clientRC.bottom);
// 
// 		WINDOWPLACEMENT wpm;
// 		wpm.length=sizeof(WINDOWPLACEMENT);
// 		wpm.flags=0;
// 		wpm.showCmd=SW_SHOWNORMAL;
// 		wpm.rcNormalPosition=fullScreecRC;
// 
// 		SetWindowPlacement(&wpm);
// 		SetWindowPos(HWND_TOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
// 		bFullScreen=TRUE;
// 	}
// 	else
// 	{
// 		ShowWindow(SW_HIDE);
// 		SetWindowPlacement(&oldWpm);
// 		SetWindowPos(HWND_NOTOPMOST,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
// 		bFullScreen=FALSE;
// 	}
	return 0;	
}









unsigned int BKDRHash(char *str)
{
	unsigned int seed = 131; // 31 131 1313 13131 131313 etc..
	unsigned int hash = 0;

	while (*str){
		hash = hash * seed + (*str++);
	}

	return (hash & 0x7FFFFFFF);
}


HMENU LoadPlaylistMenu(BOOL bDestroy)
{
	static HMENU menu=NULL;
	static HMENU subMenu=NULL;
	if (!bDestroy)
	{
		if (!menu)
		{	
			menu=::LoadMenu(NULL,MAKEINTRESOURCE(IDR_MENU_PLAYLIST));
			subMenu=::GetSubMenu(menu,0);
		}
	}
	else
	{
		if(menu)
		{
			::DestroyMenu(menu);
			menu=NULL;
		}
	}

	return subMenu;
}






LRESULT CListLrcReportView::OnDbClicked(UINT i,CPoint pt)
{
	int k=GetFirstSelItem();
	if(k!=-1)
	{
		parent->DownLoad();
	}
	return 0;
}

void CListLrcReportView::ReFillPlaylist()
{
	DeleteAllItems();
	
	for (int i=0;i<parent->searchTool.vecLrcLines.size()/3;i+=3)
	{	
		LPWSTR wAr=UTF82Unicode((char*)parent->searchTool.vecLrcLines[i*3+1].c_str());
		LPWSTR wTi=UTF82Unicode((char*)parent->searchTool.vecLrcLines[i*3+2].c_str());

		InsertItem(i,wTi);
		SetItemText(GetItemCount()-1,1,wAr);

		delete[] wAr;
		delete[] wTi;
	}

	SetItemState(0,0,LVNI_SELECTED|LVNI_FOCUSED);
	SetFocus();
}







void CDlgLrcSearch::DownLoad()
{
	int item=m_list.GetSelectedIndex();
	if (item==-1)
	{	
		savePath=NULL;
		return;
	}

	const WCHAR csStr1[]={L"歌词已保存到:"};
	const int   len=sizeof(csStr1)/sizeof(WCHAR) -1;
	WCHAR artist[MAX_PATH]={},title[40]={};

	wcscpy(artist,csStr1);
	
	if(savePath==NULL)
	{
		::GetWindowText(GetDlgItem(IDC_EDIT_AR),artist+len,sizeof(artist)-len);
		::GetWindowText(GetDlgItem(IDC_EDIT_TI),title,sizeof(title));

		const TCHAR szFilter[]=_T("歌词文件(*.lrc)\0*.lrc\0文本文件(*.txt)\0*.txt\0");
		CFileDialog dlg(FALSE,L"lrc",NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,szFilter,m_hWnd);
		
		wcscat(artist,L" - ");
		wcscpy(dlg.m_szFileName,wcscat(artist,title)+len);

		if(dlg.DoModal()==IDOK)
			savePath=dlg.m_ofn.lpstrFile;
	}

	wcscpy(artist+len,savePath);

	if(searchSuccess)
	{
		::SetWindowText(GetDlgItem(IDC_EDIT_INFO),L"下载中...");
		BOOL saved=searchTool.Download(item,savePath);
		if (saved)
		{
			GetMainFrame()->m_pStatus->SetText(0,artist);

			//check box selected?
			if((int)::SendMessage(GetDlgItem(IDC_CHECK), BM_GETCHECK, 0, 0L))
				HideSelf();
			else
				::SetWindowText(GetDlgItem(IDC_EDIT_INFO),L"下载成功");

			std::tstring pszpath(savePath);
			if(MyLib::shared()->ActivePlaylist() && MyLib::shared()->ActivePlaylist()->curTrack())
			{
				MyLib::shared()->ActivePlaylist()->curTrack()->TryLoadLrcFile(pszpath);
				GetMainFrame()->lrcChanged();
			}
		}
	}


	savePath=NULL;

}






void MyLib::play()
{
	PlayList* playlist=MyLib::shared()->ActivePlaylist();
	if (!playlist)
		return;

	PlayListItem* curTrack=playlist->curTrack();
	curTrack->ScanId3Info(TRUE,TRUE);

	CBasicPlayer* s=CBasicPlayer::shared();
	if(s->open(curTrack))
	{
		s->play();
		GetMainFrame()->m_pStatus->SetText(0,L"正在播放...");
	}
}


void MyLib::pause()
{
	CBasicPlayer::shared()->pause();
	if (CBasicPlayer::shared()->m_bPaused)
	{
		GetMainFrame()->m_pStatus->SetText(0,L"已暂停");
	}
	else
	{
		GetMainFrame()->m_pStatus->SetText(0,L"正在播放...");
	}
	
}

void MyLib::stop()
{
	if (!CBasicPlayer::shared()->stoped())
	{
		CBasicPlayer::shared()->stop();
		GetMainFrame()->m_pStatus->SetText(0,L"已停止");
	}
}