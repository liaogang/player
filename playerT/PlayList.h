#pragma once
#include "MySerialize.h"
#include <iostream>
#include <fstream>
#include <string>
#include <list>

//for parse the ID3 tag
#include <direct.h>
#include <stdlib.h>
#include <tbytevector.h>
#include <mpegfile.h>
#include <id3v2tag.h>
#include <id3v2frame.h>
#include <id3v2header.h>
#include <id3v1tag.h>
#include <apetag.h>
#include <taglib.h>
#include <fileref.h>
#include <tbytevector.h>
using namespace TagLib;



#define PLAYORDERCOUNT 7
static const TCHAR *gPlayOrderStr[PLAYORDERCOUNT] =
{
	_T("Default"),
	_T("Repeat (playlist)"),
	_T("Repeat (track)"),
	_T("Random"),
	_T("Shuffle (tracks)"),
	_T("Shuffle (albums)"),
	_T("Shuffle (folders)"),
};

enum PlayOrderIndex
{
	Default,
	Repeat_playlist,
	Repeat_track,
	Random,
	Shuffle_tracks,
	Shuffle_albums,
	Shuffle_folders,
};

class PlayListItem:
	public SerializeObj
{
public:
	virtual int SerializeB(FILE* pFile);
	virtual int ReSerialize(FILE* pFile);
public:
	PlayListItem():playCount(0),starLvl(1),indexInListView(-1)
		,pPicBuf(NULL),img(NULL),year(0)
	{
		//title(NULL)
		//,artist(NULL),album(0),year(0),genre(0),comment(0)
	}

	PlayListItem(std::tstring *url):url(*url)
		,playCount(0),starLvl(1),indexInListView(-1)
		,pPicBuf(NULL),img(NULL),year(0)
	{
		//title(NULL)
		//,artist(NULL),album(0),year(0),genre(0),comment(0)
	}

	~PlayListItem();

public:
	int indexInListView;
	std::tstring url;
	UINT          playCount;
	UINT		  starLvl;
	//-----id3 info-------
	std::tstring  title,artist,album,genre,comment,lyric;
	BOOL bUnsynLyc;
	UINT year;
	TagLib::ByteVector *pPicBuf;
	CImage *img;
public:
	BOOL ScanId3Info();
	const TCHAR* GetTitle(){return title.c_str();}
	BOOL  GetLrcFileFromLib();
private:
	BOOL LrcFileMacth(std::tstring &filename);
};


typedef PlayListItem MyTrack;


class PlayList:
	public SerializeObj
{
public:
	virtual int SerializeB(FILE* pFile);
	virtual int ReSerialize(FILE* pFile);
public:
	list<PlayListItem> m_songList;
	std::tstring       m_playlistName;
private:
	PlayListItem *lastPlayingItem,*curPlayingItem,
		*nextPlayingItem,*curSelectedItem;
	
public:
	void SetCurPlaying(PlayListItem* item,BOOL scanID3=TRUE);
	PlayListItem* lastTrack(){return lastPlayingItem;}
	PlayListItem* curTrack(){return curPlayingItem;};
	PlayListItem* nextTrack();
public:
	PlayList(void);
	PlayList(std::tstring name);
	~PlayList(void);
	
public:
	BOOL AddFolderByThread(LPCTSTR pszFolder);
	void scanAllId3Info();
	BOOL AddFolder(LPCTSTR pszFolder);
public:
	PlayOrderIndex index;
	void SetPlayOrder(enum PlayOrderIndex index){this->index=index;}
	PlayListItem* GetNextTrackByOrder(BOOL bMoveCur=TRUE);
};