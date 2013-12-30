#include "MyLib.h"
#include "BasicPlayer.h"
#include "PlayListViewMng.h"
#include "globalStuffs.h"
//-----------------------------------------
//

PlayList* SelectedPlaylist()
{
	return MyLib::shared()->GetSelectedPL();
}

void SetSelectedPlaylist(PlayList* pl)
{
	MyLib::shared()->SetSelectedIndex(pl);
}

PlayList* PlayingPlaylist()
{
	return MyLib::shared()->GetPlayingPL();
}

void SetPlayingPlaylist(PlayList* pl)
{
	MyLib::shared()->SetPlayingIndex(pl);
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
	PlayList *p=SelectedPlaylist();
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



//-----------------------------------------

BOOL MyLib::play(PlayListItem *item)
{	
	if(item->isValide() && item->IsFileExist())
	{
		PlayList *pl=item->GetPlayList();
		SetPlayingIndex(pl);
		pl->SetPlayingIndex(item->GetIndex());
		return play(item->GetFileTrack());
	}
	return FALSE;
}


BOOL MyLib::play()
{
	ClearPlayQueue();

	PlayList *pl=GetSelectedPL();
	return pl?play(pl->GetSelectedItem()):FALSE;
}

void MyLib::playNext(BOOL scanID3)
{	
	if (!playQueue.empty())
	{
		play(PopTrackFromPlayQueue());
		return;
	}

	PlayListItem *nextItem;
	PlayListItem *currItem=NULL;

	PlayList *pl=GetPlayingPL();
	if(pl)
		currItem=pl->GetItem(pl->GetPlayingIndex());
	
	if(!currItem || !currItem->isValide())
	{
		PlayList *spl=SelectedPlaylist();
		if(spl && spl->GetItemCount()>0)
		{	
			int np=spl->GetPlayingIndex();
			if(np==-1)
				np=spl->GetSelectedIndex();
			if(np==-1)
				np=0;

			currItem=spl->GetItem(np);
		}
	}


	if(!currItem || !currItem->isValide())
		return;

	PlayList *pPl=currItem->GetPlayList();

		if(IsValidePlayList(pPl))
			nextItem=pPl->GetNextTrackByOrder();
		else
			return;
	
	if(nextItem)
		play(nextItem);
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


void MyLib::SetAutoPlaylist(PlayList *pl)
{
	pPlayListAuto=pl;
}


void MyLib::InitMonitor(PlayList *pl)
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
		if( *(*i) == *item)
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

		if(*(*k) == *item)
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
		if(pl->m_bAuto)
		{
			delete m_pFileMonitor;
			m_pFileMonitor=NULL;
			SetAutoPlaylist(NULL);
		}
	}

}

void MyLib::DeletePlayList(int nIndex)
{
	DeletePlayList(Index2Playlist(nIndex));
}