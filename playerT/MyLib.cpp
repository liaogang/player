#include "MyLib.h"
#include "BasicPlayer.h"
#include "PlayListViewMng.h"
#include "globalStuffs.h"
//-----------------------------------------
//

PlayList*  MyLib::NewPlaylist(std::tstring playlistname,bool bAutoPL)
{
	PlayList *l=new PlayList(playlistname,bAutoPL);
	m_playLists.push_back(l);

	return l;
}

PlayList* MyLib::AddFolderToCurrentPlayList(LPCTSTR pszFolder)
{
	PlayList *p=ActivePlaylist();
	p->AddFolderByThread(pszFolder);
	return p;
}

static MyLib* gMylib=NULL;
MyLib* MyLib::shared()
{
	if(gMylib==NULL)
		gMylib=new MyLib();

	return gMylib;
}

MyLib::~MyLib()
{
	CBasicPlayer::shared()->stop();

	PLList::iterator i;
	for (i=m_playLists.begin();i!=m_playLists.end();i++)
		delete *i;
}

void MyLib::playAfterSlowDown(FileTrack  *item)
{
	CBasicPlayer::shared()->OpenAfterSlowDown(item);
}


//-----------------------------------------

void MyLib::playNext(BOOL scanID3)
{	
	PlayListItem track;
 	if (!playQueue.empty())
	{
		track=PopTrackFromPlayQueue();
		SetActivePlaylist(track.GetPlayList());
		
	}
	else
	{
		track=ActivePlaylist()->GetNextTrackByOrder();
	}

	ActivePlaylist()->SetCurPlaying(track);

	AllPlayListViews()->PlayingItemChanged();

	//todo update listview's item
	track.ScanId3Info(TRUE);

	CBasicPlayer *sbp=CBasicPlayer::shared();
	if (1)//todo track==ActivePlaylist()->lastTrack())
	{
		sbp->m_bFileEnd=FALSE;
		sbp->play();
	}
	else
	{
		sbp->stop();
		sbp->open(track.GetFileTrack());
		sbp->play();
	}
}

//
PlayList* MyLib::GetAutoPlaylist()
{
	static PlayList *p=NULL;
	if (!p)
	{
		p=NewPlaylist(_T("自动播放列表"),true);
	}
	
	return p;
}


void MyLib::ClearMediaPath()
{
	mediaPaths.clear();
	GetAutoPlaylist()->m_fileMonitor.Reset();
}

void MyLib::AddMediaPath(std::tstring &path)
{
	mediaPaths.push_back(path);
	GetAutoPlaylist()->m_fileMonitor.DelDirectory(path.c_str());

}

void MyLib::DelMediaPath(std::tstring &path)
{
	GetAutoPlaylist()->m_fileMonitor.DelDirectory(path.c_str());
}


//后端插入,前端取出
void MyLib::PushPlayQueue(_songContainerItem item)
{
	playQueue.push_back(item);
}

PlayListItem MyLib::PopTrackFromPlayQueue()
{
	PlayQueueContainer::iterator i=playQueue.begin();
	PlayListItem item=*i;
	playQueue.erase(i);

	return item;
}

vector<int> MyLib::GetIndexInPlayQueue(_songContainerItem item)
{
	vector<int> v;
	int idx=0;
	for(PlayQueueContainer::iterator i=playQueue.begin();
		i!=playQueue.end();++i,++idx)
		if( (void*)((*i).GetFileTrack()) == (void*)(item.GetFileTrack()))
			v.push_back(idx);
	
	return v;
}

void MyLib::DeleteFromPlayQueue(_songContainerItem item)
{
	for(PlayQueueContainer::iterator i=playQueue.begin();
		i!=playQueue.end();)
	{
		PlayQueueContainer::iterator k=i;
		++i;

		if(*k == item)
			playQueue.erase(k);
	}
}

void MyLib::ClearPlayQueue()
{
	playQueue.clear();
}


void MyLib::InitLrcLib()
{
	dataPaths.clear();
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



void MyLib::DeletePlayList(PlayList *pl)
{
	m_playLists.remove(pl);
	delete pl;
	//todo 
	//if pl == active pl
	//active = select pl
}