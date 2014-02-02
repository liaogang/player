#include "MainFrameTray.h"
#include "MySerialize.h"
#include "MyLib.h"
#include "globalStuffs.h"
#include "BasicPlayer.h"

LRESULT CMainFrame::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	LoadAll();
	trayNI.CreateSelf(m_hWnd);



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
		_stprintf_s(buf,_T("[%s]\n标题:%s\n艺术家:%s\n   "),
			bPaused?_T("已暂停"):_T("播放中"),
			item->GetTitle().c_str(),
			item->GetArtist().c_str());
		trayNI.SetTooltipText(buf);
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
	

	Update();
	return 0;
}

LRESULT CMainFrame::OnPause(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
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

	Update();
	return 0;
}

LRESULT CMainFrame::OnPlayRandom(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	MyLib::shared()->stop();
	MyLib::shared()->playNext(TRUE,Random);


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


LRESULT CMainFrame::OnFileExit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CBasicPlayer::shared()->Destroy();
	SaveAll();

	PostMessage(WM_CLOSE);
	PostMessage(WM_QUIT);
	
	return 0;
}