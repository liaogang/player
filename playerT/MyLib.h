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
	//
public:
	MyLib():m_pActivePlaylist(NULL),m_pSelPlaylist(NULL)
		,playorder(Default)
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
	typedef list<PlayList*> PLList;
	PLList m_playLists;
private:
	//pointers
	PlayList*      m_pActivePlaylist;  //playing
	PlayList*      m_pSelPlaylist;//operating
public:
	void SetSelPlaylist(PlayList* p){m_pSelPlaylist=p;}
	PlayList* SelPlaylist(){return m_pSelPlaylist;}
	void SetActivePlaylist(PlayList* p){m_pActivePlaylist=p;}
	PlayList* ActivePlaylist(){return m_pActivePlaylist;}
	PlayList* NewPlaylist(std::tstring playlistname=_T("新建播放列表"));

	static PlayList* AddFolderToCurrentPlayList(LPCTSTR pszFolder);
	static void playAfterSlowDown();
	static void play();//set active track, play 
	static void pause();
	static void stop();
	static void playNext(BOOL scanID3=TRUE);

	void DeletePlayList(PlayList *pl);

	static	BOOL SavePlaylist(PlayList *pl,LPTSTR filepath);
	PlayList* LoadPlaylist(LPTSTR filepath);

	#define CFGFILENAME _T("coredata")

	BOOL LoadCoreCfg();
	BOOL SaveCoreCfg();

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