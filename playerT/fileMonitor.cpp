#include "stdafx.h"
#include "fileMonitor.h"
#include "PlayList.h"
#include "StringConvertions.h"
#include "PlayListView.h"
#include "forwardMsg.h"
DWORD WINAPI ThreadProc(LPVOID lpParameter)
{
	auto p=(fileMonitor*)lpParameter;
	p->Watch();
	return 0;
}

void fileMonitor::HandleNotify(FILE_NOTIFY_INFORMATION *pNotify)
{
	int len=_tcslen(szPath);     
	memset(pathTo,0,sizeof(pathTo));
	_tcscpy(pathTo,szPath);
	pathTo[len++]='\\';
	while(1)
	{
		wcsncpy(pathTo+len,pNotify->FileName,pNotify->FileNameLength/sizeof(TCHAR));

		switch(pNotify->Action)
		{
		case FILE_ACTION_ADDED:
			::Sleep(250);
			pPL->AddFile(pathTo);
			break;
		case FILE_ACTION_REMOVED:
			pPL->DeleteTrackByPath(pathTo);
			break;
		case FILE_ACTION_RENAMED_OLD_NAME:
			_tcscpy(pathFrom,pathTo);
			break;
		case FILE_ACTION_RENAMED_NEW_NAME:
			pPL->ChangeTrackPath(pathFrom,pathTo);
			break;
		}

		if(pNotify->NextEntryOffset!=0)
			pNotify=(FILE_NOTIFY_INFORMATION*)((BYTE*)pNotify+pNotify->NextEntryOffset);
		else break;
	}
}




void fileMonitor::Watch()
{
	while(bShouldWatch)
	{
		if(::ReadDirectoryChangesW(hDir,
			myOverLapped.notify,sizeof(myOverLapped.notify),
			FALSE,
			FILE_NOTIFY_CHANGE_FILE_NAME|FILE_NOTIFY_CHANGE_DIR_NAME,
			0,&(myOverLapped.overlapped),0))
		{
			DWORD w=::WaitForSingleObject(myOverLapped.overlapped.hEvent,INFINITE);
			if (w==WAIT_OBJECT_0){
				FILE_NOTIFY_INFORMATION *pNotify=(FILE_NOTIFY_INFORMATION*)myOverLapped.notify;
				HandleNotify(pNotify);
				NotifyMsg(WM_PL_TRACKNUM_CHANGED,(WPARAM)pPL,NULL);
				}
		}
	}


	hWatch=NULL;
}