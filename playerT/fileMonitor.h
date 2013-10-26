#include "stdafx.h"
#include <vector>
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
	fileMonitor():hDir(NULL),bShouldWatch(true),hWatch(NULL)
	{
		memset(&myOverLapped,0,sizeof(myOverLapped));
	}

	~fileMonitor()
	{
		bShouldWatch=false;
		if(hDir)
			::CloseHandle(hDir);
		if (myOverLapped.overlapped.hEvent)
			::CloseHandle(myOverLapped.overlapped.hEvent);

		if (hWatch)
			TerminateThread(hWatch,NULL);
	}


public:
	int AddMonitorDirectory(const TCHAR* szDir)
	{
		//only one directory allowed
		ATLASSERT(!hDir);
		ATLASSERT(pPL);

		_tcscpy_s(szPath,szDir);

		hDir=::CreateFile(
			szPath,
			FILE_LIST_DIRECTORY,
			FILE_SHARE_WRITE|FILE_SHARE_READ|FILE_SHARE_DELETE,
			NULL,
			OPEN_EXISTING,FILE_FLAG_BACKUP_SEMANTICS|FILE_FLAG_OVERLAPPED ,
			NULL
			);

		if (!hDir)
			return FALSE;
		
		myOverLapped.overlapped.hEvent=::CreateEvent(0,FALSE,FALSE,0);

		::CreateThread(0,0,ThreadProc,this,0,0);

		return 0;
	}

	void Watch();


	void HandleNotify(FILE_NOTIFY_INFORMATION *pNotify);


public:
	OverlappedEx myOverLapped;   
	HANDLE hDir;
	HANDLE hWatch;
	bool bShouldWatch;
	PlayList *pPL;
	TCHAR szPath[MAX_PATH];
	TCHAR pathFrom[MAX_PATH];
	TCHAR pathTo[MAX_PATH];
};

class fileMonitors
{
public:
	fileMonitors(PlayList *pPL):pPL(pPL)
	{

	}

	~fileMonitors()
	{
		Reset();
	}

	int AddDirectory(const TCHAR *szPath)
	{
		fileMonitor *p=new fileMonitor;
		p->pPL=pPL;
		m_fileMonitors.push_back(p);
		return p->AddMonitorDirectory(szPath);
	}

	void DelDirectory(const TCHAR *szPath)
	{
		for (auto i=m_fileMonitors.begin();i!=m_fileMonitors.end();)
		{
			if(_tcscmp((*i)->szPath,szPath)==0)
			{
				delete (*i);
				m_fileMonitors.erase(i);
				break;
			}
		}
	}

	void Reset()
	{
		if (!m_fileMonitors.empty())
		{
			for (auto i=m_fileMonitors.begin();i!=m_fileMonitors.end();)
			{
				delete (*i);
			}

			m_fileMonitors.clear();
		}
		
	}

	PlayList *pPL;
private:
	std::vector<fileMonitor*> m_fileMonitors;
};