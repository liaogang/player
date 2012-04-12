#include "StdAfx.h"
#include "PlayList.h"
#include "MusicFile.h"
#include "WaveFileEx.h"
#include "Mp3File.h"
#include <direct.h>
#include <id3.h>




TCHAR* Ansi2Unicode(char *s)
{
	DWORD dwNum= MultiByteToWideChar (CP_ACP, 0,(LPCSTR) s, -1, NULL, 0);
	TCHAR *target=new TCHAR[dwNum];
	MultiByteToWideChar(CP_ACP,0,(LPCSTR)s,-1,target,dwNum);
	return target;
}

TCHAR* UTF82Unicode(char *s)
{
	DWORD dwNum = MultiByteToWideChar (CP_UTF8, 0, s, -1, NULL, 0);
	TCHAR* target=new TCHAR[dwNum];
	MultiByteToWideChar(CP_UTF8,0,(LPCSTR)s,-1,target,dwNum);
	return target;
}


PlayList& MyLib::GetPlayListObj()
{
	static PlayList playList_;
	return playList_;
}



PlayList::PlayList(void)
{
}

PlayList::~PlayList(void)
{
}

void PlayList::AddFolderToCurrentPlayList(LPCTSTR pszFolder)
{
	MyLib::GetPlayListObj().AddFolder(pszFolder);
}

void PlayList::scanAllId3Info()
{

}

BOOL PlayList::AddFolder(LPCTSTR pszFolder)
{
	// for each mp3 file in this folder
	// data . add

	//忽略了子目录下的mp3文件

	//改变当前目录
	_tchdir(pszFolder);

	WIN32_FIND_DATA findFileData;
	HANDLE hFind;

	hFind=::FindFirstFile(_T("*.mp3"),&findFileData);

	if(hFind!=INVALID_HANDLE_VALUE)
	{
		if (1)//(findFileData.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY))
		{
			TCHAR *_findName=new TCHAR[MAX_PATH];
			_tcscpy(_findName,findFileData.cFileName);
			TCHAR *pathName=new TCHAR[MAX_PATH+1];
			_tcscpy(pathName,pszFolder);
			_tcscat(pathName,_T("\\"));


			_tcscat(pathName,_findName);
			tstring str(pathName);
			//m_songList.push_back(str);

			PlayListItem item(&str);
			item.scanId3Info();
			m_songList.push_back(item);
		}

		BOOL ret;
		while(ret=FindNextFile(hFind,&findFileData))
		{
			if (1)//(!(findFileData.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY))
			{
				TCHAR *_findName=new TCHAR[MAX_PATH];
				_tcscpy(_findName,findFileData.cFileName);
				TCHAR *pathName=new TCHAR[MAX_PATH+1];
				_tcscpy(pathName,pszFolder);
				_tcscat(pathName,_T("\\"));
				_tcscat(pathName,_findName);
				tstring str(pathName);
				//m_songList.push_back(str);

				PlayListItem item(&str);
				item.scanId3Info();
				m_songList.push_back(item);
				


			}
		}
	}
	FindClose(hFind);

	return TRUE;
}


BOOL PlayListItem::scanId3Info()
{
	char* week_day[]= {"sunday", "monday", "tuesday", "wednesday","thursday", "friday", "saturday", NULL };
	char* MP3ID3Genre[]= 
	{
		"Blues","Classic Rock","Country","Dance","Disco","Funk","Grunge","Hip-Hop",
		"Jazz","Metal","New Age","Oldies","Other","Pop","R&B","Rap","Reggae","Rock",
		"Techno","Industrial","Alternative","Ska","Death Metal","Pranks","Soundtrack",
		"Euro-Techno","Ambient","Trip Hop","Vocal","Jazz Funk","Fusion","Trance",
		"Classical","Instrumental","Acid","House","Game","Sound Clip","Gospel","Noise",
		"Alternative Rock","Bass","Soul","Punk","Space","Meditative","Instrumental Pop",
		"Instrumental Rock","Ethnic","Gothic","Darkwave","Techno-Industrial","Electronic",
		"Pop Folk","Eurodance","Dream","Southern Rock","Comedy","Cult","Gangsta","Top 40",
		"Christian Rap","Pop Funk","Jungle","Native American","Cabaret","New Wave",
		"Psychadelic","Rave","ShowTunes","Trailer","Lo-Fi","Tribal","Acid Punk","Acid Jazz",
		"Polka","Retro","Musical","Rock & Roll","Hard Rock","Folk","Folk Rock",
		"National Folk","Swing","Fast Fusion","Bebob","Latin","Revival","Celtic",
		"Bluegrass","Avantgarde","Gothic Rock","Progressive Rock","Psychedelic Rock",
		"Symphonic Rock","Slow Rock","Big Band","Chorus","Easy Listening","Acoustic",
		"Humour","Speech","Chanson","Opera","Chamber Music","Sonata","Symphony","Booty Bass",
		"Primus","Porn Groove","Satire","Slow Jam","Club","Tango","Samba","Folklore","Ballad",
		"Power Ballad","Rhytmic Soul","Freestyle","Duet","Punk Rock","Drum Solo","A Capella",
		"Euro House","Dance Hall","Goa","Drum & Bass","Club House","Hardcore","Terror",
		"Indie","BritPop","Negerpunk","Polsk Punk","Beat","Christian Gangsta Rap",
		"Heavy Metal","Black Metal","Crossover","Contemporary Christian","Christian Rock",
		"Merengue","Salsa","Trash Metal","Anime","JPop","SynthPop"
	};

	TCHAR *filePath=new TCHAR[256];
	_tcscpy(filePath,url.c_str());
	int len=_tcslen(filePath);
	TCHAR* tmp=filePath+len;
	while (tmp--)
	{
		if ((TCHAR)(*tmp)=='.')
		{
			tmp++;
			break;
		}
	}
	
	MusicFile *file;
	if (!_tcscmp(tmp,_T("wav")))
		file=new WaveFile();
	if (!_tcscmp(tmp,_T("mp3")))
		file=new Mp3File();
	if (!_tcscmp(tmp,_T("wma")))
		file=new Mp3File();


	INT result=file->OpenAndReadID3Info(filePath);
	if (result==FALSE)
	{
		return FALSE;
	}

	mpg123_id3v1* v1=file->m_pMpg123_id3v1;
	mpg123_id3v2* v2=file->m_pMpg123_id3v2;
	//-----------------------------------------
	//have id3v1 or id3v2 info ???
	//int result=mpg123_meta_check(file->m)

	BOOL bID3v1=FALSE;
	char *p;
	p=(char*)v1;
	if (p)
	{
		if (!strncmp(p,"TAG",3))
		{			
			for(int i=3;i<127;i++)
			{
				if(p[i] != ' ' &&  p[i] != 0X00)
				{
					bID3v1=TRUE;
					break;
				}
			}
			//p[128]   could be 0xff
		}
	}

	
	if (bID3v1)
	{
		title=Ansi2Unicode(&v1->title[0]);
		artist=Ansi2Unicode(&v1->artist[0]);
		album=Ansi2Unicode(&v1->album[0]);
		year=Ansi2Unicode(&v1->year[0]);
		comment=Ansi2Unicode(&v1->comment[0]);

		//int igenre=atoi((const char*)v1->genre);
		//genre=Ansi2Unicode(MP3ID3Genre[v1->genre]);
	}
	else
	{
		const char  version=file->m_pMpg123_id3v2->version;
		if (version==0x03 || version==0x04)
		{
			if (v2->title)
				title=UTF82Unicode(v2->title->p);
			if (v2->artist)
				artist=UTF82Unicode(v2->artist->p);
			if (v2->album)
				album=UTF82Unicode(v2->album->p);
			if (v2->year)
				year=UTF82Unicode(v2->year->p);
			if (v2->genre)
				genre=UTF82Unicode(v2->genre->p);
			if (v2->comment)
				comment=UTF82Unicode(v2->comment->p);
			
			if (v2->genre)
			{
				int igenre=atoi((const char*)v2->genre->p);
				genre=UTF82Unicode(MP3ID3Genre[igenre]);
			}
		}
	}


	delete file;
	return TRUE;
}