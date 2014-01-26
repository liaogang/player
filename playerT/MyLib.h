#include "PlayList.h"
#include "math.h"
#pragma once



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


	//-----------------------------------------



public:
	MyLib():playorder(Default),m_pFileMonitor(NULL),m_IndexPlaying(-1),m_IndexSelecting(-1)
	{
	};

	~MyLib();
	static MyLib* shared();

public:
	//data member
	typedef  PlayList*  PLListItem;
	typedef vector<PLListItem> PLList;
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
		return nullptr;
	}


	//自动播放列表,媒体库专用.
	void InitMonitor(PlayList *pl);
	PlayList* GetAutoPlaylist();
	void SetAutoPlaylist(PlayList *pl);

	list<std::tstring> mediaPaths;
	void ClearMediaPath();
	void AddMediaPath(std::tstring &path);
	void DelMediaPath(std::tstring &path);
	fileMonitors * m_pFileMonitor;


	/////////////////////////////////////
private:
	int m_IndexPlaying;
	int m_IndexSelecting;
public:

	void SetPlayingIndex(int i){m_IndexPlaying=i;}
	void SetPlayingIndex(PlayList *pl){SetPlayingIndex(Playlist2Index(pl));}
	int  GetPlayingIndex(){return m_IndexPlaying;}
	PLListItem GetPlayingPL(){return Index2Playlist(GetPlayingIndex());}	

	void SetSelectedIndex(int i){m_IndexSelecting=i;}
	void SetSelectedPL(PlayList *pl){SetSelectedIndex(Playlist2Index(pl));}
	int  GetSelectedIndex(){return m_IndexSelecting;}
	PLListItem GetSelectedPL(){return Index2Playlist(GetSelectedIndex());}

	bool IsValidePlayList(PlayList *pPl){return Playlist2Index(pPl) < m_playLists.size() ;}

	//播放列队
	typedef list<_songContainerItem> PlayQueueContainer;
	PlayQueueContainer playQueue;
	void PushPlayQueue(_songContainerItem item);
	_songContainerItem PopTrackFromPlayQueue();

	//to test track weather in the queue
	//use return value 's empty funtion
	vector<int> GetIndexInPlayQueue(_songContainerItem item);
 
	void DeleteFromPlayQueue(_songContainerItem item);
	void ClearPlayQueue();


public:
	PlayList* NewPlaylist(std::tstring playlistname=_T("新建播放列表1"),bool bAutoPL=false);
	void DeletePlayList(PlayList *pl);
	void DeletePlayList(int nIndex);


	bool isPlaying();

	static PlayList* AddFolderToCurrentPlayList(LPCTSTR pszFolder);
	BOOL play();
	BOOL play(PlayListItem *item);//set active track, play 
private:
	BOOL play(FileTrack  *item);//set active track, play 
public:
	static void pause();
	static void stop();
	void playNext(BOOL scanID3=TRUE);
	

	
	static	BOOL SavePlaylist(PlayList *pl,LPTSTR filepath);
	PlayList* LoadPlaylist(LPTSTR filepath,TCHAR* PlName=NULL);


	
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

	template<class _InIt> inline
		_InIt GetNextByOrder(_InIt _beg , _InIt _cur ,_InIt _end)
	{
		_InIt next;

		if (playorder==Default)
		{
			next=++_cur;
		}
		else if (playorder==Repeat_playlist)
		{
			next=++_cur;
			if(next==_end)
				next=_beg;
		}
		else if (playorder==Repeat_track)
		{
			next=_cur;
		}
		else if (playorder==Random)
		{
			int s=0;
			if(s==3)
			{
				srand(time(NULL));
				s=0;
			}

			++s;

			int n=rand() % (_end - _beg) - 1;
			next=_beg + n;
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

	PlayOrder playorder;
	void SetPlayOrder(enum PlayOrder index){playorder=index;}
};



PlayList* PlayingPlaylist();
PlayList* SelectedPlaylist();
void SetPlayingPlaylist(PlayList*);
void SetSelectedPlaylist(PlayList*);
