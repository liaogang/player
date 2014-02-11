#include "stdafx.h"
#include "MyLib.h"
#include "fileMonitor.h"
#include "BasicPlayer.h"
#include "globalStuffs.h"
#include "forwardMsg.h"
#include <algorithm>
#include <time.h>


void MyLib::SetSelectedIndex(int i)
{
	if(m_IndexSelecting!=i)
	{
		m_IndexSelecting=i;
		NotifyMsg(WM_SELECTED_PL_CHANGED,FALSE,(WPARAM)GetItem(i),(LPARAM)i);
	}
}

LPCPlayList  MyLib::NewPlaylist(BOOL bNotify,std::tstring playlistname,bool bAutoPL)
{
	LPCPlayList l=new CPlayList(playlistname);
	m_playLists.push_back(l);

	if(bAutoPL)
		l->SetAuto(bAutoPL);
	
	int nIndex=GetItemCount()-1;

	SetSelectedIndex(nIndex);

	if(bNotify)
		NotifyMsg(WM_SOME_PL_CHANGED,FALSE,0,0);


	return l;
}

LPCPlayList MyLib::AddFolderToCurrentPlayList(LPCTSTR pszFolder)
{
	LPCPlayList p=MyLib::shared()->GetSelectedPL();
	p->AddFolderByThread(pszFolder);
	return p;
}

static MyLib gMylib;
MyLib* MyLib::shared()
{
	return &gMylib;
}

MyLib::~MyLib()
{
	stop();

	PLList::iterator i;
	for (i=m_playLists.begin();i!=m_playLists.end();i++)
		delete *i;
}



//-----------------------------------------



BOOL MyLib::play(BOOL bClearPlayQueue)
{
	stop();

	if(bClearPlayQueue)
		ClearPlayQueue();

	if(itemToPlay==NULL)
	{
		LPCPlayList pl=GetSelectedPL();
		if(pl)
			itemToPlay = pl->GetSelectedItem();
	}


	if( itemToPlay!=NULL && itemToPlay->isValide() && itemToPlay->IsFileExist() && itemToPlay->ScanId3Info(TRUE,TRUE))
	{
		CBasicPlayer* s=CBasicPlayer::shared();

		if(!s->stoped())
			s->stop();

		if(s->open(itemToPlay->GetUrl().c_str()))
		{
			itemPlaying=itemToPlay;
			//itemToPlay=NULL;
			s->play();
			return TRUE;
		}
	}

	return FALSE;
}

BOOL MyLib::playNext(BOOL scanID3 )
{
	auto playorder=MyLib::shared()->GetPlayOrder();
	return playNext(scanID3,playorder,FALSE);
}


BOOL MyLib::playRandomNext(BOOL scanID3)
{
	return playNext(scanID3,Random,TRUE);
}

BOOL MyLib::playNext(BOOL scanID3 ,PlayOrder playorder ,BOOL bClearPlayQueue)
{	
	if (!playQueue.empty())
	{
		itemToPlay = PopTrackFromPlayQueue();
		return play(FALSE);
	}

	if(itemPlaying !=NULL && itemPlaying->isValide() && IsValidePlayList(itemPlaying->GetPlayList() ) )
	{
		itemToPlay=itemPlaying->GetPlayList()->GetNextTrackByOrder(itemPlaying->GetIndex(),playorder);
		if(itemToPlay==NULL)
			return FALSE;
		else
			return play(FALSE);
	}
	

	return FALSE;
}



static LPCPlayList pPlayListAuto=NULL;
LPCPlayList MyLib::GetAutoPlaylist()
{
	if (!pPlayListAuto)
	{
		pPlayListAuto=NewPlaylist(TRUE,_T("自动播放列表"),true);
		m_pFileMonitor=new fileMonitors(pPlayListAuto);
	}
	
	return pPlayListAuto;
}


void MyLib::SetAutoPlaylist(LPCPlayList pl)
{
	pPlayListAuto=pl;
}


void MyLib::InitMonitor(LPCPlayList pl)
{
	pPlayListAuto=pl;
	m_pFileMonitor=new fileMonitors(pPlayListAuto);
}

void MyLib::SetMediaPathCount(int count)
{
	mediaPaths.resize(count);	
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
void MyLib::PushPlayQueue(LPCPlayListItem item)
{
	playQueue.push_back(item);
	NotifyMsg(WM_PLAYQUEUE_CHANGED,FALSE,0,0);
}

LPCPlayListItem MyLib::PopTrackFromPlayQueue()
{
	PlayQueueContainer::iterator i=playQueue.begin();
	LPCPlayListItem item=*i;
	playQueue.erase(i);
	NotifyMsg(WM_PLAYQUEUE_CHANGED,FALSE,0,0);
	return item;
}

vector<int> MyLib::GetIndexInPlayQueue(LPCPlayListItem item)
{
	vector<int> v;
	int idx=0;
	for(PlayQueueContainer::iterator i=playQueue.begin();
		i!=playQueue.end();++i,++idx)
		if( *i == item)
			v.push_back(idx);
	
	return v;
}

void MyLib::DeleteFromPlayQueue(LPCPlayListItem item)
{
	for(PlayQueueContainer::iterator i=playQueue.begin();
		i!=playQueue.end();)
	{
		PlayQueueContainer::iterator k=i;
		++i;

		if(*k == item)
		{
			playQueue.erase(k);
			break;
		}
	}

	NotifyMsg(WM_PLAYQUEUE_CHANGED,FALSE,0,0);
}

void MyLib::ClearPlayQueue()
{
	playQueue.clear();
	NotifyMsg(WM_PLAYQUEUE_CHANGED,FALSE,0,0);
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



void MyLib::DeletePlayList(LPCPlayList pl,BOOL bNotify)
{
	int nIndex= Playlist2Index(pl);
	if(nIndex != -1)
		DeletePlayList(nIndex,bNotify);


}

void MyLib::DeletePlayList(int nIndex,BOOL bNotify)
{
	LPCPlayList pl=GetItem(nIndex);

	if(nIndex == GetSelectedIndex())
	{
		if(nIndex ==GetItemCount() -1)
			m_IndexSelecting=nIndex-1;

		NotifyMsg(WM_SELECTED_PL_CHANGED,FALSE,(WPARAM)GetItem(m_IndexSelecting),(LPARAM)m_IndexSelecting);
	}

	
	if(pl->IsAuto())
	{
		delete m_pFileMonitor;
		m_pFileMonitor=NULL;
		SetAutoPlaylist(NULL);
	}


	m_playLists.erase(m_playLists.begin()+nIndex);
	NotifyMsg(WM_PL_WILL_DELETED,FALSE,(WPARAM)pl,0);
	
	delete pl;

	if(bNotify)
		NotifyMsg(WM_SOME_PL_CHANGED,FALSE,0,0);
}









void MyLib::pause()
{
	CBasicPlayer::shared()->pause();
}

void MyLib::stop()
{
#ifdef APP_PLAYER_UI
	if(itemPlaying)
		MyLib::shared()->GetPlayingItem()->ClearImgBuf();
#endif

	CBasicPlayer::shared()->stop();
}

bool MyLib::isPlaying() const
{
	CBasicPlayer *p=CBasicPlayer::shared();
	int a=p->stoped();
	return !p->stoped();
}