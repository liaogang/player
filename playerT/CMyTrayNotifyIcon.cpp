#include "CMyTrayNotifyIcon.h"
#include "BasicPlayer.h"
#include "globalStuffs.h"

BOOL CMyTrayNofityIcon::CreateSelf(HWND parent)
{
	CBasicPlayer *s=CBasicPlayer::shared();
	BOOL bPaused=s->paused();
	BOOL bStopped=s->stoped();
	BOOL bPlaying=!bStopped;
	TCHAR *text=bPlaying? (bPaused?_T("已暂停"):_T("正在播放")) : _T(" ");
	CWindow w(parent);
	if (!Create(&w, IDR_MENU_TRAYPOPUP, GetAppName(), text, GetAppName(), 10, CTrayNotifyIcon::Info, LoadIconResource(ID_HAPPY), WM_TRAYNOTIFY, 0, TRUE))
	{
		return FALSE;
	}


	return TRUE;
}