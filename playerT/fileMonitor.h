#include "stdafx.h"
#pragma once
class PlayList;
DWORD WINAPI ThreadProc(LPVOID lpParameter);
class fileMonitor
{
	typedef struct
	{
		OVERLAPPED overlapped;
		char notify[20240];
	}OverlappedEx;

public:
	fileMonitor():hDir(NULL)
	{
		memset(&myOverLapped,0,sizeof(myOverLapped));
	}

	~fileMonitor()
	{
		if(hDir)
			::CloseHandle(hDir);
		if (myOverLapped.overlapped.hEvent)
			::CloseHandle(myOverLapped.overlapped.hEvent);
	}


public:
	int AddMonitorDirectory(const TCHAR* szDir)
	{
		hDir=::CreateFile(
			szDir,
			FILE_LIST_DIRECTORY,
			FILE_SHARE_WRITE|FILE_SHARE_READ|FILE_SHARE_DELETE,
			NULL,
			OPEN_EXISTING,FILE_FLAG_BACKUP_SEMANTICS|FILE_FLAG_OVERLAPPED ,
			NULL
			);

		if (!hDir)
			return FALSE;
		
		_tcscpy(szPath,szDir);

		myOverLapped.overlapped.hEvent=::CreateEvent(0,FALSE,FALSE,0);

		::CreateThread(0,0,ThreadProc,this,0,0);

		return 0;
	}

	void Watch();


	void HandleNotify(FILE_NOTIFY_INFORMATION *pNotify);


public:
	OverlappedEx myOverLapped;
	HANDLE hDir;
	PlayList *pPL;
	TCHAR szPath[MAX_PATH];
	TCHAR pathFrom[MAX_PATH];
	TCHAR pathTo[MAX_PATH];
};

