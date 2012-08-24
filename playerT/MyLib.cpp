#include "MyLib.h"
#include "BasicPlayer.h"
#include "ProgertyDlg.h"
//-----------------------------------------
//

PlayList*  MyLib::NewPlaylist(std::tstring playlistname)
{
	PlayList *l=new PlayList(playlistname);
	m_playLists.push_back(*l);
	return l;
}

PlayList* MyLib::AddFolderToCurrentPlayList(LPCTSTR pszFolder)
{
	PlayList *p=MyLib::shared()->m_pSelPlaylist;
	p->AddFolderByThread(pszFolder);
	return p;
}

MyLib* MyLib::shared()
{
	static MyLib* p=NULL;
	if (p==NULL)
	{
		p=new MyLib;
		p->LoadCoreCfg();
	}
	return p;
}

MyLib::~MyLib()
{
	SaveCoreCfg();
}

void MyLib::playAfterSlowDown()
{
	CBasicPlayer::shared()->OpenAfterSlowDown();
}

void MyLib::play()
{
	PlayList* playlist=MyLib::shared()->ActivePlaylist();
	if (!playlist)
		return;

	PlayListItem* curTrack=playlist->curTrack();
	CBasicPlayer* s=CBasicPlayer::shared();
	s->open(curTrack);
	s->play();
}


void MyLib::pause()
{
	CBasicPlayer::shared()->pause();
}

void MyLib::stop()
{
	CBasicPlayer::shared()->stop();
}

//-----------------------------------------

void MyLib::playNext(BOOL scanID3)
{	
	PlayListItem *track=shared()->ActivePlaylist()->GetNextTrackByOrder();
	if(!track) return;

	track->ScanId3Info(TRUE);

	CBasicPlayer *sbp=CBasicPlayer::shared();
	if ( track==shared()->ActivePlaylist()->lastTrack())
	{
		sbp->m_bFileEnd=FALSE;
		sbp->play();
	}
	else
	{
		sbp->stop();
		sbp->open(track);
		sbp->play();
	}

}


LRESULT CPropertyDlgMediaLib::OnBtnAdd(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	UINT uFlags=BIF_RETURNONLYFSDIRS|BIF_NEWDIALOGSTYLE;
	uFlags&=~BIF_NONEWFOLDERBUTTON ;
	CFolderDialog dlg(m_hWnd,_T("请选择要添加的文件夹"),uFlags);
	if (dlg.DoModal()==IDOK){
		LPCTSTR path=dlg.GetFolderPath();
		//MyLib::shared()->AddFolder2LrcSearchLib(path);
		m_list.InsertItem(0,path);
	}

	return 0;
}

LRESULT CPropertyDlgMediaLib::OnBtnDel(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	return 0;
}

BOOL CPropertyDlgLyricsLib::OnInitDialog(CWindow wndFocus, LPARAM lInitParam)
{
	//DoDataExchange();
	std::tstring searchPath;
	
	vector<std::tstring>::iterator i;
	if (MyLib::shared()->lrcDirs.size()>0)
	{
		i=MyLib::shared()->lrcDirs.begin();
		searchPath=*i;
	}

	::SetWindowText(GetDlgItem(IDC_EDIT_SEARCH),searchPath.c_str());


	return TRUE;
}

LRESULT CPropertyDlgLyricsLib::OnBtnOpenDir(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	UINT uFlags=BIF_RETURNONLYFSDIRS|BIF_NEWDIALOGSTYLE;
	uFlags&=~BIF_NONEWFOLDERBUTTON ;
	CFolderDialog dlg(m_hWnd,_T("请选择要添加的文件夹"),uFlags);
	if (dlg.DoModal()==IDOK){
		LPCTSTR path=dlg.GetFolderPath();
		::SetWindowText(GetDlgItem(IDC_EDIT_SEARCH),path);
	}


	return 0;
}


LRESULT CPropertyDlgLyricsLib::OnCfgToSave(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	TCHAR lpszPath[MAX_PATH]={};
	::GetWindowText(GetDlgItem(IDC_EDIT_SEARCH),lpszPath,MAX_PATH);
	MyLib::shared()->AddFolder2LrcSearchLib(lpszPath);

	MyLib::shared()->InitLrcLib();
	return 0;
}

void MyLib::InitLrcLib()
{
	for_each(lrcDirs.begin(),lrcDirs.end(),&MyLib::ImportLycByPath);
}

void MyLib::ImportLycByPath(std::tstring path)
{
	LPCTSTR pszFolder=path.c_str();

	TCHAR* curPath=new TCHAR[MAX_PATH];
	_tgetcwd(curPath,MAX_PATH);

	//改变当前目录
	_tchdir(pszFolder);

	BOOL findResult=FALSE;
	WIN32_FIND_DATA findFileData;
	HANDLE hFind;

	hFind=::FindFirstFile(_T("*.lrc"),&findFileData);
	if(hFind!=INVALID_HANDLE_VALUE){
		findResult=TRUE;
		while(findResult)
		{
			TCHAR *_findName=new TCHAR[MAX_PATH];
			_tcscpy(_findName,findFileData.cFileName);
			TCHAR *pathName=new TCHAR[MAX_PATH+1];
			_tcscpy(pathName,pszFolder);
			_tcscat(pathName,_T("\\"));
			_tcscat(pathName,_findName);
			//std::tstring str(pathName);
			
			std::tstring str(pathName);
			MyLib::shared()->dataPaths.push_back(str);

			findResult=FindNextFile(hFind,&findFileData);
		}
		FindClose(hFind);
	}

	//回复当前目录
	_tchdir(curPath);
}