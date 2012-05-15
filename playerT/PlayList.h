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

	~PlayListItem()
	{
		if (img)
		{
			delete img;
			img=NULL;
		}
		if (pPicBuf)
		{	
			delete pPicBuf;
			pPicBuf=NULL;
		}
	}

public:
	int indexInListView;

	std::tstring url;
	UINT          playCount;
	UINT		  starLvl;
	//LRC*        lrc;

	//-----id3 info-------
	std::tstring  title,artist,album,genre,comment;
	UINT year;
	
	TagLib::ByteVector *pPicBuf;
	CImage *img;
public:
	BOOL ScanId3Info();
	const TCHAR* GetTitle(){return title.c_str();}
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
	void SetCurPlaying(PlayListItem* item,BOOL scanID3=TRUE)
	{
		curPlayingItem=item;
		if (scanID3)
			curPlayingItem->ScanId3Info();
	}

	PlayListItem* lastTrack(){return lastPlayingItem;}
	PlayListItem* curTrack(){return curPlayingItem;};
	PlayListItem* nextTrack()
	{
		PlayListItem*tmp=nextPlayingItem; 
		nextPlayingItem=NULL;
		return tmp;
	}
	
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
	void SetPlayOrder(enum PlayOrderIndex index)
	{
		this->index=index;
	}


	PlayListItem* GetNextTrackByOrder(BOOL bMoveCur=TRUE)
	{
		list<PlayListItem>::iterator cur,next;

		for (cur=m_songList.begin();cur!=m_songList.end();cur++)
		{
			if (&(*cur)==curPlayingItem)
				break;
		}

		lastPlayingItem=curPlayingItem;

		if (++cur==m_songList.end())
			return NULL;
		
		
		if (index==Default)
		{
			next=cur;
		}
		if (index==Repeat_playlist)
		{
			next;
		}
		if (index==Random)
		{
			next;
		}
		if (index==Repeat_track)
		{
		}

		if(bMoveCur)curPlayingItem=&(*next);
		return &(*next);
	}
};


class MyLib:
	public SerializeObj
{
public:
	virtual int SerializeB(FILE* pFile);
	virtual int ReSerialize(FILE* pFile);
public:
	MyLib():m_pActivePlaylist(NULL),m_pSelPlaylist(NULL)
	{
	};

	~MyLib()
	{
	};

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
public:
	PlayList* NewPlaylist(std::tstring playlistname=_T("新建播放列表"));
	static void AddFolderToCurrentPlayList(LPCTSTR pszFolder);
	static void play();//set active track, play 
	static void pause();
	static void stop();
	static void playNext(BOOL scanID3=TRUE);
public:
	BOOL SaveCurPlaylist(LPTSTR filepath);
	BOOL LoadPlaylist(LPTSTR filepath);
};





#ifdef asdfsdfoi


#define ID3_NR_OF_V1_GENRES 148
static const char *ID3_v1_genre_description[ID3_NR_OF_V1_GENRES] =
{
	"Blues",             //0
	"Classic Rock",      //1
	"Country",           //2
	"Dance",             //3
	"Disco",             //4
	"Funk",              //5
	"Grunge",            //6
	"Hip-Hop",           //7
	"Jazz",              //8
	"Metal",             //9
	"New Age",           //10
	"Oldies",            //11
	"Other",             //12
	"Pop",               //13
	"R&B",               //14
	"Rap",               //15
	"Reggae",            //16
	"Rock",              //17
	"Techno",            //18
	"Industrial",        //19
	"Alternative",       //20
	"Ska",               //21
	"Death Metal",       //22
	"Pranks",            //23
	"Soundtrack",        //24
	"Euro-Techno",       //25
	"Ambient",           //26
	"Trip-Hop",          //27
	"Vocal",             //28
	"Jazz+Funk",         //29
	"Fusion",            //30
	"Trance",            //31
	"Classical",         //32
	"Instrumental",      //33
	"Acid",              //34
	"House",             //35
	"Game",              //36
	"Sound Clip",        //37
	"Gospel",            //38
	"Noise",             //39
	"AlternRock",        //40
	"Bass",              //41
	"Soul",              //42
	"Punk",              //43
	"Space",             //44
	"Meditative",        //45
	"Instrumental Pop",  //46
	"Instrumental Rock", //47
	"Ethnic",            //48
	"Gothic",            //49
	"Darkwave",          //50
	"Techno-Industrial", //51
	"Electronic",        //52
	"Pop-Folk",          //53
	"Eurodance",         //54
	"Dream",             //55
	"Southern Rock",     //56
	"Comedy",            //57
	"Cult",              //58
	"Gangsta",           //59
	"Top 40",            //60
	"Christian Rap",     //61
	"Pop/Funk",          //62
	"Jungle",            //63
	"Native American",   //64
	"Cabaret",           //65
	"New Wave",          //66
	"Psychadelic",       //67
	"Rave",              //68
	"Showtunes",         //69
	"Trailer",           //70
	"Lo-Fi",             //71
	"Tribal",            //72
	"Acid Punk",         //73
	"Acid Jazz",         //74
	"Polka",             //75
	"Retro",             //76
	"Musical",           //77
	"Rock & Roll",       //78
	"Hard Rock",         //79
	// following are winamp extentions
	"Folk",                  //80
	"Folk-Rock",             //81
	"National Folk",         //82
	"Swing",                 //83
	"Fast Fusion",           //84
	"Bebob",                 //85
	"Latin",                 //86
	"Revival",               //87
	"Celtic",                //88
	"Bluegrass",             //89
	"Avantgarde",            //90
	"Gothic Rock",           //91
	"Progressive Rock",      //92
	"Psychedelic Rock",      //93
	"Symphonic Rock",        //94
	"Slow Rock",             //95
	"Big Band",              //96
	"Chorus",                //97
	"Easy Listening",        //98
	"Acoustic",              //99
	"Humour",                //100
	"Speech",                //101
	"Chanson",               //102
	"Opera",                 //103
	"Chamber Music",         //104
	"Sonata",                //105
	"Symphony",              //106
	"Booty Bass",            //107
	"Primus",                //108
	"Porn Groove",           //109
	"Satire",                //110
	"Slow Jam",              //111
	"Club",                  //112
	"Tango",                 //113
	"Samba",                 //114
	"Folklore",              //115
	"Ballad",                //116
	"Power Ballad",          //117
	"Rhythmic Soul",         //118
	"Freestyle",             //119
	"Duet",                  //120
	"Punk Rock",             //121
	"Drum Solo",             //122
	"A capella",             //123
	"Euro-House",            //124
	"Dance Hall",            //125
	"Goa",                   //126
	"Drum & Bass",           //127
	"Club-House",            //128
	"Hardcore",              //129
	"Terror",                //130
	"Indie",                 //131
	"Britpop",               //132
	"Negerpunk",             //133
	"Polsk Punk",            //134
	"Beat",                  //135
	"Christian Gangsta Rap", //136
	"Heavy Metal",           //137
	"Black Metal",           //138
	"Crossover",             //139
	"Contemporary Christian",//140
	"Christian Rock ",       //141
	"Merengue",              //142
	"Salsa",                 //143
	"Trash Metal",           //144
	"Anime",                 //145
	"JPop",                  //146
	"Synthpop"               //147
};
#endif










