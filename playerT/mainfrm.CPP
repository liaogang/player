#include "StdAfx.h"
#include <string>
#include "mytree.h"
#include "MainFrm.h"
#include "MyControls.h"
#include "PlayList.h"
#include "BasicPlayer.h"
#include "mysplit.h"
#include "globalStuffs.h"


#include "DialogConfig.h"
#include "CAlbumCoverView.h"
#include "CMyTabBar.h"
#include "DlgProcessing.h"
#include "PlayListViewMng.h"
#include "DialogSearch.h"
#include "DialogLyric.h"
#include "DialogFFTOutline.h"
#include "DlgConsole.h"
#include "PlayListManager.h"
#include "glassWnd.h"
#include <mpg123.h>
#include "MyConfigs.h"
#include "Util1.h"

using namespace std;


void CMainFrame::RegisterMyHotKeys()
{
	// 获取当前窗口句柄 
	HWND handle = m_hWnd;
	//得到热键的唯一标识
	HotKeyId = GlobalAddAtom(_T("Player_LG_HotKey")); 
	//注册全局热键Alt + F8
	RegisterHotKey(handle,HotKeyId,NULL, VK_MEDIA_NEXT_TRACK);
	RegisterHotKey(handle,HotKeyId,NULL, VK_MEDIA_STOP);
	RegisterHotKey(handle,HotKeyId,NULL, VK_MEDIA_PLAY_PAUSE);
}


LRESULT CMainFrame::OnHotKey(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{

	UINT fuModifiers = (UINT) LOWORD(lParam);  // key-modifier flags   
	UINT uVirtKey = (UINT) HIWORD(lParam);     // virtual-key code    
	UINT nChar=uVirtKey;

	//Next Tract
	if(nChar == VK_MEDIA_NEXT_TRACK)
		OnPlayNext(0,0,0,bHandled);

	//Prev track
	//if(nChar == VK_MEDIA_PREV_TRACK)

	//Stop
	if(nChar == VK_MEDIA_STOP)
		OnStop(0,0,0,bHandled);

	//Play or Pause
	if(nChar == VK_MEDIA_PLAY_PAUSE    )
		OnPause(0,0,0,bHandled);


	bHandled=TRUE;

	return 0;
}

BOOL CMainFrame::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message==WM_KEYDOWN)
	{
		UINT nChar=(TCHAR)pMsg->wParam;
		BOOL bHandled=FALSE;

		//F3 or Ctrl + F 
		if(nChar==VK_F3)
			ShowSearchDialog();
		if(nChar=='f' ||nChar=='F')
			if (GetKeyState(VK_CONTROL) & 0x80)
				ShowSearchDialog();
			//Ctrl + P
		if(nChar == 'P' || nChar =='p')
			if (GetKeyState(VK_CONTROL) & 0x80)
				OnConfig(0,0,0,bHandled);
	}

	return CFrameWindowImpl<CMainFrame>::PreTranslateMessage(pMsg);
}


BOOL CMainFrame::OnIdle()
{
	UIUpdateToolBar();
	BOOL flag=SelectedPlaylist()!=NULL;
	UIEnable(ID_FILE_OPENDIRECTORY,flag);
	UIEnable(ID_FILE_OPEN,flag);
	UIEnable(ID_FILE_SAVEPLAYLIST,flag);
	return FALSE;
}

LRESULT CMainFrame::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	LoadAll();

	InitData();

	if(m_rcMain.right>0 && m_rcMain.bottom > 0)
	::SetWindowPos(m_hWnd, NULL, m_rcMain.left, m_rcMain.top, m_rcMain.right - m_rcMain.left, m_rcMain.bottom - m_rcMain.top,SWP_NOZORDER);

	SetWindowText(GetAppName());

	SetTimer((UINT_PTR)&m_nIDEvent,m_uElapse,NULL);


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
	//SetMenu(pMenu->m_hMenu);

	//-----------------------------------------
	//standed toobar 
	HWND hWndToolBar = CreateSimpleToolBarCtrl(m_hWnd,IDR_MAINFRAME, FALSE, ATL_SIMPLE_TOOLBAR_PANE_STYLE);

	//CreateSimpleReBar(ATL_SIMPLE_REBAR_STYLE);
	
	CreateSimpleReBar(ATL_SIMPLE_REBAR_NOBORDER_STYLE);
	m_pRebar=new CMySimpleRebar;
	m_pRebar->SubclassWindow(m_hWndToolBar);


	AddSimpleReBarBand(hWndCmdBar);
	AddSimpleReBarBand(hWndToolBar, NULL,TRUE,0,TRUE);
	
	
	//------------------------------------------------
	//progress track bar
	m_pTrackBar=new CMyTrackBar;
	UINT style=WS_CHILD  | WS_VISIBLE /*| WS_CLIPCHILDREN | WS_CLIPSIBLINGS*/;
	style|=TBS_TOOLTIPS  |TBS_NOTICKS |TBS_AUTOTICKS  | TBS_BOTH ; 
	UINT styleEx=0; 

	HWND hWndProgressBar=m_pTrackBar->CreateIsWnd(m_hWndToolBar);
	m_pRebar->pTrack=m_pTrackBar;


	//HRESULT result=EnableThemeDialogTexture(hWndProgressBar,ETDT_ENABLE);

	//result=EnableThemeDialogTexture(hWndToolBar,ETDT_ENABLE);

	AddSimpleReBarBand(hWndProgressBar,NULL,FALSE,280,TRUE);

	
	//combo box
	m_pComboBox=new CMyComboBox;
	HWND comboBar=m_pComboBox->CreateIsWnd(m_hWndToolBar);

	
// 	 	CFont font;
// 	 	LOGFONT lf;
// 	 	memset(&lf, 0, sizeof(LOGFONT));       // zero out structure
// 	 	//lf.lfHeight = 20;                      // request a 12-pixel-height font
// 	 	lf.lfItalic=TRUE;
// 	 	//lf.lfCharSet=
// 	 	lf.lfUnderline=TRUE;
// 	 	_tcscpy((TCHAR*)lf.lfFaceName, _T("Arial"));        // request a face name "Arial"
// 	 	font.CreateFontIndirect(&lf);
// 	 	//m_wndComboBox.GetFont();
// 	 	m_wndComboBox.SetFont(font,TRUE);
	AddSimpleReBarBand(comboBar,NULL,FALSE,140,TRUE);


	
	//volume track bar
	m_pVolumeBar=new CMyVolumeBar;
	HWND hWndVolumeBar=m_pVolumeBar->Create(m_hWndToolBar,NULL,NULL,style,styleEx);
	m_pVolumeBar->Init();
	
	m_pRebar->pVolume=m_pVolumeBar;
	AddSimpleReBarBand(hWndVolumeBar,NULL,FALSE,70,TRUE);


	
	//status bar  
	//add it all
	CreateSimpleStatusBar();
	m_pStatus=new CMyStatusBar;
	m_pStatus->SubclassWindow(m_hWndStatusBar);
	m_pStatus->Init();

	UIAddToolBar(hWndToolBar);
	UIAddStatusBar(m_hWndStatusBar);

	UISetCheck(ID_VIEW_TOOLBAR, 1);
	UISetCheck(ID_VIEW_STATUS_BAR, 1);

	InitUILayout();

	// register object for message filtering and idle updates
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->AddMessageFilter(this);
	pLoop->AddIdleHandler(this);

	

	

	RegisterMyHotKeys();

	SetMsgHandled(FALSE);
	return 0;
}



void CMainFrame::InitUILayout()
{
	const DWORD dwSplitStyle = WS_CHILD|WS_VISIBLE | WS_CLIPCHILDREN |WS_CLIPSIBLINGS  ;
	const DWORD dwSplitExStyle =WS_EX_CLIENTEDGE;

	m_pWndMultiSplitter=new CMultiSpliltWnd;
	m_hWndClient=m_pWndMultiSplitter->Create(m_hWnd,rcDefault,NULL,dwSplitStyle,0);
}


HWND CMyComboBox::CreateIsWnd(HWND parent)
{
	Create(parent,NULL,NULL,WS_CHILD|WS_VISIBLE|WS_HSCROLL | WS_VSCROLL|CBS_DROPDOWNLIST);


	for (int i=0;i<sizeof(gPlayOrderStr)/sizeof(gPlayOrderStr[0]);++i)
		AddString(gPlayOrderStr[i]);

	SetCurSel(MyLib::shared()->playorder);

	return m_hWnd;
}



LRESULT CMainFrame::OnNotify2(int idCtrl, LPNMHDR pnmh)
{
// 	if (pnmh->code==TCN_SELCHANGE){
// 		//drop list combo box , changed
// 		if (pnmh->hwndFrom==m_wndComboBox.m_hWnd){
// // 			int sel=m_pTabBar->GetCurSel();
// // 			OnTabChanged(sel);
// 		}
// 
// 	}
	
	//track bar  mouse released
	/*
	if (pnmh->code==NM_RELEASEDCAPTURE && pnmh->hwndFrom==m_pTrackBar->m_hWnd){
		int pos=m_pTrackBar->GetPos();
		int posMax=m_pTrackBar->GetRangeMax();

		CBasicPlayer::shared()->SetPos(pos,posMax);
		m_pTrackBar->m_bPressing=FALSE;
	}
	*/

	//when rebar height changed ,we update the layout
	if(pnmh->code==RBN_HEIGHTCHANGE && pnmh->hwndFrom==m_hWndToolBar)
		UpdateLayout();
	
	return 0;
} 


LRESULT CMyTrackBar::OnMouseReleased(int /*idCtrl*/, LPNMHDR pnmh, BOOL& bHandled)
{
	m_bPressing=false;
	ReleaseCapture();

	int pos=GetPos();
	int posMax=GetRangeMax();
	
	CBasicPlayer::shared()->SetPos(pos,posMax);

	m_uCurrTime=pos * 1000 ;

	return 0;
}


/*
LRESULT CMySimpleRebar::OnNotify(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	int idCtrl;
	LPNMHDR pnmh;
	pnmh= (LPNMHDR)lParam;

	//track bar  mouse released
	if (pnmh->code==NM_RELEASEDCAPTURE && pnmh->hwndFrom==GetMainFrame()->m_pTrackBar->m_hWnd){
		int pos=GetMainFrame()->m_pTrackBar->GetPos();
		int posMax=GetMainFrame()->m_pTrackBar->GetRangeMax();

		CBasicPlayer::shared()->SetPos(pos,posMax);
		GetMainFrame()->m_pTrackBar->m_bPressing=FALSE;
	}


	bHandled=FALSE;
	return 0;
}
*/

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
	MyLib::shared()->ClearPlayQueue();
	//MyLib::stop();

	_songContainerItem track=MyLib::shared()->GetWaitPlayItem();
	if(track->isValide())
	{
		MyLib::shared()->play(track);
	}
	//RedrawItems(nItemPlaying,nItemPlaying);

	return 0;
}

LRESULT CMainFrame::OnOpen(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	const TCHAR szFilter[]=_T("mp3 files(*.mp3)\0*.mp3\0wave files(*.wav)\0*.wav\0");
	CFileDialog dlg(TRUE,NULL,NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,szFilter,m_hWnd);
	dlg.m_ofn.Flags|=OFN_ALLOWMULTISELECT;

	if(dlg.DoModal()==IDOK)
	{
		PlayList* selPl=MyLib::shared()->GetSelectedPL();
		if(selPl)
		{
			TCHAR *p=dlg.m_ofn.lpstrFile;

			TCHAR tmpFileName[MAX_PATH]={};
			_tcscat(_tcscpy(tmpFileName,p),_T("\\"));

			p+=dlg.m_ofn.nFileOffset;
			while(*p)
			{
				int len=1+_tcslen(p);//with null string len
				
				_tcsncpy(tmpFileName+dlg.m_ofn.nFileOffset,p,len);
				if(!selPl->AddFile(tmpFileName))
				{
					//Show Error
				}

				p+=len;
			}


			AllPlayListViews()->Reload(selPl);
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

LRESULT CMainFrame::OnPlayNext(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	MyLib::shared()->playNext();
	return 0;
}

LRESULT CMainFrame::OnConfig(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	if (!m_pDlgConfig)
	{
		m_pDlgConfig=new CDialogConfig;

		m_pDlgConfig->Create(m_hWnd,m_rcConfig);

		if(m_rcConfig.right>0 && m_rcConfig.bottom > 0)
			::SetWindowPos(m_pDlgConfig->m_hWnd, NULL, m_rcConfig.left, m_rcConfig.top, m_rcConfig.right - m_rcConfig.left, m_rcConfig.bottom - m_rcConfig.top,SWP_NOZORDER);

	}
	

	m_pDlgConfig->SetFocus();
	
	m_pDlgConfig->ShowWindow(SW_SHOW);
	return 0;
}


LRESULT CMainFrame::OnSaveConfig(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	SaveAll();

	return 0;
}



LRESULT CMainFrame::OnConfigLayout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	if (!m_pDlgConfig)
	{
		m_pDlgConfig=new CDialogConfig;
		m_pDlgConfig->Create(m_hWnd);
	}

	m_pDlgConfig->ShowWindow(SW_SHOW);
	m_pDlgConfig->ShowDlgByName(std::wstring(L"界面布局"));

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
		
		MyLib::AddFolderToCurrentPlayList(tmp);
	}
	return 0;
}

LRESULT CMainFrame::OnChangeLVFont(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	AllPlayListViews()->EnlargeLVFont(wID==ID_LV_ENLARGE_FONT?1:-1);
	
	if (m_pDlgSearch)
		m_pDlgSearch->m_list.EnlargeLVFont(wID==ID_LV_ENLARGE_FONT?1:-1);

	return 0;
}

LRESULT CMainFrame::OnAddFolder(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	//msg send to playlist track num changed
	/*
	PlayList *playlist=MyLib::shared()->SelPlaylist();
	
	if (playlist){
		//m_pTabBar->AddPlaylistTab(playlist,TRUE,-1);
		AllPlayListViews()->Reload(playlist);
		
		if (m_pDlgPLMng && m_pDlgPLMng->IsWindow())
		{
			m_pDlgPLMng->m_list.ReFillPlaylist();
		}
	}*/


	return 0;
}


void CPlayListManager::AddPlayList(PlayList *pPL)
{
	int item=GetItemCount();
	TCHAR strCount[256];
	_itow(pPL->m_songList.size(),strCount,10);

	InsertItem(item,pPL->m_playlistName.c_str());
	SetItemText(item,1,strCount);
	SetItemText(item,2,pPL->m_bAuto?_T("自动列表"):_T("普通列表"));
	SetItemData(item,(DWORD_PTR)pPL);
	//++item;
}

void CPlayListManager::DelPlayList(PlayList *pPL)
{
	int c=GetItemCount();
	for (int i=0;i<c;++i)
	{
		auto pl= (PlayList*)GetItemData(i);
		if (pl==pPL){
			DeleteItem(i);
			break;
			}
	}
}

void CPlayListManager::UpdateByPLTrack(PlayList *pPL)
{
	int c=GetItemCount();
	for (int i=0;i<c;++i)
	{
		auto pl= (PlayList*)GetItemData(i);
		if (pl==pPL){
			SetItemText(i,0,pPL->m_playlistName.c_str());

			TCHAR strCount[256];
			_itow(pPL->m_songList.size(),strCount,10);
			SetItemText(i,1,strCount);
			break;
			}
	}
}

LRESULT CMainFrame::OnConfigLyric(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	if (!m_pDlgConfig)
	{
		m_pDlgConfig=new CDialogConfig;
		m_pDlgConfig->Create(m_hWnd);
	}

	m_pDlgConfig->ShowWindow(SW_SHOW);
	m_pDlgConfig->ShowDlgByName(std::wstring(L"歌词搜索"));
	return 0;
}


LRESULT CMainFrame::OnConfigMediaLib(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	if (!m_pDlgConfig)
	{
		m_pDlgConfig=new CDialogConfig;
		m_pDlgConfig->Create(m_hWnd);
	}

	m_pDlgConfig->ShowWindow(SW_SHOW);
	m_pDlgConfig->ShowDlgByName(std::wstring(L"多媒体库"));
	
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
	}

	dlg->ShowWindow(SW_SHOW);
	
	
	return 0;
}




LRESULT CMainFrame::OnShowConsole(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	if(!m_pDlgConsole)
	{
		m_pDlgConsole=new CDialogConsole;
		m_pDlgConsole->Create(m_hWnd);

		if(m_rcPLConsole.right>0 && m_rcPLConsole.bottom > 0)
			::SetWindowPos(m_pDlgConsole->m_hWnd, NULL, m_rcPLConsole.left, m_rcPLConsole.top, m_rcPLConsole.right - m_rcPLConsole.left, m_rcPLConsole.bottom - m_rcPLConsole.top,SWP_NOZORDER);
	}

	::ShowWindow(m_pDlgConsole->m_hWnd,SW_SHOW);
	
	return 0;
}

/*
void CMainFrame::lrcChanged()
{
	if (m_dlgLrc)
		m_dlgLrc->TrackChanged();

	//::PostMessage(m_hWnd,WM_LYRIC_RELOAD,0,0);

	if(lyricView)
		lyricView->TrackChanged();
}
*/

LRESULT CMainFrame::OnNewTrackStarted(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	ChangeTitle();


	//refresh the active playlist item. 
	if(m_pDlgPLMng && m_pDlgPLMng->IsWindow())
		m_pDlgPLMng->m_list.Invalidate();
	
	return 0;
}

//
//LRESULT CMainFrame::OnPlayDirectly(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
//{
//	MyLib::stop();
//	
//	_songContainerItem item=MyLib::shared()->GetWaitPlayItem();
//	//SetPlayingItem(*item);
//	//SetActivePlaylist(item->GetPlayList());
//
//	MyLib::shared()->play((FileTrack*)wParam);
//
//	return 0;
//}



//LRESULT CMainFrame::OnPos(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
/*{
	ATLASSERT(FALSE);
	trackPosInfo *posInfo=(trackPosInfo*)wParam;

	if (!m_pTrackBar->m_bPressing)
	{
		bHandled=FALSE;
		m_pTrackBar->SetRange(0,(int)(posInfo->used+posInfo->left));
		m_pTrackBar->SetPos((int)posInfo->used);
	}

	//if (m_dlgLrc && ::IsWindow(m_dlgLrc->m_hWnd))
	//	::PostMessage(m_dlgLrc->m_hWnd,WM_TRACKPOS,wParam,lParam);
	//if (lyricView)
//		::PostMessage(lyricView->m_hWnd,WM_TRACKPOS,wParam,lParam);
	

	//AtlTrace("delete %p",posInfo);
	//delete posInfo;
	return 1;
}
*/

LRESULT CMyComboBox::OnCbnSelchanged(UINT,int id, HWND hWndCtl)
{
	MyLib::shared()->SetPlayOrder(static_cast<MyLib::PlayOrder>( GetCurSel() ));
	

	SetMsgHandled(FALSE);
	return 0;
}



// LRESULT CMainFrame::OnSizing(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
// {
// 	m_pWndMultiSplitter->OnSizing();
// 	bHandled=FALSE;
// 	
// 	return 0;
// }



LRESULT CMainFrame::OnTrackReachEnd(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	MyLib::shared()->playNext();


	return 0;
}

LRESULT CMainFrame::OnTrackStopped(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	/*
	MyLib::shared()->playNext();

	ChangeTitle();
	*/

	return 0;
}

LRESULT CMainFrame::OnPLChanged(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	BOOL bAdd=(BOOL)lParam;
	PlayList *pl=(PlayList*)wParam;

	if (m_pDlgPLMng && m_pDlgPLMng->IsWindow())
	{
		if (bAdd)
			m_pDlgPLMng->m_list.AddPlayList(pl);
		else
			m_pDlgPLMng->m_list.DelPlayList(pl);
		
	}

	if (bAdd)
		AllPlayListViews()->Reload(pl);
	else
	{
		int size=MyLib::shared()->m_playLists.size();

		if(size==0)
			AllPlayListViews()->Reload(nullptr);
		else
			AllPlayListViews()->Reload(* MyLib::shared()->m_playLists.begin());
	}

	return 0;
}


LRESULT CMainFrame::OnPLTrackNumChanged(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	PlayList *pl=(PlayList*)wParam;
		
	int numberChanged=(int)lParam;
	if(numberChanged==-1)
	{
		AllPlayListViews()->DeleteOneTrackFirst(pl);
		return 0;
	}

	AllPlayListViews()->Reload(pl);

	if (m_pDlgPLMng && m_pDlgPLMng->IsWindow())
	{
		m_pDlgPLMng->m_list.UpdateByPLTrack(pl);
	}
	
	return 0;
}


LRESULT CMainFrame::OnFileNewPlaylist(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	PlayList *pPl=MyLib::shared()->NewPlaylist();
	
	AllPlayListViews()->Reload(pPl);
	SetSelectedPlaylist(pPl);

	return 0;
}


LRESULT CMainFrame::OnOpenPlaylist(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	const TCHAR szFilter[]=_T("playlist files(*.pl)\0*.pl\0");
	const TCHAR szDefaultExt[]=_T("pl");

	const TCHAR *strInitPath=ChangeCurDir2PlaylistPath();
	if(!strInitPath)
		strInitPath=ChangeCurDir2ModulePath(NULL);

	CFileDialog dlg(TRUE,szDefaultExt,NULL,OFN_FILEMUSTEXIST|OFN_DONTADDTORECENT,szFilter,m_hWnd);
	dlg.m_ofn.lpstrInitialDir=strInitPath;

	if(dlg.DoModal()==IDOK)
	{
		PlayList* playlist =MyLib::shared()->LoadPlaylist(dlg.m_ofn.lpstrFile);
		::PostMessage(m_hWnd,WM_ADDFOLDERED,NULL,NULL);
	}

	return 0;
}


LRESULT CMainFrame::OnSavePlaylist(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	PlayList* pl=SelectedPlaylist();
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
#ifdef _DEBUG
	ATLASSERT(bInit);
#endif

	//PlayList *curPl=MyLib::shared()->ActivePlaylist();

	//if (!MyLib::shared()->isPlaying())
	//	return;
	
	PlayList *pl=MyLib::shared()->GetPlayingPL();
	if(!pl)
		return;

	PlayListItem *item=pl->GetPlayingItem();
	if(!item->isValide()) 
		return;

	FileTrack *track=item->GetFileTrack();
	

// 	WCHAR tmp[MAX_PATH]={};
// 	wcscpy(tmp,track->artist.c_str());
// 	wcscat(tmp,L" - ");
// 	wcscat(tmp,track->title.c_str());
// 	
// 	//todo
// 	GetMainFrame()->m_pStatus->SetText(0,tmp);


	AllPlayListViews()->Reload(item->GetPlayList(),item->GetIndex());
	NotifyMsg(WM_PLAYLISTVIEW_SETFOCUS);
}


//no used
/*
void CMainFrame::OnSelectionChanged()
{
	ATLASSERT(FALSE);

	int curSel=m_pTabBar->GetCurSel( );
	m_pTabBar->SetActiveTab(curSel);
	PlayList* pPl=(PlayList*)m_pTabBar->GetTabParam(curSel);
	//m_pPlaylistView->Reload(pPl);

	SetActivePlaylist(pPl);
	return;
}*/


//void CPlayListView::InsertTrackItem(PlayListItem &track1,int item,BOOL SetIndex)
//{
//	//for visual list
//	//InsertItem(item,NULL);
//	FileTrack *track=track1.GetFileTrack();
//	int k=0;
//	InsertItem(item,track->url.c_str());
//	SetItemText(item,k++,track->title.c_str());
//	SetItemText(item,k++,track->artist.c_str());
//	SetItemText(item,k++,track->album.c_str());
//
//	//TCHAR year[256]={};
//	//wsprintf(year,_T("%u"),track.year);
//
//	//SetItemText(item,k++,(const TCHAR*)year);
//	//SetItemText(item,k++,track.comment.c_str());
//	//SetItemText(item,k++,track.genre.c_str());
//
//	//SetItemData(item,(DWORD_PTR)&track);
//
//	//if(SetIndex)
//	//	track.indexInListView=item;
//}




void PlayListViewItem(int index)
{

}

void CPlayListView::PlaySelectedItem(_songContainerItem *item)
{

}

void CPlayListView::PlayItem(int nItem)
{
	MyLib* s=MyLib::shared();
// 	if(s->ActivePlaylist())
// 	{
// 		PlayListItem *lastTrack=s->ActivePlaylist()->curTrack();
// 		//if (lastTrack)
// 		//	RedrawItems(lastTrack,lastTrack);
// 	}

	PlayListItem* track;
	if(nItem==-1)return;

	track=GetPlayList()->GetItem(nItem);


	MyLib::shared()->play(track);
	
	InvalidateItem(nItem);
}



LRESULT CMainFrame::OnExit(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	bHandled=FALSE;
	return 0;
}

void PlayList::TerminateAddDirThread()
{
	::TerminateThread(hAddDir,-1);
	NotifyMsg(WM_FILE_FINDED,NULL,(LPARAM)0);
	::PostMessage(GetMainFrame()->m_hWnd,WM_ADDFOLDERED,NULL,NULL);
}

void CMainFrame::ChangeTitle()
{						 
	PlayList *pl=MyLib::shared()->GetPlayingPL();
	if(!pl)
		return;

	PlayListItem *item = pl->GetPlayingItem();
	FileTrack *track = item->GetFileTrack();
	
		
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
		if(!m_pDlgSearch->Create(m_hWnd))
			return;

		if(m_rcSearch.right>0 && m_rcSearch.bottom > 0)
			::SetWindowPos(m_pDlgSearch->m_hWnd, NULL, m_rcSearch.left, m_rcSearch.top, m_rcSearch.right - m_rcSearch.left, m_rcSearch.bottom - m_rcSearch.top,SWP_NOZORDER);
	}
	
	m_pDlgSearch->ShowSelf();
}

void CMainFrame::InitData()
{
	RegisterCreateWndFuns(_T("播放列表"),MYTREE_Set_Playlist);
	RegisterCreateWndFuns(_T("封面面板"),MYTREE_Set_AlbumView);
	RegisterCreateWndFuns(_T("歌词面板"),MYTREE_Set_LyricView);
	RegisterCreateWndFuns(_T("频谱面板"),MYTREE_Set_SpectrumView);


	::PostMessage(m_hWnd,WM_ADDFOLDERED,NULL,NULL);
	
	IWantToReceiveMessage(WM_FILE_FINDED);
	IWantToReceiveMessage(WM_TRACK_REACH_END);
	IWantToReceiveMessage(WM_ADDFOLDERED);
	IWantToReceiveMessage(WM_NEW_TRACK_STARTED);
	IWantToReceiveMessage(WM_TRACKSTOPPED);
	IWantToReceiveMessage(WM_PL_CHANGED);
	IWantToReceiveMessage(WM_PL_TRACKNUM_CHANGED);
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
	if(!pDlgProcess)
		ShowDlgProcessFile();

	pDlgProcess->ShowFileName((TCHAR*)wParam,(BOOL)lParam);
	return 0;
}

LRESULT CMainFrame::OnChangePLColorDefault(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	NotifyMsg(WM_PLAYLISTVIEW_COLOR_DEFAULT);
	return 0;
}

LRESULT CMainFrame::OnChangePLColorBlue(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	NotifyMsg(WM_PLAYLISTVIEW_COLOR_BLUE);
	return 0;
}

LRESULT CMainFrame::OnViewPlaylistManager(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	if(!m_pDlgPLMng){
	m_pDlgPLMng=new DialogPLManager;
	m_pDlgPLMng->m_list.pMain=this;
	m_pDlgPLMng->Create(m_hWnd);

	if(m_rcPLMng.right>0 && m_rcPLMng.bottom > 0)
		::SetWindowPos(m_pDlgPLMng->m_hWnd, NULL, m_rcPLMng.left, m_rcPLMng.top, m_rcPLMng.right - m_rcPLMng.left, m_rcPLMng.bottom - m_rcPLMng.top,SWP_NOZORDER);
	}

	m_pDlgPLMng->ShowSelf();	
	return 0;
}

void CPlayListManager::ReFillPlaylist()
{
	DeleteAllItems();

	MyLib *s=MyLib::shared();
	if (s->m_playLists.size()>0)
	{
		MyLib::PLList::iterator i;
		for (i=s->m_playLists.begin();i!=s->m_playLists.end();++i)
		{
			PlayList *l=*i;
			AddPlayList(l);
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

	FileTrack *track=GetPlayList()->GetItem(iItemIndx)->GetFileTrack();
	if(!track) return 0;
	
	
	if (pItem->mask & LVIF_TEXT) //valid text buffer?
	{
		switch(pItem->iSubItem){
		case COLUMN_INDEX_INDEX:
			{
				TCHAR strIndex[4]={0};
				_itow(iItemIndx+1,strIndex,10);
				lstrcpy(pItem->pszText,strIndex);
			}
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
			lstrcpy(pItem->pszText,track->year.c_str());
			break;
		case COLUMN_INDEX_GENRE:
			lstrcpy(pItem->pszText,track->genre.c_str());
			break;
		}
	}

	return 1;
}


/*
LRESULT CMyTabBar::OnSelectionChanged( LPNMHDR )
{
	int curSel=GetCurSel( );

	PlayList* pPl=GetTabPlayList(curSel);
	SetActivePlaylist(pPl);

	//SetActiveTab( curSel);
	//pMain->m_pPlaylistView->Reload(pPl);
	//pMain->m_pPlaylistView->SetFocus();
	return 0;
}
*/
/*
void CMyTabBar::SetCurSel(PlayList* ppl,int index)
{
	int nItem;

	for (nItem=0;GetItemCount();nItem++)
	{
		PlayList* pplOnTab=(PlayList*)GetTabParam(nItem);
		if(pplOnTab==ppl)
		{
			SetActiveTab(nItem);
			//pMain->m_pPlaylistView->Reload(ppl,index);
			break;
		}
	}
}
*/


	LRESULT  CPlayListManager::OnDbClicked(UINT i,CPoint pt)
	{
		int k=GetFirstSelItem();
		if(k!=-1)
		{
			PlayList *l=(PlayList*)GetItemData(k);

			SetSelectedPlaylist(l);

			AllPlayListViews()->Reload(l);

			//pMain->m_pPlaylistView->Reload(l);
			//pMain->m_pPlaylistView->SetFocus();
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
	//pMain->m_pPlaylistView->Reload(GetTabPlayList(GetCurSel()));

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
// 		std::tstring parameters(_T("/select,"));
// 		parameters+=pl->m_saveLocation;
// 
// 		ShellExecute(NULL,
// 			_T("open"),
// 			_T("explorer"),
// 			parameters.c_str(),
// 			0,
// 			SW_SHOWNORMAL
// 			);
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

HMENU LoadPlaylistManagerMenu(BOOL bDestroy)
{
	static HMENU menu=NULL;
	static HMENU subMenu=NULL;
	if (!bDestroy)
	{
		if (!menu)
		{	
			menu=::LoadMenu(NULL,MAKEINTRESOURCE(IDR_MENU_PL_MNG));
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
	
	for (size_t i=0;i<parent->searchTool.vecLrcLines.size()/3;i+=3)
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



			/*
			std::tstring pszpath(savePath);
			if(ActivePlaylist() && MyLib::shared()->isPlaying())
			{
				GetPlayingItem().TryLoadLrcFile(pszpath,TRUE);
				NotifyMsg(WM_LYRIC_RELOAD);
			}
			*/


			MyLib::shared()->InitLrcLib();
			NotifyMsg(WM_LYRIC_RELOAD);

		}
	}


	savePath=NULL;

}






void MyLib::play(FileTrack  *track)
{
	ATLASSERT(track);

	track->ScanId3Info(TRUE,TRUE);

	CBasicPlayer* s=CBasicPlayer::shared();
	s->stop();
	if(s->open(track))
	{
		s->play();
	}
}


void MyLib::pause()
{
	CBasicPlayer::shared()->pause();

	
}

void MyLib::stop()
{
	CBasicPlayer::shared()->stop();
}

bool MyLib::isPlaying()
{
	CBasicPlayer *p=CBasicPlayer::shared();
	int a=p->stoped();
	return !p->stoped();
}

BOOL CPlayListView::IsAllSelectedItemInPlayQueue()
{
	MyLib *s=MyLib::shared();
	vector<int> v;

	CListArray < int >  aSelectedItems;
	if(GetSelectedItems(aSelectedItems))
		for (int i=0;i<aSelectedItems.GetSize();i++)
		{
			v=s->GetIndexInPlayQueue(GetPlayList()->GetItem(aSelectedItems[i]));
			if(v.empty())
				return FALSE;
		}

	return TRUE;
}

LRESULT CPlayListView::OnPushToPlayqueue(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	MyLib *s=MyLib::shared();
	CListArray < int >  aSelectedItems;
	if(GetSelectedItems(aSelectedItems))
		for (int i=0;i<aSelectedItems.GetSize();i++)
			s->PushPlayQueue(GetPlayList()->GetItem(aSelectedItems[i]));

	return 0;
}

LRESULT CPlayListView::OnDeleteFromPlayqueue(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{

	MyLib *s=MyLib::shared();
	CListArray < int >  aSelectedItems;
	if(GetSelectedItems(aSelectedItems))
		for (int i=0;i<aSelectedItems.GetSize();i++)
			s->DeleteFromPlayQueue(GetPlayList()->GetItem(aSelectedItems[i]));

	return 0;
}


void PlayListViewMng::PlayingItemChanged()
{
// 	if (!MyLib::shared()->isPlaying())
// 		return;
// 
// 	FileTrack *track=MyLib::shared()->GetPlayingItem().GetFileTrack();
// 
// 	PlayList *pPL=track->GetPlayList();
// 	if (pPL==MyLib::shared()->ActivePlaylist())
// 		for (container::iterator i=dataCollection.begin();i!=dataCollection.end();++i)
// 		{
// 			CPlayListView *playlistview=(CPlayListView*)(*i);
// 			PlayList *playlist=playlistview->GetPlayList();
// 			if (playlist==pPL)
// 			{
// 				playlistview->SetPlayingItem(track);
// 			}else
// 				playlistview->SetPlayingIdx(-1);
// 			
// 		}
}



void PlayListViewMng::Reload(PlayList *pPL,bool activeplaying)
{
	for (container::iterator i=dataCollection.begin();
		i!=dataCollection.end();++i)
	{
		(*i)->Reload(pPL,activeplaying);

		//int centerItem=(*i)->GetPlayList()->selectedIndex;

		//if(centerItem==INVALID_ITEM)
			//centerItem=(*i)->GetPlayList()->selectedIndex;

		//todo
		//(*i)->ClearAllSel();
		//(*i)->EnsureVisibleAndCentrePos(index);
		//(*i)->SelectAndFocusItem(index);
	}
}


LRESULT CPlayListView::OnItemChanged(int /**/,LPNMHDR pnmh,BOOL bHandled)
{
	NMLISTVIEW * pnml=(NMLISTVIEW *)pnmh;
	if(m_bC)
	{
		GetPlayList()->topVisibleIndex=pnml->iItem;
		GetPlayList()->selectedIndex=pnml->iItem;
		if (pnml->iItem!=-1)
		{
			_songContainerItem item= GetPlayList()->GetItem(pnml->iItem);
			MyLib::shared()->SetWaitPlayItem(item);
			//GetPlayList()->SetSelectedItem(pnml->iItem);

		}
	}
	return 1;
}

LRESULT CPlayListView::OnSelected(int /**/,LPNMHDR pnmh,BOOL bHandled)
{	
	NMLISTVIEW * pnml=(NMLISTVIEW *)pnmh;
	if(m_bC)
	{
		GetPlayList()->topVisibleIndex=pnml->iItem;
		GetPlayList()->selectedIndex=pnml->iItem;
		if (pnml->iItem!=-1)
		{
			_songContainerItem item= GetPlayList()->GetItem(pnml->iItem);
			MyLib::shared()->SetWaitPlayItem(item);
			//GetPlayList()->SetSelectedItem(pnml->iItem);

		}
	}
	return 0;
}


void CPlayListView::SortItems2()
{
	PlayList *pl=MyLib::shared()->GetPlayingPL();
	PlayListItem *curr=NULL;
	if(pl)
		curr= pl->GetPlayingItem();

	
	//两项相等的情况下,STL的DEBUG检验会失败.
#ifdef _DEBUG
	//sort(GetPlayList()->m_songList.begin(),GetPlayList()->m_songList.end(),CompareProc);
	reverse(GetPlayList()->m_songList.begin(),GetPlayList()->m_songList.end());
#else
	sort(GetPlayList()->m_songList.begin(),GetPlayList()->m_songList.end(),CompareProc);
#endif

	m_Order = m_Order  ^ ( 1<< columnIndexClicked );

	// 			//重新分配序号
	auto iter=GetPlayList()->m_songList.begin();
	for (int i=0;iter!=GetPlayList()->m_songList.end();++i,++iter)
	{
		PlayListItem *item=*iter;
		item->SetIndex(i);
	}

	//如果是正在播放的列表,定位至播放的音轨.
	if(GetPlayList()==MyLib::shared()->GetPlayingPL() && curr)
	{
		GetPlayList()->SetPlayingIndex(curr->GetIndex());
		Reload(GetPlayList(),false);
	}
	else
	{
		Invalidate();
	}

}



void SdMsg(UINT msg,BOOL bPost,WPARAM wparam,LPARAM lparam)
{
	static HWND HMainFrm=NULL;
	if (!HMainFrm)
	{
		HMainFrm=GetMainFrame()->m_hWnd;
		ATLASSERT(::IsWindow(HMainFrm));
	}

	if (bPost)
		::PostMessage(HMainFrm,msg,wparam,lparam);
	else
		::SendMessage(HMainFrm,msg,wparam,lparam);
}


void CMainFrame::UpdateTreeView(MYTREE *treeData)
{
	if(m_pDlgConfig)
		m_pDlgConfig->GetLayoutTagDlg()->UpdateTree(treeData);
}

void CMultiSpliltWnd::UpdateTree(MYTREE *treeData)
{
	GetMainFrame()->UpdateTreeView(treeData);
}





LRESULT CMultiSpliltWnd::OnAddEmptyPane(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	MYTREE_Add_Playlist(rclickTree);
	Invalidate();
	UpdateTree(rclickTree);
	return 0;
}



LRESULT CMultiSpliltWnd::OnAddPlaylistView(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	MYTREE_Add_Playlist(rclickTree);

	Invalidate();
	UpdateTree(rclickTree);

	return 0;
}

void CMyTreeView::TraverseSplitTreeAndShowInTreeView(MYTREE *cur,HTREEITEM item)
{
	for (;cur;cur=cur->next)
	{
		if (cur->hasChild())
		{
			HTREEITEM newItem=InsertItem(cur->data.nodeName,item,TVI_LAST);
			SetItemData(newItem,(DWORD)cur);
			cur->data.treeItem=newItem;

			TraverseSplitTreeAndShowInTreeView(cur->child,newItem);
			Expand(newItem,TVE_EXPAND);
		}
		else
		{
			HTREEITEM newItem= InsertItem(cur->data.nodeName,item,TVI_LAST);
			SetItemData(newItem,(DWORD)cur);
			cur->data.treeItem=newItem;
		}
	}
}

void CMyTreeView::UpdateTree(MYTREE *treeData)
{
	HTREEITEM item=treeData->data.treeItem;
	ATLASSERT(item);
	//ATLASSERT(treeData->hasChild());
	//ATLASSERT(ItemHasChildren(item));

	//delete all child item
	HTREEITEM hNextItem;
	HTREEITEM hChildItem = GetChildItem(item);
	while (hChildItem != NULL)
	{
		hNextItem = GetNextItem(hChildItem, TVGN_NEXT);
		DeleteItem(hChildItem);
		hChildItem = hNextItem;
	}
	
	SetItemText(item,treeData->data.nodeName);

	//insert all child item
	TraverseSplitTreeAndShowInTreeView(treeData->child,item);
	
	Expand(item,TVE_EXPAND);
}




LRESULT CPropertyDlgMediaLib::OnBtnAdd(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	UINT uFlags=BIF_RETURNONLYFSDIRS|BIF_NEWDIALOGSTYLE;
	uFlags&=~BIF_NONEWFOLDERBUTTON ;
	CFolderDialog dlg(m_hWnd,_T("请选择要添加的文件夹"),uFlags);
	if (dlg.DoModal()==IDOK)
	{
		LPCTSTR path=dlg.GetFolderPath();

		int count=m_list.GetItemCount();
		m_list.InsertItem(count,path);
		m_list.SetItemText(count,1,_T("正在扫描"));

		std::tstring *strPath=new std::tstring(path);
		MyLib::shared()->GetAutoPlaylist()->AddFolderByThread(strPath->c_str());
		MyLib::shared()->AddMediaPath(*strPath);
		
		m_list.SetItemData(count,(DWORD)strPath);
	}

	return 0;
}

LRESULT CPropertyDlgMediaLib::OnBtnDel(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	int index=m_list.GetSelectedIndex();

	std::tstring *strPath((std::tstring*)m_list.GetItemData(index));

	MyLib::shared()->DelMediaPath(*strPath);

	m_list.DeleteItem(index);

	delete strPath;
	return 0;
}

BOOL CPropertyDlgNormal::OnInitDialog(CWindow wndFocus, LPARAM lInitParam)
{
	int nCheck=GetMyConfigs()->getResumeOnReboot();
	
	 ::SendMessage(GetDlgItem(IDC_CHECK_RESUME_ONBOOT), BM_SETCHECK, nCheck, 0);
	 
	return 0;
}

LRESULT CPropertyDlgNormal::OnButtonResumeOnBoot(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	int nCheck=(int)::SendMessage(GetDlgItem(IDC_CHECK_RESUME_ONBOOT), BM_GETCHECK, 0, 0L);

	GetMyConfigs()->setResumeOnReboot(nCheck);
	return 0;
}

BOOL CPropertyDlgLyricsLib::OnInitDialog(CWindow wndFocus, LPARAM lInitParam)
{
	DoDataExchange();

	//set style full row select
	list.SetExtendedListViewStyle(list.GetExtendedListViewStyle()|LVS_EX_FULLROWSELECT);

	list.InsertColumn(0,_T("路径"),LVCFMT_LEFT,220);


	vector<std::tstring>::iterator i;int index;
	for (i=MyLib::shared()->lrcDirs.begin(),index=0;i!=MyLib::shared()->lrcDirs.end();i++,index++)
	{
		std::tstring str=*i;
		int nItem=list.InsertItem(index,str.c_str());

		int len=_tcslen(str.c_str());
		LPTSTR pathCopy=new TCHAR[len+1];
		memset(pathCopy,0,(len+1)*sizeof(TCHAR));
		_tcsncpy(pathCopy,str.c_str(),len);
		list.SetItemData(nItem,(DWORD_PTR)pathCopy);
	}

	return TRUE;
}


LRESULT CPropertyDlgLyricsLib::OnBtnAdd(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	UINT uFlags=BIF_RETURNONLYFSDIRS|BIF_NEWDIALOGSTYLE;
	uFlags&=~BIF_NONEWFOLDERBUTTON ;
	CFolderDialog dlg(m_hWnd,_T("请选择要添加的文件夹"),uFlags);
	if (dlg.DoModal()==IDOK)
	{
		LPCTSTR path=dlg.GetFolderPath();
		int index=list.GetItemCount();
		index=list.InsertItem(index,path);

		int len=_tcslen(path);
		LPTSTR pathCopy=new TCHAR[len+1];
		memset(pathCopy,0,(len+1)*sizeof(TCHAR));
		_tcsncpy(pathCopy,path,len);
		list.SetItemData(index,(DWORD_PTR)pathCopy);
	}

	return 0;
}

LRESULT CPropertyDlgLyricsLib::OnBtnDel(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	int i=list.GetSelectedIndex();
	LPCTSTR path=(LPCTSTR)list.GetItemData(i);
	list.DeleteItem(i);
	delete[] path;
	return 0;
}


LRESULT CPropertyDlgLyricsLib::OnCfgToSave(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{	
	MyLib::shared()->ClearLrcSearchLib();

	int count=list.GetItemCount();
	for (int i=0;i<count;i++)
	{
		LPCTSTR path=(LPCTSTR)list.GetItemData(i);
		MyLib::shared()->AddFolder2LrcSearchLib(path);
	}

	MyLib::shared()->InitLrcLib();
	return 0;
}


BOOL CPropertyDlgUILayout::OnInitDialog(CWindow wndFocus, LPARAM lInitParam)
{
	tree.SubclassWindow(GetDlgItem(IDC_TREE));

	MyRoot=GetSplitter()->rootTree;
	HTREEITEM rootItem= tree.InsertItem(MyRoot->data.nodeName,TVI_ROOT,TVI_LAST);
	MyRoot->data.treeItem=rootItem;
	tree.SetItemData(rootItem,(DWORD)MyRoot);

	UpdateTree(MyRoot);

	return 0;
}




CMultiSpliltWnd* GetSplitter()
{
	return GetMainFrame()->m_pWndMultiSplitter;
}




void MYTREE_Add_EmptyWnd(MYTREE* tree)
{
	MYTREE *newTree=new MYTREE();

	CPlaceHolderWnd *w=new CPlaceHolderWnd;

	w->Create(GetSplitter()->m_hWnd,0,0, WS_CHILD | WS_VISIBLE |WS_CLIPCHILDREN |WS_CLIPSIBLINGS,WS_EX_CLIENTEDGE);
	

	newTree->setWnd(w->m_hWnd);
	wcscpy(newTree->data.nodeName,L"空占位视图");


	tree->addChildNodeAfter(newTree);
	tree->EvenPanes();

}

void  CPlayListView::CreateIsWnd()
{
// 	Create(GetSplitter()->m_hWnd, 0 , _T("WtlSysListView32"), 
// 		WS_CHILD | WS_VISIBLE  |WS_CLIPCHILDREN |WS_CLIPSIBLINGS 
// 		|LVS_REPORT | LVS_SHOWSELALWAYS |LVS_OWNERDATA | LVS_SORTASCENDING /*| LVS_OWNERDRAWFIXED |*/
// 		,0/*WS_EX_CLIENTEDGE*/);
		
	RegisterClass();
	CPlaceHolderWnd *w=new CPlaceHolderWnd;

	w->Create(GetSplitter()->m_hWnd,NULL,NULL,WS_CHILD|WS_VISIBLE);

//  	Create(GetSplitter()->m_hWnd, 0 , _T("WtlListView32"),
//  		WS_CHILD | WS_VISIBLE  |WS_CLIPCHILDREN |WS_CLIPSIBLINGS ,0);
	SubclassWindow(w->m_hWnd);
}

void MYTREE_Set_Playlist(MYTREE* tree)
{
	CPlayListView *plv=new CPlayListView;

	plv->CreateIsWnd();
	
	plv->Init();

	tree->setWnd(plv->m_hWnd);
}


void MYTREE_Add_Playlist(MYTREE* tree)
{
	MYTREE *newTree=new MYTREE();
	MYTREE_Set_Playlist(newTree);

	wcscpy(newTree->data.nodeName,L"播放列表");

	tree->addChildNodeAfter(newTree);
	tree->EvenPanes();
	PaneSizeStore(newTree->parent);
	UpdateLayout(newTree->parent);
}






HWND CAlbumCoverView::CreateMyWnd()
{
	return Create(GetSplitter()->m_hWnd,0,0, WS_CHILD | WS_VISIBLE ,WS_EX_CLIENTEDGE);
}

void MYTREE_Set_AlbumView(MYTREE* tree)
{
	CAlbumCoverView *w=new CAlbumCoverView;
	tree->setWnd(w->CreateMyWnd());
	w->Init();
}

void MYTREE_Add_AlbumView(MYTREE* tree)
{	
	MYTREE *newTree=new MYTREE();
	MYTREE_Set_AlbumView(newTree);
	
	wcscpy(newTree->data.nodeName,L"封面面板");

	tree->addChildNodeAfter(newTree);
	tree->EvenPanes();
	PaneSizeStore(newTree->parent);
	UpdateLayout(newTree->parent);
}
void MYTREE_Set_LyricView(MYTREE* tree)
{
	CWndLyric *w=new CWndLyric;
	w->Create(GetSplitter()->m_hWnd,0,0, WS_CHILD | WS_VISIBLE ,WS_EX_CLIENTEDGE);
	tree->setWnd(w->m_hWnd);
}

void MYTREE_Add_LyricView(MYTREE* tree)
{
	MYTREE *newTree=new MYTREE();
	MYTREE_Set_LyricView(newTree);
	wcscpy(newTree->data.nodeName,L"歌词面板");

	tree->addChildNodeAfter(newTree);
	tree->EvenPanes();
	PaneSizeStore(newTree->parent);
	UpdateLayout(newTree->parent);
}



void MYTREE_Set_SpectrumView(MYTREE* tree)
{
	DialogFFT *w=new DialogFFT;
	w->Create(GetSplitter()->m_hWnd,0,0, WS_CHILD | WS_VISIBLE ,WS_EX_CLIENTEDGE);
	tree->setWnd(w->m_hWnd);
}
void MYTREE_Add_SpectrumView(MYTREE* tree)
{
	MYTREE *newTree=new MYTREE();
	MYTREE_Set_SpectrumView(newTree);
	wcscpy(newTree->data.nodeName,L"频谱面板");
	tree->addChildNodeAfter(newTree);
	tree->EvenPanes();
}

void CMyStatusBar::UpdateTrackInfoText()
{
	if(!MyLib::shared()->isPlaying())
		return;

	int totalSec=getTrackPosInfo()->used+getTrackPosInfo()->left;
	int minute=totalSec / 60;
	int sec=totalSec - minute*60 ;

	int usedtotalSec=getTrackPosInfo()->used;
	int usedMinute=usedtotalSec/60;
	int usedSec=usedtotalSec - usedMinute*60;

	TrackFormatInfo *info=GetTrackFormatInfo();
	TCHAR format[256]={0};
	_stprintf(format,_T("MP3 | %d kbps | %d Hz | %s | %d:%02d / %d:%02d"),info->rate,info->nSamplesPerSec ,info->getModeString(),usedMinute,usedSec,minute,sec);


	SetText(0,format);
}

CMainFrame::~CMainFrame()
{

}


void CMainFrame::OnFinalMessage(_In_ HWND /*hWnd*/)
{


}


LRESULT CMainFrame::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	// unregister message filtering and idle updates
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->RemoveMessageFilter(this);
	pLoop->RemoveIdleHandler(this);


	KillTimer(m_nIDEvent);


	//save the windows position
	GetWindowRect(&m_rcMain);
	
	if(m_pDlgConfig)
	{	
		m_rcConfig=m_pDlgConfig->m_rc;
		m_dlgConfigShow=m_pDlgConfig->m_show;
		RECT m_dlgLrcShow;
		RECT m_dlgProcessShow;
		RECT ;
		RECT m_dlgFFTShow;
		RECT m_dlgPLMngShow;
		RECT m_dlgPLConsoleShow;
	}
// 	if(m_dlgLrc)
// 		m_dlgLrc->GetWindowRect(&m_rcLrc);
// 	if(pDlgProcess)
// 		pDlgProcess->GetWindowRect(&m_rcProcess);
	if(m_pDlgSearch)
	{
		m_rcSearch=m_pDlgSearch->m_rc;
		//m_dlgSearchShow=m_pDlgSearch->m_show;
	}
// 	if(m_pDlgFFT)
// 		m_pDlgFFT->GetWindowRect(&m_rcFFT);
	if(m_pDlgPLMng)
		m_rcPLMng=m_pDlgPLMng->m_rc;

// 	if(m_pDlgConsole)
// 		m_pDlgConsole->GetWindowRect(&m_rcPLConsole);


	SaveAll();


	UnregisterHotKey(m_hWnd, HotKeyId); 

	GlobalDeleteAtom(HotKeyId);



	bHandled = FALSE;
	return 1;
}

static map<TCHAR*,CreateWindowFun> createWndFuns;
void RegisterCreateWndFuns(TCHAR* panename,CreateWindowFun func)
{
	createWndFuns.insert(map<TCHAR*,CreateWindowFun>::value_type(panename,func));

	CreateWindowFun fun =createWndFuns.find(panename)->second;
}

void CreateHWNDbyName(MYTREE *tree)
{
	for (;tree;tree=tree->next)
	{	
		for (auto i=createWndFuns.begin();i!=createWndFuns.end();++i)
		{
			if (_tcscmp(i->first,tree->data.nodeName)==0)
			{
				CreateWindowFun fun=i->second;
				fun(tree);
				break;
			}
		}
	
		if (tree->hasChild())
			CreateHWNDbyName(tree->child);
	}

}





void CPlayListView::OnDestroy()
{
	IDonotWantToReceiveMessage(WM_PLAYLISTVIEW_SETFOCUS);
	IDonotWantToReceiveMessage(WM_PLAYLISTVIEW_COLOR_DEFAULT);
	IDonotWantToReceiveMessage(WM_PLAYLISTVIEW_COLOR_BLUE);

	SetMsgHandled(FALSE);
}



void CPlayListView::Init(bool bSearch)
{	
	IWantToReceiveMessage(WM_PLAYLISTVIEW_SETFOCUS);
	IWantToReceiveMessage(WM_PLAYLISTVIEW_COLOR_DEFAULT);
	IWantToReceiveMessage(WM_PLAYLISTVIEW_COLOR_BLUE);


	SetLVFont(20);


	ChangeColorDefault();


	if ( !m_ilItemImages.CreateFromImage( IDB_EXAMPLE, 16, 0, RGB( 255, 0, 255 ), IMAGE_BITMAP, LR_CREATEDIBSECTION ) )
		return ;

	SetImageList( m_ilItemImages );

	
	const TCHAR * columnName[]={
		_T("索引"),
		_T("标题"),
		_T("艺术家"),
		_T("专辑"),
		_T("年份"),
		_T("流派")
	};
	const UINT alignment[]={
		ITEM_FLAGS_CENTRE,
		ITEM_FLAGS_LEFT,
		ITEM_FLAGS_LEFT,
		ITEM_FLAGS_LEFT,
		ITEM_FLAGS_RIGHT,
		ITEM_FLAGS_RIGHT
	};
	const TCHAR* columnNamePlaceHoder[]={
		_T("Index"),
		_T("Title                        "),
		_T("艺术家艺"),
		_T("Album         "),
		_T("年份"),
		_T("Genre               ")
	};

	
	const int cxOffset = 40;
	for (int i=0;i<sizeof(columnName)/sizeof(int);++i)
		AddColumn(columnName[i],
		GetTextWidth(columnNamePlaceHoder[i])+cxOffset,
		-1,0,NULL_COLUMN,alignment[i]);
		
	
	
	m_ilListItems.Detach();
	m_curDivider.Detach();
	m_curHyperLink.Detach();
	m_ttToolTip.Detach();

	CListImpl< CPlayListView >::Initialise();

	if(!bSearch)
		AllPlayListViews()->AddItem(this);
}



void CollectInfo()
{
	MyConfigs *c=GetMyConfigs();
	c->setPlayersVolume(CBasicPlayer::shared()->m_curVolume);


	if(c->getResumeOnReboot())
	{
		PlayList *pl=MyLib::shared()->GetPlayingPL();
		if(pl)
		{
			PlayListItem *track=pl->GetPlayingItem();
			if(track)
			{		
				c->playlistIndex=MyLib::shared()->Playlist2Index(track->GetPlayList());
				c->trackIndex=track->GetIndex();
			}
		}
		CBasicPlayer::shared()->GetPos(&(c->pos),0);
	}

	c->setPlayersVolume(CBasicPlayer::shared()->m_curVolume);
	c->playorder=MyLib::shared()->playorder;
}

void ValidateCfg()
{
	MyConfigs *c=GetMyConfigs();
	CBasicPlayer::shared()->SetVolumeByEar(c->getPlayersVolume());
	MyLib::shared()->SetPlayOrder(static_cast<MyLib::PlayOrder>(c->playorder));

	
}



LPDIRECTSOUND DSoundDeviceCreate(LPGUID lpGuid  )
{
	LPDIRECTSOUND lpDSound;
	if(FAILED(DirectSoundCreate(lpGuid,&lpDSound,NULL) ))return NULL;

	//可以设置主缓冲的播放模式的级别
	// 	HWND hWnd=GetForegroundWindow();
	// 	if (!hWnd)
	// 		hWnd=GetDesktopWindow();

	if(FAILED(lpDSound->SetCooperativeLevel( GetMainFrame()->m_hWnd, DSSCL_NORMAL) ))return NULL;
	return lpDSound;
}