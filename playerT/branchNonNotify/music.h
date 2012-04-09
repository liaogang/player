
#pragma once

//////////////////////////////////////////////////////////////////////////
//struct//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
enum MUSIC_TYPE
{
	type_MP3=0,
	type_WAV=1,
	type_OTHER=2
};
struct TAGINFO
{

};

struct MUSIC_INFO
{
	MUSIC_TYPE type;
//	tstring filename;
//	tstring path;
	TAGINFO tag;
};

//////////////////////////////////////////////////////////////////////////
class music
{
public:
	music(void);
public:
	~music(void);
private:
	MUSIC_INFO m_musicInfo;
};



class MusicLibrary
{
public:
	MusicLibrary(void);
public:
	~MusicLibrary(void);
};


class playlist
{
public:
	playlist(void);
public:
	~playlist(void);
};
