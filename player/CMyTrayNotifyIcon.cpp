#include "stdafx.h"
#include "resource1.h"
#include "CMyTrayNotifyIcon.h"
#include "BasicPlayer.h"
#include "globalStuffs.h"

BOOL CMyTrayNofityIcon::CreateSelf(HWND parent)
{
	CBasicPlayer *s=CBasicPlayer::shared();
	BOOL bPaused=s->paused();
	BOOL bStopped=s->stoped();
	BOOL bPlaying=!bStopped;
	TCHAR *text=bPlaying? (bPaused?_T("����ͣ"):_T("���ڲ���")) : _T(" ");
	CWindow w(parent);
	if (!Create(&w, IDR_MENU_TRAYPOPUP, GetAppName(), text, GetAppName(), 10, CTrayNotifyIcon::Info, LoadIconResource(IDR_MAINFRAME), WM_TRAYNOTIFY, 0, TRUE))
	{
		return FALSE;
	}


	return TRUE;
}