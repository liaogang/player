#include "stdafx.h"
#include "NTRAY.H"

#define WM_TRAYNOTIFY WM_USER + 100

class CMyTrayNofityIcon:public CTrayNotifyIcon 
{
public:
	virtual ~CMyTrayNofityIcon()
	{

	}


	BOOL CreateSelf(HWND parent)
	{
		CWindow w(parent);
		if (!Create(&w, IDR_MENU_TRAYPOPUP, _T("player"), _T("Demo Tray Application"), _T("First Tray Icon"), 10, User, LoadIconResource(ID_HAPPY), WM_TRAYNOTIFY, 0, TRUE))
	//	if (!g_TrayIcon1.Create(this, IDR_MENU_TRAYPOPUP, _T("player"), m_hIcons[0], WM_TRAYNOTIFY)) //Uncomment this line and comment out line above if you want to start with a tray icon without animations
		{
			//MessageBox(_T("Failed to create tray icon 1"), "Error",MB_OK | MB_ICONSTOP);
			return -1;
		}
		return TRUE;
	}

};