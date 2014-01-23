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

	RegisterStuffs();

	if(m_rcMain.right>0 && m_rcMain.bottom > 0)
	::SetWindowPos(m_hWnd, NULL, m_rcMain.left, m_rcMain.top, m_rcMain.right - m_rcMain.left, m_rcMain.bottom - m_rcMain.top,SWP_NOZORDER);

	SetWindowText(GetAppName());

	SetTimer((UINT_PTR)&m_nIDEvent,m_uElapse,NULL);


	SetMenu(NULL);

	m_hWndToolBar=m_wndRebar.CreateIsWnd(m_hWnd);
	m_wndRebar.ResetBands();


	CreateSimpleStatusBar();
	m_pStatus=new CMyStatusBar;
	m_pStatus->SubclassWindow(m_hWndStatusBar);
	m_pStatus->Init();

	UIAddStatusBar(m_hWndStatusBar);
	ShowStatusBar(m_bShowStatusBar);


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

vector<MY_REBARBANDINFO*> CMySimpleRebar::m_vecBandInfos;
LRESULT CMySimpleRebar::OnCtrlColorStatic(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	LRESULT lResult=FALSE;
	HDC dc=(HDC)wParam;
	HWND wnd=(HWND)lParam;



	RECT rc;
	::GetWindowRect(m_hWnd,&rc);

	HDC dcMem;
	dcMem= ::CreateCompatibleDC(dc);
	HBITMAP bmp,oldBmp;
	bmp=::CreateCompatibleBitmap(dc,rc.right-rc.left,rc.bottom-rc.top);
	oldBmp=(HBITMAP)::SelectObject(dcMem,bmp);

	POINT pt={0,0};
	::MapWindowPoints(wnd,m_hWnd,&pt,1);
	::OffsetWindowOrgEx(dcMem,pt.x,pt.y,&pt);
	::SendMessage(m_hWnd,WM_ERASEBKGND, (WPARAM)dcMem, 0L);
	::SetWindowOrgEx(dcMem,pt.x,pt.y,NULL);
	::DeleteDC(dcMem);

	static HBRUSH hBrush=0;
	if (hBrush)
		::DeleteObject((HGDIOBJ)hBrush);

	hBrush = ::CreatePatternBrush(bmp);
	::DeleteObject(bmp);

	lResult=(LRESULT)hBrush;

	return lResult;
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
	MyLib::shared()->stop();
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
	_itow(pPL->GetItemCount(),strCount,10);

	InsertItem(item,pPL->m_playlistName.c_str());
	SetItemText(item,1,strCount);
	SetItemText(item,2,pPL->m_bAuto?_T("自动列表"):_T("普通列表"));
	SetItemData(item,(DWORD_PTR)pPL);
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
			_itow(pPL->GetItemCount(),strCount,10);
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



LRESULT CMainFrame::OnNewTrackStarted(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	ChangeTitle();

	AllPlayListViews()->Reload(MyLib::shared()->GetPlayingPL(),TRUE);

	//refresh the active playlist item. 
	if(m_pDlgPLMng && m_pDlgPLMng->IsWindow())
		m_pDlgPLMng->m_list.Invalidate();
	
	return 0;
}


LRESULT CMyComboBox::OnCbnSelchanged(UINT,int id, HWND hWndCtl)
{
	MyLib::shared()->SetPlayOrder(static_cast<MyLib::PlayOrder>( GetCurSel() ));
	

	SetMsgHandled(FALSE);
	return 0;
}




LRESULT CMainFrame::OnTrackReachEnd(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	CBasicPlayer* s=CBasicPlayer::shared();
	s->stop();

	MyLib::shared()->playNext();

	return 0;
}

LRESULT CMainFrame::OnTrackStopped(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	ChangeTitle();

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
		PlayList *anotherSel=NULL;
		if(size!=0)
			anotherSel=* MyLib::shared()->m_playLists.begin();

		AllPlayListViews()->Reload(anotherSel);
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


LRESULT CMainFrame::OnPLVCenterItem(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	AllPlayListViews()->Reload(MyLib::shared()->GetPlayingPL(),TRUE);
	NotifyMsg(WM_PLAYLISTVIEW_SETFOCUS);
	return TRUE;
}

void CMyStatusBar::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	::PostMessage(GetMainFrame()->m_hWnd,WM_PLAYLISTVIEW_CENTER_ITEM,NULL,NULL);
}



LRESULT CPlayListView::OnDbClicked(int /**/,LPNMHDR pnmh,BOOL bHandled)
{	
	if(!GetPlayList())return 0;

	int k=GetPlayList()->GetSelectedIndex();
	if(k!=-1)
	{
		MyLib::shared()->play();
		GetPlayList()->SetTopVisibleIndex(k);
	}

	SetMsgHandled(FALSE);
	return 0;
} 


LRESULT CPlayListViewS::OnDbClicked(int /**/,LPNMHDR pnmh,BOOL bHandled)
{	
	if(!GetPlayList())return 0;

	int k=playlistParent->GetSelectedIndex();
	if(k!=-1)
	{
		MyLib::shared()->play();
		GetPlayList()->SetTopVisibleIndex(k);
	}

	SetMsgHandled(TRUE);
	return 0;
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
	if(item==NULL)return;
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
		if(!m_pDlgSearch->Create(m_hWnd))
			{
				DWORD a=GetLastError();
				return;
		}

		if(m_pDlgSearch->IsWindow() && m_rcSearch.right>0 && m_rcSearch.bottom > 0)
			::SetWindowPos(m_pDlgSearch->m_hWnd, NULL, m_rcSearch.left, m_rcSearch.top, m_rcSearch.right - m_rcSearch.left, m_rcSearch.bottom - m_rcSearch.top,SWP_NOZORDER);
	}
	
	m_pDlgSearch->ShowSelf();
}

void CMainFrame::RegisterStuffs()
{
	CMyCommandBarCtrl::RegisterSelf();
	CMyToolBar::RegisterSelf();
	CMyTrackBar::RegisterSelf();
	CMyComboBox::RegisterSelf();
	CMyVolumeBar::RegisterSelf();
	

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





	LRESULT  CPlayListManager::OnDbClicked(UINT i,CPoint pt)
	{
		int k=GetFirstSelItem();
		if(k!=-1)
		{
			PlayList *l=(PlayList*)GetItemData(k);

			SetSelectedPlaylist(l);

			AllPlayListViews()->Reload(l);

		}

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
			NotifyMsg(WM_LYRIC_RELOAD);

		}
	}


	savePath=NULL;

}






BOOL MyLib::play(FileTrack * track)
{
	ATLASSERT(track);

	if(!track->ScanId3Info(TRUE,TRUE))
		return FALSE;

	CBasicPlayer* s=CBasicPlayer::shared();
	
	if(!s->stoped())
		s->stop();

	if(s->open(track))
	{
		 s->play();
		 return TRUE;
	}

	return FALSE;
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

LRESULT CPlayListViewS::OnPushToPlayqueue(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	MyLib *s=MyLib::shared();
	CListArray < int >  aSelectedItems;
	if(GetSelectedItems(aSelectedItems))
		for (int i=0;i<aSelectedItems.GetSize();i++)
		{
			PlayListItem *item=GetPlayList()->GetItem(aSelectedItems[i]);
			PlayListItem *itemInParent=GetItemInParent(item);
			s->PushPlayQueue(itemInParent);
		}
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

	MyLib::shared()->SetSelectedPL(pPL);
}




LRESULT CPlayListView::OnSelected(int /**/,LPNMHDR pnmh,BOOL bHandled)
{	
// 	NMLISTVIEW * pnml=(NMLISTVIEW *)pnmh;
// 	int nItem=pnml->iItem;
	
	int nItem=GetFirrstSelectedItem();

	GetPlayList()->SetTopVisibleIndex(nItem);
	GetPlayList()->SetSelectedIndex(nItem);

	return 0;
}


LRESULT CPlayListViewS::OnSelected(int /**/,LPNMHDR pnmh,BOOL bHandled)
{	
	vector<int> items;

	CListArray < int > aSelectedItems;
	GetSelectedItems(aSelectedItems);

	for (int count=0;count< aSelectedItems.GetSize();count++)
	{
		int nItem=aSelectedItems[count];
		PlayListItem *itemInParent=GetItemInParent(GetPlayList()->GetItem(nItem));
		
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

void CPropertyDlgMediaLib::InitList()
{
	m_list.DeleteAllItems();

	MyLib * s=MyLib::shared();
	for (auto i=s->mediaPaths.begin();i!=s->mediaPaths.end();++i)
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

	CBasicPlayer::shared()->Destroy();

	SaveAll();


	UnregisterHotKey(m_hWnd, HotKeyId); 

	GlobalDeleteAtom(HotKeyId);



	bHandled = FALSE;
	return 1;
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
		

	SetDragDrop(TRUE);
	//SetSingleSelect();

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

	if(c->getResumeOnReboot())
	{
		c->playingStatus=CBasicPlayer::shared()->m_lastStatus;
		c->pos=CBasicPlayer::shared()->m_lastPos;
	}

	c->setPlayersVolume(CBasicPlayer::shared()->m_curVolume);
	c->playorder=MyLib::shared()->playorder;
}

void ValidateCfg()
{
	MyConfigs *c=GetMyConfigs();
	CBasicPlayer::shared()->SetVolumeByEar(c->getPlayersVolume());
	MyLib::shared()->SetPlayOrder(static_cast<MyLib::PlayOrder>(c->playorder));

	MyLib* s= MyLib::shared();
	PlayList *pl= s->Index2Playlist(c->playlistIndex);
	if(pl)
	{	s->SetPlayingIndex(c->playlistIndex);
		pl->SetPlayingIndex(c->trackIndex);
	}

	if(c->getResumeOnReboot() )
	{
		if(c->playingStatus==status_playing||c->playingStatus==status_paused)
		{
			CBasicPlayer::shared()->m_lastStatus=c->playingStatus;
			CBasicPlayer::shared()->m_lastPos=c->pos;
			SdMsg(WM_COMMAND,TRUE,MAKEWPARAM(ID_PLAY,0),(LPARAM)0);	
		}
	}
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


HWND CreateTrackBand(HWND hWndParent)
{
	CMyTrackBar *pB=new CMyTrackBar;
	pB->CreateIsWnd(hWndParent);
	return pB->m_hWnd;
}


HWND CreateComboBand(HWND hWndParent)
{
	CMyComboBox *pB=new CMyComboBox;
	pB->CreateIsWnd(hWndParent);
	return pB->m_hWnd;
}


HWND CreateVolumeBand(HWND hWndParent)
{
	CMyStatusBar *pB=new CMyStatusBar;
	pB->Create(hWndParent,NULL,NULL,WS_CHILD  | WS_VISIBLE |TBS_TOOLTIPS  |TBS_NOTICKS |TBS_AUTOTICKS  | TBS_BOTH ,0);
	pB->Init();
	return pB->m_hWnd;
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