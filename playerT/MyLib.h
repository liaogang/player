#include "PlayList.h"
#pragma once


class MyLib:
	public SerializeObj
{
	//-----------------------------------------
	//
public:
	vector<std::tstring> lrcDirs;//歌词搜索路径表
	vector<std::tstring> dataPaths;

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

public:
	virtual int SerializeB(FILE* pFile);
	virtual int ReSerialize(FILE* pFile);

	//-----------------------------------------



public:
	MyLib():playorder(Default)
	{
	};

	~MyLib();
	static MyLib* shared();
public:
	HWND hMain;
	static void SetMain(HWND hMain);
	static HWND GetMain();

public:
	//data member
	typedef vector<PlayList*> PLList;
	PLList m_playLists;
	int  Playlist2Index(PlayList *pl)
	{
		int index=0;
		for (auto i=m_playLists.begin();
			i!=m_playLists.end();++i,++index)
		{
			if (*i == pl)
				break;
		}

		return index;
	}

	PlayList * Index2Playlist(int index)
	{
		PlayList *pl=NULL;
		if(index < m_playLists.size() && index>= 0 )
			return m_playLists[index];
	}


	//自动播放列表,媒体库专用.
	PlayList* GetAutoPlaylist();
	vector<std::tstring> mediaPaths;
	void ClearMediaPath();
	void AddMediaPath(std::tstring &path);
	void DelMediaPath(std::tstring &path);

	//播放列队
	typedef list<_songContainerItem> PlayQueueContainer;
	PlayQueueContainer playQueue;
	void PushPlayQueue(_songContainerItem item);
	PlayListItem PopTrackFromPlayQueue();

	//to test track weather in the queue
	//use return value 's empty funtion
	vector<int> GetIndexInPlayQueue(_songContainerItem item);
 
	void DeleteFromPlayQueue(_songContainerItem item);
	void ClearPlayQueue();


public:
	_songContainerItem lastPlayingItem;
	_songContainerItem nextPlayingItem;
	_songContainerItem curSelectedItem;
private:
	_songContainerItem WaitPlayItem;
public:
	void SetWaitPlayItem(_songContainerItem item)
	{
		WaitPlayItem=item;
	}

	_songContainerItem *GetWaitPlayItem(){
		return &WaitPlayItem;}
public:
	PlayList* NewPlaylist(std::tstring playlistname=_T("新建播放列表1"),bool bAutoPL=false);
	void DeletePlayList(PlayList *pl);

	bool isPlaying();

	static PlayList* AddFolderToCurrentPlayList(LPCTSTR pszFolder);
	void playAfterSlowDown(FileTrack * item);
	void play(FileTrack  *item);//set active track, play 
	static void pause();
	static void stop();
	void playNext(BOOL scanID3=TRUE);
	

	
	static	BOOL SavePlaylist(PlayList *pl,LPTSTR filepath);
	PlayList* LoadPlaylist(LPTSTR filepath);


	//-----------------------------------------
	enum PlayOrder
	{
		Default,
		Repeat_playlist,
		Repeat_track,
		Random,
		Shuffle_tracks,
		Shuffle_albums,
		Shuffle_folders,
	};
	PlayOrder playorder;
	void SetPlayOrder(enum PlayOrder index){playorder=index;}

	template<class _InIt> inline
	_InIt GetNextByOrder(_InIt _cur)
	{
		_InIt next;

		if (playorder==Default)
		{
			next=++_cur;
		}
		else if (playorder==Repeat_playlist)
		{
			next=++(++_cur);
		}
		else if (playorder==Repeat_track)
		{
			next=_cur;
		}
		else if (playorder==Random)
		{
			next=++_cur;
		}
		else if (playorder==Shuffle_tracks)
		{
			next=++_cur;
		}
		else if (playorder==Shuffle_albums)
		{
			next=++_cur;
		}
		else if (playorder==Shuffle_folders)
		{
			next=++_cur;
		}

		return next;
	}
};

//count=sizeof(gPlayOrderStr)/sizeof(int)
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