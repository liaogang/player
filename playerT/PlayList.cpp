#include "StdAfx.h"
#include "PlayList.h"
//#include "MusicFile.h"
//#include "WaveFileEx.h"
//#include "Mp3File.h"
#include <direct.h>
//#include <id3/tag.h>



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
using namespace TagLib;



LPSTR Unicode2Ansi(LPCWSTR s)
{
	DWORD dwNum= WideCharToMultiByte (CP_ACP, 0, s, -1, NULL, 0,0,0);
	LPSTR target=new char[dwNum];
	WideCharToMultiByte(CP_ACP,0,s,-1,target,dwNum,0,0);
	return target;
}



LPWSTR Ansi2Unicode(LPSTR s)
{
	DWORD dwNum= MultiByteToWideChar (CP_ACP, 0,(LPCSTR) s, -1, NULL, 0);
	LPWSTR target=new WCHAR[dwNum];
	MultiByteToWideChar(CP_ACP,0,(LPCSTR)s,-1,target,dwNum);
	return target;
}

LPWSTR UTF82Unicode(LPSTR s)
{
	DWORD dwNum = MultiByteToWideChar (CP_UTF8, 0, s, -1, NULL, 0);
	LPWSTR target=new WCHAR[dwNum];
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
			std::tstring str(pathName);
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
				std::tstring str(pathName);
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
// 
// LPTSTR id3Tag_get(ID3_Tag* tag,ID3_FrameID id)
// {
// 	ID3_Frame *frame=NULL;
// 	ID3_Field *field=NULL;
// 	frame=tag->Find(id);
// 	if (frame)
// 	{
// 
// 		const INT LEN=256;
// 		field=frame->GetField(ID3FN_TEXT);
// 		char *_tmp=new char[LEN];
// 		field->Get(_tmp,LEN);
// 		
// #ifdef _UNICODE 
// 		return Ansi2Unicode(_tmp);
// #else
// 		return _tmp;
// #endif
// 		
// 	}
// 	else
// 		return NULL;
// }


BOOL PlayListItem::scanId3Info()
{
// 	ID3_Tag tag;
// 
// #ifdef _UNICODE
// 	tag.Link(Unicode2Ansi(url.c_str()),ID3TT_ID3V1);
// #else
// 	tag.Link((url.c_str()),ID3TT_ALL);
// #endif
// 	
// 	
// 	if (tag.HasV1Tag())
// 	{
// 		int i=0;
// 		i++;
// 	}
// 	if (tag.HasV2Tag())
// 	{
// 		int j=0;
// 		j++;
// 	}
// 	ID3_V2Spec spec=tag.GetSpec();
// 
// 
// 	title=id3Tag_get(&tag,ID3FID_TITLE);
// 	artist=id3Tag_get(&tag,ID3FID_LEADARTIST);
// 	album=id3Tag_get(&tag,ID3FID_ALBUM);
// 	year=id3Tag_get(&tag,ID3FID_YEAR);
// 	comment=id3Tag_get(&tag,ID3FID_COMMERCIAL);
// 	
// 	if (!title && !artist && !album && !year)
// 	{
// 		//bID3v1=FALSE;
// 	}

	
// 	if (tag.HasV2Tag())
// 	{
// 		ID3_Tag tag2;
// 		tag2.Link(Unicode2Ansi(url.c_str()),ID3TT_ID3V2/*|ID3TT_LYRICS*/);
// 		ID3_Frame *frame=NULL;
// 		ID3_Field *field=NULL;
// 
// 		title=id3Tag_get(&tag,ID3FID_TITLE);
// 		artist=id3Tag_get(&tag,ID3FID_LEADARTIST);
// 		album=id3Tag_get(&tag,ID3FID_ALBUM);
// 		year=id3Tag_get(&tag,ID3FID_YEAR);
// 		comment=id3Tag_get(&tag,ID3FID_COMMERCIAL);
// 
// 
// 		frame=tag.Find(ID3FID_PICTURE);
// 		if (frame && frame->Contains(ID3FN_DATA))
//		{
// 			frame->Field(ID3FN_DATA).ToFile("C:\\abc.pic");
// 
// 
// 
// 
// 			ID3_Tag::Iterator *i=tag2.CreateIterator();
// 			while( (frame=i->GetNext()) !=NULL)
// 			{
// 				void *buf=frame->GetField(ID3FN_TEXT);
// 
// 			}
// 
// 			BITMAP bitmap;
// 		}
	





// 	TagLib::FileRef f("Latex Solar Beef.mp3");
// 	TagLib::String artist = f.tag()->artist(); // artist == "Frank Zappa"
// 
// 	f.tag()->setAlbum("Fillmore East");
// 	f.save();

// 	TagLib::FileRef g("Free City Rhymes.ogg");
// 	TagLib::String album = g.tag()->album(); // album == "NYC Ghosts & Flowers"
// 
// 	g.tag()->setTrack(1);
// 	g.save();


	MPEG::File f(url.c_str());

	ID3v2::Tag *id3v2tag = f.ID3v2Tag();

	if(id3v2tag) 
	{

// 		ID3v2::FrameList::ConstIterator it = id3v2tag->frameList().begin();
// 		for(; it != id3v2tag->frameList().end(); it++)
// 		{
// 			//(*it)->frameID()
// 			TagLib::String ss=(*it)->toString();
// 			const char *s=ss.toCString(true);
// 			s="";
// 		}
		TagLib::String title=id3v2tag->title();
		TagLib::String artist=id3v2tag->artist();
		TagLib::String album=id3v2tag->album();
		TagLib::uint year=id3v2tag->year();
		TagLib::String genre=id3v2tag->genre();
	}
	else
	{
		//cout << "file does not have a valid id3v2 tag" << endl;

	//cout << endl << "ID3v1" << endl;

	ID3v1::Tag *id3v1tag = f.ID3v1Tag();

	if(id3v1tag) {
		TagLib::String s=id3v1tag->title();
		s=id3v1tag->album();

// 		cout << "title   - \"" << id3v1tag->title()   << "\"" << endl;
// 		cout << "artist  - \"" << id3v1tag->artist()  << "\"" << endl;
// 		cout << "album   - \"" << id3v1tag->album()   << "\"" << endl;
// 		cout << "year    - \"" << id3v1tag->year()    << "\"" << endl;
// 		cout << "comment - \"" << id3v1tag->comment() << "\"" << endl;
// 		cout << "track   - \"" << id3v1tag->track()   << "\"" << endl;
// 		cout << "genre   - \"" << id3v1tag->genre()   << "\"" << endl;
	}
	else
		cout << "file does not have a valid id3v1 tag" << endl;



	APE::Tag *ape = f.APETag();

	cout << endl << "APE" << endl;

	if(ape) 
	{
		for(APE::ItemListMap::ConstIterator it = ape->itemListMap().begin();
			it != ape->itemListMap().end(); ++it)
		{
			cout << (*it).first << " - \"" << (*it).second.toString() << "\"" << endl;
		}
	}


/*	cout << endl;*/
}







	return TRUE;
}