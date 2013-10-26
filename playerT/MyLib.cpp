#include "MyLib.h"
#include "BasicPlayer.h"
#include "PlayListViewMng.h"
#include "globalStuffs.h"
//-----------------------------------------
//

PlayList* ActivePlaylist()
{
	return MyLib::shared()->GetSelectedPL();
}

void SetActivePlaylist(PlayList* pl)
{
	MyLib::shared()->SetSelectedIndex(pl);
}

PlayList*  MyLib::NewPlaylist(std::tstring playlistname,bool bAutoPL)
{
	PlayList *l=new PlayList(playlistname);
	m_playLists.push_back(l);

	if(bAutoPL)
		l->m_bAuto=bAutoPL;

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

/*
void MyLib::playAfterSlowDown(FileTrack  *item)
{
	//CBasicPlayer::shared()->OpenAfterSlowDown(item);
}*/


//-----------------------------------------

void MyLib::playNext(BOOL scanID3)
{	
	PlayListItem *track;
 	if (!playQueue.empty())
	{
		track=PopTrackFromPlayQueue();
		SetActivePlaylist(track->GetPlayList());
	}
	else
	{
		track=ActivePlaylist()->GetNextTrackByOrder();
	}

	if(!track->isValide())
		return;

	ActivePlaylist()->SetCurPlaying(track);

	//AllPlayListViews()->PlayingItemChanged();

	//todo update listview's item
	//track.ScanId3Info(TRUE);

	//CBasicPlayer *sbp=CBasicPlayer::shared();
	//if (0)//todo track==ActivePlaylist()->lastTrack())
	//{
	//	sbp->m_bFileEnd=FALSE;
	//	sbp->play();
	//}
	//else
	//{
		::SetPlayingItem(track);
		play(track->GetFileTrack());

		//sbp->open(track.GetFileTrack());
		//sbp->play();
	//}
}



static PlayList *pPlayListAuto=NULL;
PlayList* MyLib::GetAutoPlaylist()
{
	if (!pPlayListAuto)
	{
		pPlayListAuto=NewPlaylist(_T("自动播放列表"),true);
		m_pFileMonitor=new fileMonitors(pPlayListAuto);
	}
	
	return pPlayListAuto;
}

void MyLib::SetAutoPlayList(PlayList *pl)
{
	pPlayListAuto=pl;
	m_pFileMonitor=new fileMonitors(pPlayListAuto);
}


void MyLib::ClearMediaPath()
{
	mediaPaths.clear();
	m_pFileMonitor->Reset();
}

void MyLib::AddMediaPath(std::tstring &path)
{
	mediaPaths.push_back(path);
	m_pFileMonitor->AddDirectory(path.c_str());
}

void MyLib::DelMediaPath(std::tstring &path)
{
	m_pFileMonitor->DelDirectory(path.c_str());
	mediaPaths.erase( find(mediaPaths.begin(),mediaPaths.end(),path) );	
}


//后端插入,前端取出
void MyLib::PushPlayQueue(_songContainerItem item)
{
	playQueue.push_back(item);
}

_songContainerItem MyLib::PopTrackFromPlayQueue()
{
	PlayQueueContainer::iterator i=playQueue.begin();
	_songContainerItem item=*i;
	playQueue.erase(i);

	return item;
}

vector<int> MyLib::GetIndexInPlayQueue(_songContainerItem item)
{
	vector<int> v;
	int idx=0;
	for(PlayQueueContainer::iterator i=playQueue.begin();
		i!=playQueue.end();++i,++idx)
		if( *i == item)
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
	auto i= find(m_playLists.begin(),m_playLists.end(),pl);
	if(i!=m_playLists.end())
	{	
		m_playLists.erase(i);
		delete pl;
	}
	//todo 
	//if pl == active pl
	//active = select pl
}

void MyLib::DeletePlayList(int nIndex)
{
	DeletePlayList(Index2Playlist(nIndex));
}