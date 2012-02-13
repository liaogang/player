#include "StdAfx.h"
#include "PlayList.h"
#include <direct.h>
PlayList& MyLib::GetPlayListObj()
{
	static PlayList playList_;
	return playList_;
}



PlayList::PlayList(void)
{
}

PlayList::~PlayList(void)
{
}

 void PlayList::AddFolderToCurrentPlayList(LPCTSTR pszFolder)
{
	MyLib::GetPlayListObj().AddFolder(pszFolder);
}

 BOOL PlayList::AddFolder(LPCTSTR pszFolder)
 {
	 // for each mp3 file in this folder
	 // data . add

	 //忽略了子目录下的mp3文件

	//改变当前目录
	 _tchdir(pszFolder);

	 WIN32_FIND_DATA findFileData;
	 HANDLE hFind;
	 
	 hFind=::FindFirstFile(_T("*.mp3"),&findFileData);

	 if(hFind!=INVALID_HANDLE_VALUE)
	 {
		 if (1)//(findFileData.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY))
			{
				TCHAR *_findName=new TCHAR[MAX_PATH];
				_tcscpy(_findName,findFileData.cFileName);
				TCHAR *pathName=new TCHAR[MAX_PATH+1];
				_tcscpy(pathName,pszFolder);
				_tcscat(pathName,_T("\\"));
				

				_tcscat(pathName,_findName);
				tstring str(pathName);
				m_songList.push_back(str);
			}

		 BOOL ret;
		 while(ret=FindNextFile(hFind,&findFileData))
			{
				if (1)//(!(findFileData.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY))
				{
					TCHAR *_findName=new TCHAR[MAX_PATH];
					_tcscpy(_findName,findFileData.cFileName);
					TCHAR *pathName=new TCHAR[MAX_PATH+1];
					_tcscpy(pathName,pszFolder);
					_tcscat(pathName,_T("\\"));


					_tcscat(pathName,_findName);
					tstring str(pathName);
					m_songList.push_back(str);
				}
			}
	 }
	 FindClose(hFind);

	return TRUE;
 }