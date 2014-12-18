#include "MainFrameTray.h"
#include "MySerialize.h"
#include "MyLib.h"
#include "globalStuffs.h"
#include "BasicPlayer.h"
#include "Util1.h"

LRESULT CMainFrame::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	m_bExit2UI=FALSE;

	ValidateCfg();

	trayNI.CreateSelf(m_hWnd);
	trayNI.SetDefaultMenuItem(ID_PLAY_NEXT,FALSE);

	RegisterMyHotKeys();

	m_uElapse=GetDoubleClickTime();

	PlayOrder order = MyLib::shared()->GetPlayOrder();
	m_nOrder=order;
	UISetCheck(ID_ORDER_DEFAULT+m_nOrder,TRUE);

	Update();

	return 0;
}

void CMainFrame::Update()
{
	CBasicPlayer *s=CBasicPlayer::shared();
	BOOL bPaused=s->paused();
	BOOL bStopped=s->stoped();
	BOOL bPlaying=!bStopped;
	UISetCheck(ID_STOP,bStopped);
	UISetCheck(ID_PAUSE,bPlaying && bPaused);
	UISetCheck(ID_PLAY,bPlaying);
	

	if(bStopped)
		trayNI.SetTooltipText(GetAppName());
	else
	{
		auto item=MyLib::shared()->GetPlayingItem();
		TCHAR buf[128];
		int len;

		std::tstring artist=item->GetArtist();
		std::tstring album=item->GetAlbum();

		_stprintf_s(buf,_T("[%s]\n标题: %s"),bPaused?_T("已暂停"):_T("播放中") ,item->GetTitle().c_str());
		
		len= _tcslen(buf);
		if(!artist.empty())
			_stprintf(&buf[len],_T("\n艺术家: %s"),artist.c_str());

		len= _tcslen(buf);
		if(!album.empty())
			_stprintf(&buf[len],_T("\n专辑: %s"),album.c_str() );
		
		trayNI.SetTooltipText(buf);

		trayNI.SetBalloonDetails(buf,m_strAction,CTrayNotifyIcon::Info,10);
	}
}

LRESULT CMainFrame::OnTrackReachEnd(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& bHandled)
{
	MyLib::shared()->playNext();
	Update();
	return 0;
}

LRESULT CMainFrame::OnPlay(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	MyLib::shared()->stop();
	MyLib::shared()->play();
	
	m_strAction=_T("正在播放: ");

	Update();
	return 0;
}

LRESULT CMainFrame::OnPause(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	if(CBasicPlayer::shared()->paused())
		m_strAction=_T("已取消暂停: ");
	
	else
		m_strAction=_T("已暂停: ");

	MyLib::shared()->pause();
	Update();
	return 0;
}

LRESULT CMainFrame::OnStop(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	MyLib::shared()->stop();

	UISetCheck(ID_STOP,TRUE);
	UISetCheck(ID_PLAY,FALSE);

	Update();

	return 0;
}

LRESULT CMainFrame::OnPlayPrev(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	return 0;
}

LRESULT CMainFrame::OnPlayNext(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	MyLib::shared()->stop();

	if(MyLib::shared()->GetPlayOrder() == Repeat_track)
		MyLib::shared()->playNext(TRUE,Default);
	else
		MyLib::shared()->playNext(TRUE);

	m_strAction=_T("正在播放: ");
	Update();

	return 0;
}


LRESULT CMainFrame::OnPlayRandom(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	MyLib::shared()->stop();
	MyLib::shared()->playNext(TRUE,Random);

	m_strAction=_T("正在播放: ");
	Update();

	return 0;
}

LRESULT CMainFrame::OnSetOrder(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	PlayOrder order = static_cast<PlayOrder>(wID - ID_ORDER_DEFAULT);
	MyLib::shared()->SetPlayOrder(order);
	
	UISetCheck(ID_ORDER_DEFAULT+m_nOrder,FALSE);
	m_nOrder=order;
	UISetCheck(ID_ORDER_DEFAULT+m_nOrder,TRUE);
	
	return 0;
}


LRESULT CMainFrame::OnFileExit(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	m_bExit2UI= wID ==ID__EXIT2UI;

	UnregisterHotKey(m_hWnd, HotKeyId); 
	GlobalDeleteAtom(HotKeyId);

	CBasicPlayer::shared()->Destroy();
	SaveAll();


	if(m_bExit2UI)
	{
		ChangeCurDir2ModulePath();
		ShellExecute(NULL,
			_T("open"),
			_T("player.exe"),
			NULL,
			NULL,
			SW_SHOW);
	}

	PostMessage(WM_CLOSE);
	PostMessage(WM_QUIT);
	
	return 0;
}



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

LRESULT CMainFrame::OnTrayNotification(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	//Show Info b... when left button clicked once.
	if (lParam == WM_LBUTTONDBLCLK) 
	{
		if(m_nIDEvent!=NULL)
			KillTimer((UINT_PTR)&m_nIDEvent);

		MyLib *s=MyLib::shared();
		CBasicPlayer *b=CBasicPlayer::shared();
		if(b->stoped() )
			OnPlay(0,0,0,bHandled);
		else if(b->paused() )
			OnPause(0,0,0,bHandled);
		else
			OnPlayNext(0,0,0,bHandled);
	}
	else
	if (lParam == WM_LBUTTONUP) 
	{
		SetTimer((UINT_PTR)&m_nIDEvent,m_uElapse,NULL);
	}
	else
		trayNI.OnTrayNotification(wParam,lParam);

	return 0L;
}

LRESULT CMainFrame::OnTimer(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	KillTimer((UINT_PTR)&m_nIDEvent);
	m_nIDEvent=NULL;
		
	Update();

	return 0L;
}