#include "MainFrameTray.h"
#include "MySerialize.h"
#include "MyLib.h"
#include "globalStuffs.h"

LRESULT CMainFrame::OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	LoadAll();
	trayNI.CreateSelf(m_hWnd);
	return 0;
}

LRESULT CMainFrame::OnPlay(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	MyLib::shared()->stop();
	MyLib::shared()->play();
	return 0;
}

LRESULT CMainFrame::OnPause(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	MyLib::shared()->pause();
	return 0;
}

LRESULT CMainFrame::OnStop(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	MyLib::shared()->stop();
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
	return 0;
}

LRESULT CMainFrame::OnPlayRandom(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	MyLib::shared()->stop();
	MyLib::shared()->playNext(TRUE,Random);

	return 0;
}

LRESULT CMainFrame::OnSetOrder(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	PlayOrder order = static_cast<PlayOrder>(wID - ID_ORDER_DEFAULT);
	MyLib::shared()->SetPlayOrder(order);

	return 0;
}


LRESULT CMainFrame::OnFileExit(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	PostMessage(WM_CLOSE);
	PostMessage(WM_QUIT);
	SaveAll();
	return 0;
}