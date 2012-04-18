#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <list>
//#include <mpg123.h>
using namespace std;

//
#define SONG 

#ifndef _PLAYLIST_H_
#define _PLAYLIST_H_

class PlayListItem
{
public:
	PlayListItem(tstring *url):url(*url)
		,playCount(0),starLvl(1)
	{
		//title(NULL)
		//,artist(NULL),album(0),year(0),genre(0),comment(0)

	}
public:
	tstring url;
	UINT          playCount;
	UINT		  starLvl;
	//LRC*        lrc;

	//-----id3 info-------
	tstring  title,artist,album,genre,comment;
	UINT year;
public:
	BOOL scanId3Info();
};


class PlayList
{
public:
	list<PlayListItem> m_songList;
	//list<tstring> m_songList;
public:
	PlayList(void);
	~PlayList(void);
public:
	static void AddFolderToCurrentPlayList(LPCTSTR pszFolder);
	BOOL AddFolder(LPCTSTR pszFolder);
	void scanAllId3Info();
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



#endif