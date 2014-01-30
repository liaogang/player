#pragma once
#include "stdafx.h"
#include "PlayList.h"
#include "math.h"
#include <list>
#include <vector>
class fileMonitors;
typedef list<LPCPlayListItem> PlayQueueContainer;
typedef vector<LPCPlayList> PLList;

//count=sizeof(gPlayOrderStr)/sizeof(gPlayOrderStr[0])
static const TCHAR *gPlayOrderStr[] =
{
	_T("Default"),
	_T("Repeat (playlist)"),
	_T("Repeat (track)"),
	_T("Random"),
	_T("Shuffle (tracks)"),
	_T("Shuffle (albums)"),
	_T("Shuffle (folders)"),
};





class MyLib
{
public:
	MyLib():playorder(Default),m_pFileMonitor(NULL),m_IndexSelecting(-1),itemPlaying(NULL)
	{
	};

	~MyLib();
	static MyLib* shared();
private:
	PLList m_playLists;
	PlayQueueContainer playQueue;
	vector<std::tstring> lrcDirs;//歌词搜索路径表
	vector<std::tstring> dataPaths;

	list<std::tstring> mediaPaths;
	fileMonitors * m_pFileMonitor;
	PlayOrder playorder;

	int m_IndexSelecting; // the playlist selected
	LPCPlayListItem itemToPlay; // the playlistitem to play
	LPCPlayListItem itemPlaying;
public:
	void SetSelectedIndex(int i){m_IndexSelecting=i;}
	void SetSelectedPL(LPCPlayList pl){SetSelectedIndex(Playlist2Index(pl));}
	void SetAutoPlaylist(LPCPlayList pl);
	void SetPlayOrder(int o){playorder=static_cast<PlayOrder>(o);}
	void SetItemToPlay(LPCPlayListItem item){itemToPlay=item;}

	int  GetSelectedIndex() const {return m_IndexSelecting;}
	LPCPlayList GetSelectedPL() const {return Index2Playlist(GetSelectedIndex());}
	int  GetPlayingIndex() const {return Playlist2Index(GetPlayingPL());}
	LPCPlayList GetPlayingPL() const {return itemPlaying->GetPlayList();}
	LPCPlayListItem GetPlayingItem(){return itemPlaying;}
	LPCPlayList GetAutoPlaylist();
	vector<std::tstring>& GetDataPaths(){return dataPaths;};
	vector<std::tstring>& GetLrcDirs(){return lrcDirs;};
	list<std::tstring>& GetMediaPaths(){return mediaPaths;}
	PlayOrder GetPlayOrder() const {return playorder;}

	void ClearLrcSearchLib()
	{
		lrcDirs.clear();
	}

	void AddFolder2LrcSearchLib(LPCTSTR pszFolder)
	{
		lrcDirs.push_back(pszFolder);
	}

	static void ImportLycByPath(std::tstring path);
	void MyLib::InitLrcLib();

	int  Playlist2Index(LPCPlayList pl) const 
	{
		for (int index=0; index < GetItemCount(); ++index)
		{
			if ( m_playLists[index] == pl)
				return index;
		}

		return -1;
	}

	BOOL Empty() const
	{
		return m_playLists.empty();
	}

	INT GetItemCount() const
	{
		return m_playLists.size();
	}

	LPCPlayList GetItem(int index) const
	{
		if(index < m_playLists.size() && index>= 0 )
			return m_playLists[index];
		return NULL;
	}

	LPCPlayList Index2Playlist(int index) const
	{
		return GetItem(index);
	}

	//自动播放列表,媒体库专用.
	void InitMonitor(LPCPlayList pl);
	void SetMediaPathCount(int count);
	void ClearMediaPath();
	void AddMediaPath(std::tstring &path);
	void DelMediaPath(std::tstring &path);

	//播放列队
	void PushPlayQueue(LPCPlayListItem item);
	LPCPlayListItem PopTrackFromPlayQueue();
	//to test track weather in the queue
	//use return value 's empty funtion
	vector<int> GetIndexInPlayQueue(LPCPlayListItem item);
	void DeleteFromPlayQueue(LPCPlayListItem item);
	void ClearPlayQueue();

	//test if is deleted
	bool IsValidePlayList(LPCPlayList pPl) const 
	{
		int index=Playlist2Index(pPl) ;
		return index >=0 && index < GetItemCount();
	}

	LPCPlayList NewPlaylist(std::tstring playlistname=_T("新建播放列表1"),bool bAutoPL=false);
	void DeletePlayList(LPCPlayList pl);
	void DeletePlayList(int nIndex);

	bool isPlaying() const;

	static LPCPlayList AddFolderToCurrentPlayList(LPCTSTR pszFolder);
	BOOL play();
	void pause();
	void stop();
	BOOL playNext(BOOL scanID3=TRUE);
	BOOL playNext(BOOL scanID3 ,PlayOrder playorder);
	BOOL playRandomNext(BOOL scanID3=TRUE);

	static	BOOL SavePlaylist(LPCPlayList pl,LPTSTR filepath);
	LPCPlayList LoadPlaylist(LPTSTR filepath,TCHAR* PlName=NULL);
	void SetPlayOrder(enum PlayOrder index){playorder=index;}
};


