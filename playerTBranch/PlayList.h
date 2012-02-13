#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <list>

using namespace std;

//
#define SONG 
class PlayList
{
public:
	list<tstring> m_songList;
public:
	PlayList(void);
	~PlayList(void);
public:
	static void AddFolderToCurrentPlayList(LPCTSTR pszFolder);
	BOOL AddFolder(LPCTSTR pszFolder);

};


class MyLib
{
public:
	MyLib(){};
	~MyLib(){};
public:
	//playlist 
	static PlayList& GetPlayListObj();

	//cur play playlist
	//cur operator playlist
};
