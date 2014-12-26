#include "MainFrm.h"
#include <string>
#include "mytree.h"
#include "MyControls.h"
#include "PlayList.h"
#include "BasicPlayer.h"
#include "globalStuffs.h"


#include "DialogConfig.h"
#include "CAlbumCoverView.h"
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
#include "WuLines.h"

#include "funcCmd.h"
#include "shortcutKey.h"

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


	//MOD_ALT MOD_CONTROL MOD_SHIFT MOD_WIN
	bool ctrl = fuModifiers & MOD_ALT;


	shortcutKeyPressed( msgKeytoString( ctrl , nChar), true);

	//Next Tract
	if( nChar == VK_MEDIA_NEXT_TRACK )
		OnPlayNext(0,0,0,bHandled);

	//Prev track
	//if(nChar == VK_MEDIA_PREV_TRACK)

	//Stop
	if( nChar == VK_MEDIA_STOP )
		OnStop(0,0,0,bHandled);

	//Play or Pause
	if( nChar == VK_MEDIA_PLAY_PAUSE )
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

		/// if nChar is upper , change to lower.
		if( isupper(nChar))
			nChar -= 'A' - 'a';

		BOOL ctrl = GetKeyState(VK_CONTROL) & 0x80 ;

		/*
		BOOL lwin = GetKeyState(VK_LWIN) & 0x80 ;
		BOOL rwin = GetKeyState(VK_RWIN) & 0x80 ;
		BOOL win  = lwin || rwin;
		*/

		
		shortcutKeyPressed( msgKeytoString( ctrl , nChar), false);

		/*
		//F3 
		if(nChar==VK_F3 )
			ShowSearchDialog();

		if( ctrl )
		{
			switch(nChar)
			{ 
				case 'f': //Ctrl + F 
					ShowSearchDialog();
					break;
				case 'p': //Ctrl + P
					ShowDialog_Configure();
					break;
				case VK_OEM_PLUS: /// Ctrl + '+' or '-'
					EnlargeLVFont();
					break;
				case VK_OEM_MINUS:
					ReduceLVFont();
					break;
				case 's':	//save config
					SaveAll();
					break;
				case 'm':	//popup the playlist manager
					actionPopupPlaylistManager();
				break;
			}
		}
		*/
	}

	return CFrameWindowImpl<CMainFrame>::PreTranslateMessage(pMsg);
}


BOOL CMainFrame::OnIdle()
{
	UIUpdateToolBar();
	BOOL flag=MyLib::shared()->GetSelectedPL()!=NULL;
	UIEnable(ID_FILE_OPENDIRECTORY,flag);
	UIEnable(ID_FILE_OPEN,flag);
	UIEnable(ID_FILE_SAVEPLAYLIST,flag);
	return FALSE;
}

void CMainFrame::registCmdFuntions()
{
	registMsgForCommandstring(string("File.ShowDialog_Configure"),WM_SHOW_DIALOG_CONFIGURE);
	registMsgForCommandstring(string("File.SaveConfigure"),WM_SAVE_CONFIGURE);
	registMsgForCommandstring(string("View.ShowDialog_Search"),WM_SHOW_DIALOG_SEARCH);
	registMsgForCommandstring(string("View.ShowDialog_PlaylistManager"),WM_SHOW_DIALOG_PLAYLIST_MANAGER);
	registMsgForCommandstring(string("View.EnlargeListViewFont"),WM_CHANGE_LISTVIEW_FONT_ENLARGE);
	registMsgForCommandstring(string("View.ReduceListViewFont"),WM_CHANGE_LISTVIEW_FONT_REDUCE);
}

LRESULT CMainFrame::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
{
	ValidateCfg();

	registCmdFuntions();

	ReceiveMsg();

	SetTimer((UINT_PTR)&m_nIDEvent,m_uElapse,NULL);

	SetMenu(NULL);

	m_hWndToolBar=m_wndRebar.CreateIsWnd(m_hWnd);
	m_wndRebar.Init();

	CreateSimpleStatusBar();
	m_pStatus=new CMyStatusBar;
	m_pStatus->SubclassWindow(m_hWndStatusBar);
	m_pStatus->Init();

	UIAddStatusBar(m_hWndStatusBar);
	ShowStatusBar(m_bShowStatusBar);


	InitUILayout();


	if (m_DlgSearch.m_bHidenLast == FALSE)
	{
		ShowSearchDialog();
	}


	// register object for message filtering and idle updates
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->AddMessageFilter(this);
	pLoop->AddIdleHandler(this);

	RegisterMyHotKeys();

	SetMsgHandled(FALSE);

	return 0;
}

void CMainFrame::ReceiveMsg()
{
	IWantToReceiveMessage(WM_FILE_FINDED);
	IWantToReceiveMessage(WM_TRACK_REACH_END);
	IWantToReceiveMessage(WM_ADDFOLDERED);
	IWantToReceiveMessage(WM_NEW_TRACK_STARTED);
	IWantToReceiveMessage(WM_TRACKSTOPPED);
	IWantToReceiveMessage(WM_PL_TRACKNUM_CHANGED);


	IWantToReceiveMessage(WM_SHOW_DIALOG_CONFIGURE );
	IWantToReceiveMessage(WM_SAVE_CONFIGURE );
	IWantToReceiveMessage(WM_SHOW_DIALOG_SEARCH );
	IWantToReceiveMessage(WM_SHOW_DIALOG_PLAYLIST_MANAGER );
	IWantToReceiveMessage(WM_CHANGE_LISTVIEW_FONT_ENLARGE );
	IWantToReceiveMessage(WM_CHANGE_LISTVIEW_FONT_REDUCE );

}

HWND CMySimpleRebar::CreateIsWnd(HWND hParent)
{
	HWND hWnd= CMainFrame::CreateSimpleReBarCtrl(hParent, ATL_SIMPLE_REBAR_NOBORDER_STYLE, ATL_IDW_TOOLBAR);
	SubclassWindow(hWnd);
	return hWnd;
}

HWND CMyTrackBar::CreateIsWnd(HWND parent)
{
	HWND hWnd=baseclass::CreateIsWnd(parent);

	m_CtrlTooltip.Create(hWnd);
	m_CtrlTooltip.AddTool(hWnd, LPSTR_TEXTCALLBACK); 

	IWantToReceiveMessage(WM_TRACK_POS_CHANGED);
	IWantToReceiveMessage(WM_NEW_TRACK_STARTED);
	IWantToReceiveMessage(WM_PAUSED);
	IWantToReceiveMessage(WM_PAUSE_START);
	IWantToReceiveMessage(WM_TRACKSTOPPED);


	CBasicPlayer *s=CBasicPlayer::shared();
	if(s->stoped())
		EnableWindow(FALSE);
	else 
	{
		::PostMessage(m_hWnd,WM_NEW_TRACK_STARTED,NULL,NULL);
		if(s->paused())
			::PostMessage(m_hWnd,WM_PAUSED,NULL,NULL);
	}

	return hWnd;
}

void CMyVolumeBar::Init()
{
	SetPageSize(1);
	SetLineSize(1);
	SetThumbLength(2);
	SetRange(0,100);
	SetPos(CBasicPlayer::shared()->m_curVolume);
}


HWND CMyToolBar::CreateIsWnd(HWND parent)
{
	HWND hWnd=CMainFrame::CreateSimpleToolBarCtrl(::GetParent(parent),IDR_MAINFRAME, FALSE, ATL_SIMPLE_TOOLBAR_PANE_STYLE);
	SubclassWindow(hWnd);
	return hWnd;
}




void CMainFrame::InitUILayout()
{
	const DWORD dwSplitStyle = WS_CHILD|WS_VISIBLE | WS_CLIPCHILDREN |WS_CLIPSIBLINGS  ;
	const DWORD dwSplitExStyle =WS_EX_CLIENTEDGE;

	m_hWndClient=m_WndMultiSplitter.Create(m_hWnd,rcDefault,NULL,dwSplitStyle,0);
}



HWND CMyComboBox::CreateIsWnd(HWND parent)
{
	Create(parent,NULL,NULL,WS_CHILD|WS_VISIBLE|WS_HSCROLL | WS_VSCROLL|CBS_DROPDOWNLIST);


	for (int i=0;i<sizeof(gPlayOrderStr)/sizeof(gPlayOrderStr[0]);++i)
		AddString(gPlayOrderStr[i]);

	SetCurSel(MyLib::shared()->GetPlayOrder());

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

	
	CBasicPlayer::shared()->SetPos(pos);


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
	MyLib::shared()->stop();
	MyLib::shared()->play();
	
	return 0;
}

void CPlayListView::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (nChar==VK_RETURN)
	{
		MyLib::shared()->play();
	}
}

void CPlayListViewS::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (nChar==VK_RETURN)
	{
		LPCPlayListItem item= GetPlayList()->GetItem(0);
		if(item)
		{
			MyLib::shared()->SetItemToPlay(GetItemInParent(item));
			MyLib::shared()->play();
		}
	}
}

LRESULT CMainFrame::OnOpen(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	const TCHAR szFilter[]=_T("mp3 files(*.mp3)\0*.mp3\0wave files(*.wav)\0*.wav\0");
	CFileDialog dlg(TRUE,NULL,NULL,OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT,szFilter,m_hWnd);
	dlg.m_ofn.Flags|=OFN_ALLOWMULTISELECT;

	if(dlg.DoModal()==IDOK)
	{
		LPCPlayList selPl=MyLib::shared()->GetSelectedPL();
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
	MyLib::shared()->pause();
	ChangeTitle();
	return 0;
}

LRESULT CMainFrame::OnStop(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	MyLib::shared()->stop();
	return 0;
}

LRESULT CMainFrame::OnPlayNext(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	MyLib::shared()->stop();

	if(MyLib::shared()->GetPlayOrder() == Repeat_track)
		MyLib::shared()->playNext(TRUE,Default,FALSE);
	else
		MyLib::shared()->playNext(TRUE);

	return 0;
}


LRESULT CMainFrame::OnPlayRandom(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	MyLib::shared()->stop();
	MyLib::shared()->playRandomNext();
	return 0;
}

void CMainFrame::ShowDialog_Configure()
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
}

LRESULT CMainFrame::OnConfig(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{	
	ShowDialog_Configure();
	return 0;
}


LRESULT CMainFrame::OnSaveConfig(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	SaveAll();

	return 0;
}

void CMainFrame::showDialogConfigure()
{
	if (!m_pDlgConfig)
	{
		m_pDlgConfig=new CDialogConfig;
		m_pDlgConfig->Create(m_hWnd);
	}

	m_pDlgConfig->ShowWindow(SW_SHOW);
	m_pDlgConfig->ShowDlgByName(std::wstring(L"界面布局"));
}

LRESULT CMainFrame::OnConfigLayout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	showDialogConfigure();	
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

void CMainFrame::OnChangeLVFont(int offset)
{
	CPlayListView::EnlargeLVFont(offset);

	AllPlayListViews()->updateListFont();
	
	if (m_DlgSearch.IsWindow())
		m_DlgSearch.m_list.updateListFont();
}

LRESULT CMainFrame::OnChangeLVFont(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	OnChangeLVFont(wID == ID_LV_ENLARGE_FONT ? 1 : -1);

	return 0;
}

LRESULT CMainFrame::OnRemoveDeadItems(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	LPCPlayList pl=MyLib::shared()->GetSelectedPL();
	if(pl)
		pl->RemoveDeadItems();

	return 0;
}

LRESULT CMainFrame::OnRemoveDuplicates(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	LPCPlayList pl=MyLib::shared()->GetSelectedPL();
	if(pl)
		pl->RemoveDuplicates();

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
	if(m_pDlgLrc==NULL)
	{
		m_pDlgLrc=new CDialogLyric;
		m_pDlgLrc->Create(m_hWnd);
	}

	m_pDlgLrc->Show();

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



LRESULT CMainFrame::OnNewTrackStarted(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	ChangeTitle();
	LPCPlayListItem item=MyLib::shared()->GetPlayingItem();

	AllPlayListViews()->Reload(item->GetPlayList(),item->GetIndex());

	//refresh the active playlist item. 
	if(m_pDlgPLMng && m_pDlgPLMng->IsWindow())
		m_pDlgPLMng->m_list.Invalidate();
	
	return 0;
}


LRESULT CMyComboBox::OnCbnSelchanged(UINT,int id, HWND hWndCtl)
{
	MyLib::shared()->SetPlayOrder( GetCurSel() );
	

	SetMsgHandled(FALSE);
	return 0;
}




LRESULT CMainFrame::OnTrackReachEnd(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	MyLib::shared()->playNext();

	return 0;
}

LRESULT CMainFrame::OnTrackStopped(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	ChangeTitle();

	return 0;
}
// 
// LRESULT CMainFrame::OnPLChanged(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
// {
// 	BOOL bAdd=(BOOL)lParam;
// 	LPCPlayList pl=(LPCPlayList)wParam;
// 
// 	if (m_pDlgPLMng && m_pDlgPLMng->IsWindow())
// 	{
// 		if (bAdd)
// 			m_pDlgPLMng->m_list.AddPlayList(pl);
// 		else
// 			m_pDlgPLMng->m_list.DelPlayList(pl);
// 	}
// 
// 	if (bAdd)
// 		AllPlayListViews()->Reload(pl);
// 	else
// 	{
// 		LPCPlayList anotherSel=NULL;
// 		if(!MyLib::shared()->Empty())
// 			anotherSel= MyLib::shared()->GetItem(0);
// 
// 		AllPlayListViews()->Reload(anotherSel);
// 	}
// 
// 	return 0;
// }





LRESULT CMainFrame::OnFileNewPlaylist(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	LPCPlayList pPl=MyLib::shared()->NewPlaylist(TRUE);
	
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
		LPCPlayList playlist =MyLib::shared()->LoadPlaylist(dlg.m_ofn.lpstrFile);
		::PostMessage(m_hWnd,WM_ADDFOLDERED,NULL,NULL);
	}

	return 0;
}


LRESULT CMainFrame::OnSavePlaylist(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	LPCPlayList pl=MyLib::shared()->GetSelectedPL();
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


LRESULT CMainFrame::OnPLVCenterItem(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	LPCPlayListItem item=MyLib::shared()->GetPlayingItem();
	if(item)
	{
		AllPlayListViews()->Reload(item->GetPlayList(),item->GetIndex());
		NotifyMsg(WM_PLAYLISTVIEW_SETFOCUS,FALSE,0,0);
	}

	return TRUE;
}

LRESULT CMainFrame::OnWndActive(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& bHandled)
{
	static HWND hWndOldFocused=NULL;

	//save the window current focused
	UINT t= LOWORD(wParam);
	if(t==WA_INACTIVE)
	{
		hWndOldFocused=GetFocus();
	}
	else
	{
		if(hWndOldFocused!=NULL)
		{
			::SetFocus(hWndOldFocused);
		}
	}

	return 0;
}

void CMyStatusBar::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	::PostMessage(GetMainFrame()->m_hWnd,WM_PLAYLISTVIEW_CENTER_ITEM,NULL,NULL);
}



LRESULT CPlayListView::OnDbClicked(int /**/,LPNMHDR pnmh,BOOL bHandled)
{	
	if(!GetPlayList())return 0;

	int nItem=GetFirrstSelectedItem();

	GetPlayList()->SetSelectedIndex(nItem);

	if(m_bManual)
		MyLib::shared()->SetItemToPlay(GetPlayList()->GetItem(nItem));

	int k=GetPlayList()->GetSelectedIndex();
	if(k!=-1)
	{
		MyLib::shared()->play();
	}

	SetMsgHandled(FALSE);
	return 0;
} 


LRESULT CPlayListViewS::OnDbClicked(int /**/,LPNMHDR pnmh,BOOL bHandled)
{	
	if(!GetPlayList())return 0;

	if (HasSeleted())
	{
		int k = playlistParent->GetSelectedIndex();
		if (k != -1)
		{
			MyLib::shared()->play();
		}
	}

	SetMsgHandled(TRUE);
	return 0;
} 


LRESULT CMainFrame::OnExit(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	bHandled=FALSE;
	return 0;
}

void CPlayList::TerminateAddDirThread()
{
	::TerminateThread(hAddDir,-1);
	NotifyMsg(WM_FILE_FINDED,FALSE,NULL,(LPARAM)0);
	::PostMessage(GetMainFrame()->m_hWnd,WM_ADDFOLDERED,NULL,NULL);
}

void CMainFrame::ChangeTitle()
{						 
	LPCPlayListItem track = MyLib::shared()->GetPlayingItem();
	if(track==NULL || !track->isValide())return;

	std::tstring title;
	if(CBasicPlayer::shared()->m_bStopped)
	{
		title+=GetAppName();
	}
	else if (CBasicPlayer::shared()->m_bPaused)
	{
		title+=track->GetTitle();
		title+=_T(" (Paused)");	
	}
	else 
	{
		title+=track->GetTitle();
	}

	SetWindowText(title.c_str());
}


void CMainFrame::ShowSearchDialog()
{
	if (!::IsWindow(m_DlgSearch.m_hWnd) )
	{
		if(!m_DlgSearch.Create(m_hWnd))
		{
			return;
		}
	}
	
	m_DlgSearch.ShowSelf();
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
	NotifyMsg(WM_PLAYLISTVIEW_COLOR_DEFAULT,FALSE,0,0);
	return 0;
}

LRESULT CMainFrame::OnChangePLColorBlue(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	NotifyMsg(WM_PLAYLISTVIEW_COLOR_BLUE,FALSE,0,0);
	return 0;
}

void CMainFrame::showDialogPlaylistManager()
{
	if(!m_pDlgPLMng){
	m_pDlgPLMng=new DialogPLManager;
	m_pDlgPLMng->Create(m_hWnd);

	if(m_rcPLMng.right>0 && m_rcPLMng.bottom > 0)
		::SetWindowPos(m_pDlgPLMng->m_hWnd, NULL, m_rcPLMng.left, m_rcPLMng.top, m_rcPLMng.right - m_rcPLMng.left, m_rcPLMng.bottom - m_rcPLMng.top,SWP_NOZORDER);
	}

	m_pDlgPLMng->ShowSelf();	
}


LRESULT CMainFrame::OnViewPlaylistManager(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	showDialogPlaylistManager();

	return 0;
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





	LRESULT  CPlayListManager::OnDbClicked(UINT i,CPoint pt)
	{
		int k=GetFirstSelItem();
		if(k!=-1)
		{
			LPCPlayList l=(LPCPlayList)GetItemData(k);

			MyLib::shared()->SetSelectedPL(l);
		}

		Invalidate();
		//RedrawItems(0,-1);

		SetMsgHandled(FALSE);
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
		else
			return;
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
			NotifyMsg(WM_LYRIC_RELOAD,FALSE,0,0);

		}
	}


	savePath=NULL;

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


BOOL CPlayListViewS::IsAllSelectedItemInPlayQueue()
{
	MyLib *s=MyLib::shared();
	vector<int> v;
	CListArray < int >  aSelectedItems;
	if(GetSelectedItems(aSelectedItems))
		for (int i=0,c=aSelectedItems.GetSize();i<c;i++)
		{
			v=s->GetIndexInPlayQueue(GetItemInParent(GetPlayList()->GetItem(aSelectedItems[i])));
			if(v.empty())
				return FALSE;
		}

		return TRUE;
}

LRESULT CPlayListViewS::OnPushToPlayqueue(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	MyLib *s=MyLib::shared();
	CListArray < int >  aSelectedItems;
	if(GetSelectedItems(aSelectedItems))
		for (int i=0;i<aSelectedItems.GetSize();i++)
		{
			LPCPlayListItem item=GetPlayList()->GetItem(aSelectedItems[i]);
			LPCPlayListItem itemInParent=GetItemInParent(item);
			s->PushPlayQueue(itemInParent);
		}
		return 0;
}



LRESULT CPlayListViewS::OnDeleteFromPlayqueue(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{

	MyLib *s=MyLib::shared();
	CListArray < int >  aSelectedItems;
	if(GetSelectedItems(aSelectedItems))
		for (int i=0;i<aSelectedItems.GetSize();i++)
			s->DeleteFromPlayQueue(GetItemInParent(GetPlayList()->GetItem(aSelectedItems[i])));

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



void PlayListViewMng::Reload(LPCPlayList pPL,int itemActive)
{
	for (container::iterator i=dataCollection.begin();
		i!=dataCollection.end();++i)
	{
		(*i)->Reload(pPL,itemActive);

		//int centerItem=(*i)->GetPlayList()->selectedIndex;

		//if(centerItem==INVALID_ITEM)
			//centerItem=(*i)->GetPlayList()->selectedIndex;

		//todo
		//(*i)->ClearAllSel();
		//(*i)->EnsureVisibleAndCentrePos(index);
		//(*i)->SelectAndFocusItem(index);
	}

	MyLib::shared()->SetSelectedPL(pPL);
}




LRESULT CPlayListView::OnSelected(int /**/,LPNMHDR pnmh,BOOL bHandled)
{	
// 	NMLISTVIEW * pnml=(NMLISTVIEW *)pnmh;
// 	int nItem=pnml->iItem;
	
	int nItem=GetFirrstSelectedItem();

	GetPlayList()->SetSelectedIndex(nItem);

	if(m_bManual)
		MyLib::shared()->SetItemToPlay(GetPlayList()->GetItem(nItem));

	return 0;
}


LRESULT CPlayListViewS::OnSelected(int /**/,LPNMHDR pnmh,BOOL bHandled)
{	
	vector<int> items;

	CListArray < int > aSelectedItems;
	GetSelectedItems(aSelectedItems);

	int size = aSelectedItems.GetSize();
	for (int count=0;count< size ;count++)
	{
		int nItem=aSelectedItems[count];
		LPCPlayListItem itemInParent=GetItemInParent(GetPlayList()->GetItem(nItem));
		if(count==0)
		{
			if(m_bManual)
				MyLib::shared()->SetItemToPlay(itemInParent);
			playlistParent->SetSelectedIndex(itemInParent->GetIndex());
		}

		items.push_back(itemInParent->GetIndex());
	}

	if(aSelectedItems.GetSize()>0)
		AllPlayListViews()->SelectItems(items);
	


// 	NMLISTVIEW * pnml=(NMLISTVIEW *)pnmh;
// 	int nItem=pnml->iItem;
// 
// 	PlayListItem *itemInSearch=GetPlayList()->GetItem(nItem);
// 
// 	PlayListItem *itemInParent=GetItemInParent(itemInSearch);
// 	
// 	ATLASSERT(itemInParent);
//	
// 	playlistParent->SetTopVisibleIndex(itemInParent->GetIndex());
// 	playlistParent->SetSelectedIndex(itemInParent->GetIndex());
	bHandled=FALSE;
	return 0;
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

void CPropertyDlgMediaLib::InitList()
{
	m_list.DeleteAllItems();

	MyLib * s=MyLib::shared();
	for (auto i=s->GetMediaPaths().begin();i!=s->GetMediaPaths().end();++i)
	{
		std::tstring path=*i;
		int count=m_list.GetItemCount();
		m_list.InsertItem(count,path.c_str());
		m_list.SetItemText(count,1,_T("正在监视"));
		std::tstring *strPath=new std::tstring(path);
		m_list.SetItemData(count,(DWORD)strPath);
	}
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
	for (i=MyLib::shared()->GetLrcDirs().begin(),index=0;i!=MyLib::shared()->GetLrcDirs().end();i++,index++)
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

	MyRoot=GetSplitter()->GetRootTree();
	HTREEITEM rootItem= tree.InsertItem(MyRoot->data.nodeName,TVI_ROOT,TVI_LAST);
	MyRoot->data.treeItem=rootItem;
	tree.SetItemData(rootItem,(DWORD)MyRoot);

	UpdateTree(MyRoot);

	return 0;
}




CMultiSpliltWnd* GetSplitter()
{
	return & GetMainFrame()->m_WndMultiSplitter;
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
		
	
	CPlaceHolderWnd *w=new CPlaceHolderWnd;

	w->Create(GetSplitter()->m_hWnd,NULL,NULL,WS_CHILD|WS_VISIBLE);

//  	Create(GetSplitter()->m_hWnd, 0 , _T("WtlListView32"),
//  		WS_CHILD | WS_VISIBLE  |WS_CLIPCHILDREN |WS_CLIPSIBLINGS ,0);
	SubclassWindow(w->m_hWnd);
}

void MYTREE_Set_Playlist(MYTREE* tree)
{
	CPlayListView *plv=new CPlayListView;

	plv->SetSerializeData(& tree->data.wndData);

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

	//plv->SetSerializeData(tree->data.pWndData);
}

void MYTREE_Add_AlbumView(MYTREE* tree)
{	
	MYTREE *newTree=new MYTREE();

	MYTREE_Set_AlbumView(newTree);
	
	wcscpy(newTree->data.nodeName,L"封面面板");

	tree->addChildNodeAfter(newTree);

	tree->EvenPanes();
	
	UpdateLayout(newTree->parent);
}

void MYTREE_Set_LyricView(MYTREE* tree)
{
	CMyLyricWnd *w=new CMyLyricWnd;
	w->Create(GetSplitter()->m_hWnd,0,0, WS_CHILD | WS_VISIBLE ,WS_EX_CLIENTEDGE);
	tree->setWnd(w->m_hWnd);
	
	//plv->SetSerializeData(tree->data.pWndData);
}

void MYTREE_Add_LyricView(MYTREE* tree)
{
	MYTREE *newTree=new MYTREE();
	MYTREE_Set_LyricView(newTree);

	wcscpy(newTree->data.nodeName,L"歌词面板");

	tree->addChildNodeAfter(newTree);

	tree->EvenPanes();

	UpdateLayout(tree);
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

	UpdateLayout(tree);
}

void CMyStatusBar::UpdateTrackInfoText()
{
	if(!MyLib::shared()->isPlaying())
		return;

	auto s= CBasicPlayer::shared();
	int totalSec=s->MillisecondsTotal()/1000;
	int minute=totalSec / 60;
	int sec=totalSec - minute*60 ;

	int usedtotalSec=s->MillisecondsPlayed()/1000;
	int usedMinute=usedtotalSec/60;
	int usedSec=usedtotalSec - usedMinute*60;

	TrackFormatInfo *info=GetTrackFormatInfo();
	TCHAR format[256]={0};
	_stprintf(format,_T("MP3 | %d kbps | %d Hz | %s | %d:%02d / %d:%02d"),info->rate,info->nSamplesPerSec ,info->getModeString(),usedMinute,usedSec,minute,sec);

	SetText(0,format);
}

CMainFrame::~CMainFrame()
{
	if(!m_bSaved)
		SaveAll();
}

//when windows is destroyed.
void CMainFrame::OnFinalMessage(_In_ HWND /*hWnd*/)
{
	if(!m_bSaved)
		SaveAll();
	m_bSaved=TRUE;
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
	m_wndsPlacement.length=sizeof(WINDOWPLACEMENT);
	GetWindowPlacement(&m_wndsPlacement);
	
	m_uShowState=IsZoomed()?SW_SHOWMAXIMIZED:(IsIconic()?SW_SHOWMINIMIZED:SW_SHOWNORMAL);


	
	if(m_pDlgConfig)
	{	
		m_rcConfig=m_pDlgConfig->m_rc;
		m_dlgConfigShow=m_pDlgConfig->m_show;
// 		RECT m_dlgLrcShow;
// 		RECT m_dlgProcessShow;
// 		RECT ;
// 		RECT m_dlgFFTShow;
// 		RECT m_dlgPLMngShow;
// 		RECT m_dlgPLConsoleShow;
	}
// 	if(m_dlgLrc)
// 		m_dlgLrc->GetWindowRect(&m_rcLrc);
// 	if(pDlgProcess)
// 		pDlgProcess->GetWindowRect(&m_rcProcess);
// 	if(m_pDlgSearch)
// 	{
// 		m_rcSearch=m_DlgSearch.m_rc;
// 		//m_dlgSearchShow=m_DlgSearch.m_show;
// 	}
// 	if(m_pDlgFFT)
// 		m_pDlgFFT->GetWindowRect(&m_rcFFT);
	if(m_pDlgPLMng)
		m_rcPLMng=m_pDlgPLMng->m_rc;

// 	if(m_pDlgConsole)
// 		m_pDlgConsole->GetWindowRect(&m_rcPLConsole);

	CBasicPlayer::shared()->Destroy();
	
	UnregisterHotKey(m_hWnd, HotKeyId); 

	GlobalDeleteAtom(HotKeyId);

	bHandled = FALSE;
	return 1;
}


LRESULT CMainFrame::OnNcDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	SaveAll();
	m_bSaved=TRUE;
	MyLib::shared()->stop();

	//exit and start the tray player .
	if (m_bExit2Tray )
	{
		ChangeCurDir2ModulePath();
		INT ret= (INT)
		ShellExecute(NULL,
			_T("open"),
			_T("playerTray.exe"),
			NULL,
			NULL,
			SW_HIDE );
		
		if (ret < 32)
		{
			MessageBox(L"Failed Executing playerTray");
		}
	}

	bHandled=FALSE;
	return 1;
}



void CPlayListView::OnDestroy()
{
	IDonotWantToReceiveMessage(WM_PLAYLISTVIEW_SETFOCUS);
	IDonotWantToReceiveMessage(WM_PLAYLISTVIEW_COLOR_DEFAULT);
	IDonotWantToReceiveMessage(WM_PLAYLISTVIEW_COLOR_BLUE);
	IDonotWantToReceiveMessage(WM_SELECTED_PL_CHANGED);


	IDonotWantToReceiveMessage(WM_SHOW_DIALOG_CONFIGURE );
	IDonotWantToReceiveMessage(WM_SAVE_CONFIGURE );
	IDonotWantToReceiveMessage(WM_SHOW_DIALOG_SEARCH );
	IDonotWantToReceiveMessage(WM_SHOW_DIALOG_PLAYLIST_MANAGER );
	IDonotWantToReceiveMessage(WM_CHANGE_LISTVIEW_FONT_ENLARGE );
	IDonotWantToReceiveMessage(WM_CHANGE_LISTVIEW_FONT_REDUCE );

	SetMsgHandled(FALSE);
}













HWND CMyTrackBarBase::CreateIsWnd(HWND parent)
{
	UINT style=WS_CHILD  | WS_VISIBLE /*| WS_CLIPCHILDREN | WS_CLIPSIBLINGS*/;
	style|=TBS_TOOLTIPS  |TBS_NOTICKS /*|TBS_AUTOTICKS */ | TBS_BOTH ; 
	UINT styleEx=0; 
	Create( parent,NULL,NULL,style,styleEx);

	SetPageSize(1);
	SetLineSize(1);
	SetThumbLength(30);

	m_bPressing=false;

	return m_hWnd;
}




DWORD CMyVolumeBar::OnItemPrePaint(int /*idCtrl*/, LPNMCUSTOMDRAW pNMCD)
{
	LRESULT lr = CDRF_DODEFAULT;


	//draw a triangle channel bar
	if(pNMCD->dwItemSpec == TBCD_CHANNEL)
	{
		CDC dc;
		dc.Attach(pNMCD->hdc);

		CRect r;
		GetClientRect(r);
		r.DeflateRect(8, 4, 10, 6);
		CopyRect(&pNMCD->rc, &r);

		CPen shadow;
		shadow.CreatePen(PS_SOLID, 1, GetSysColor(COLOR_3DSHADOW));
		CPen light;
		light.CreatePen(PS_SOLID, 1, GetSysColor(COLOR_3DHILIGHT));
		CPen old= dc.SelectPen(light);

		dc.MoveTo(pNMCD->rc.right, pNMCD->rc.top);
		dc.LineTo(pNMCD->rc.right, pNMCD->rc.bottom);
		dc.LineTo(pNMCD->rc.left, pNMCD->rc.bottom);
		dc.SelectPen(shadow);
		//dc.LineTo(pNMCD->rc.right, pNMCD->rc.top);

		DrawWuLine(&dc,pNMCD->rc.left,pNMCD->rc.bottom,pNMCD->rc.right,pNMCD->rc.top,GetSysColor(COLOR_3DSHADOW));

		dc.SelectPen(old);

		dc.Detach();



		/*
		RECT rc=pNMCD->rc;
		HDC  hdc=pNMCD->hdc;
		DWORD clrLight=GetSysColor(COLOR_3DFACE);

		TRIVERTEX vertex[3];
		vertex[0].x=rc.left;
		vertex[0].y=rc.bottom;
		vertex[0].Red=GetRValue(clrLight);
		vertex[0].Green=GetGValue(clrLight);
		vertex[0].Blue=GetBValue(clrLight);
		vertex[0].Alpha = 0x0000;

		vertex[1].x=rc.right;
		vertex[1].y=rc.top;
		vertex[1].Alpha = 0x0000;
		vertex[1].Red=GetRValue(clrLight);
		vertex[1].Green=GetGValue(clrLight);
		vertex[1].Blue=GetBValue(clrLight);


		vertex[2].x=rc.right;
		vertex[2].y=rc.bottom;
		vertex[2].Red=GetRValue(clrLight);
		vertex[2].Green=GetGValue(clrLight);
		vertex[2].Blue=GetBValue(clrLight);
		vertex[2].Alpha = 0x0000;

		GRADIENT_TRIANGLE gTriangle;
		gTriangle.Vertex1 = 0;
		gTriangle.Vertex2 = 1;
		gTriangle.Vertex3 = 2;
		GradientFill(hdc, vertex, 3, &gTriangle, 1, GRADIENT_FILL_TRIANGLE);
		*/



		lr = CDRF_SKIPDEFAULT;
	}


	pNMCD->uItemState &= ~CDIS_FOCUS;

	return lr;
}
