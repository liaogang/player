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
	list<PlayList> m_playLists;
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

	static void AddFolderToCurrentPlayList(LPCTSTR pszFolder);
	static void playAfterSlowDown();
	static void play();//set active track, play 
	static void pause();
	static void stop();
	static void playNext(BOOL scanID3=TRUE);

	BOOL SaveCurPlaylist(LPTSTR filepath);
	PlayList* LoadPlaylist(LPTSTR filepath);

	#define CFGFILENAME _T("coredata")

	BOOL LoadCoreCfg();
	BOOL SaveCoreCfg();
};