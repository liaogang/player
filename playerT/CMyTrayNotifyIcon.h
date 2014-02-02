#include "stdafx.h"
#include "NTRAY.H"

#define WM_TRAYNOTIFY WM_USER + 100

class CMyTrayNofityIcon:public CTrayNotifyIcon 
{
public:
	virtual ~CMyTrayNofityIcon()
	{

	}


	BOOL CreateSelf(HWND parent);

};