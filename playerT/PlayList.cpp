#include "StdAfx.h"
#include "PlayList.h"
#include "MyLib.h"
#include "BasicPlayer.h"
#include "customMsg.h"
#include "StringConvertions.h"
#include "DialogLyric.h"

struct PLANDPATH
{
	PlayList* pPlaylist;
	LPCTSTR pszFolder;
};

static DWORD CALLBACK AddFolderThreadProc(LPVOID lpParameter)
{
	PLANDPATH* p=(PLANDPATH*)lpParameter;
	BOOL result=p->pPlaylist->AddFolder(p->pszFolder);
	
	::PostMessage(MyLib::GetMain(),WM_ADDFOLDERED,NULL,NULL);
	
	delete p;
	return result;
}




//-----------------------------------------
//PlayList
 PlayList::PlayList(void):curPlayingItem(NULL),curSelectedItem(NULL)
{
	index=Default;
	std::tstring tmp(_T("all.pl"));
	m_saveLocation=tmp;
}

 PlayList::PlayList(std::tstring name):curPlayingItem(NULL),curSelectedItem(NULL)
 {
	index=Default;
	m_playlistName=name;
	std::tstring tmp(_T("all.pl"));
	m_saveLocation=tmp;
 }


PlayList::~PlayList(void)
{	
}


BOOL PlayList::AddFolderByThread(LPCTSTR pszFolder)
{
	PLANDPATH* p=new PLANDPATH;
	p->pPlaylist=this;
	p->pszFolder=pszFolder;

	HANDLE handle=::CreateThread(NULL,NULL,AddFolderThreadProc,(LPVOID)p,
		NULL,NULL);
	return 0;
}

BOOL PlayList::AddFolder(LPCTSTR pszFolder)
{
	//todo
	//忽略了子目录下的mp3文件
	TCHAR* curPath=new TCHAR[256];
	_tgetcwd(curPath,256);

	//改变当前目录
	_tchdir(pszFolder);

	BOOL findResult=FALSE;
	WIN32_FIND_DATA findFileData;
	HANDLE hFind;

	hFind=::FindFirstFile(_T("*.mp3"),&findFileData);
	if(hFind!=INVALID_HANDLE_VALUE){
		findResult=TRUE;
		while(findResult)
		{
			TCHAR *_findName=new TCHAR[MAX_PATH];
			_tcscpy(_findName,findFileData.cFileName);
			TCHAR *pathName=new TCHAR[MAX_PATH+1];
			_tcscpy(pathName,pszFolder);
			_tcscat(pathName,_T("\\"));
			_tcscat(pathName,_findName);
			std::tstring str(pathName);

			PlayListItem *pItem=new PlayListItem(&str);
			pItem->ScanId3Info();
			m_songList.push_back(*pItem);
			//msonglist的析构会删除*pItem;

			findResult=FindNextFile(hFind,&findFileData);
		}

		FindClose(hFind);
	}
	

	_tchdir(curPath);

	return TRUE;
}

PlayListItem* PlayList::nextTrack()
{
	PlayListItem*tmp=nextPlayingItem; 
	nextPlayingItem=NULL;
	return tmp;
}


void PlayList::SetCurPlaying(PlayListItem* item,BOOL scanID3)
{
	curPlayingItem=item;
	if (scanID3)
		curPlayingItem->ScanId3Info();
}
PlayListItem* PlayList::GetNextTrackByOrder(BOOL bMoveCur)
{
	list<PlayListItem>::iterator cur,next;

	for (cur=m_songList.begin();cur!=m_songList.end();cur++){
		if (&(*cur)==curPlayingItem)
			break;
	}

	lastPlayingItem=curPlayingItem;

	if (++cur==m_songList.end())
		return NULL;

	if (index==Default){
		next=cur;
	}
	if (index==Repeat_playlist){
		next=cur;
	}
	if (index==Random){
		next=cur;
	}
	if (index==Repeat_track){
		next=cur;
	}

	if(bMoveCur)curPlayingItem=&(*next);
	return &(*next);
}


PlayListItem::~PlayListItem()
{
	if (img){
		delete img;
		img=NULL;
	}
	if (pPicBuf){	
		delete pPicBuf;
		pPicBuf=NULL;
	}
}

BOOL PlayListItem::ScanId3Info()
{
	BOOL bInvalidID3V2=FALSE;

	MPEG::File f(url.c_str());

	ID3v2::Tag *id3v2tag = f.ID3v2Tag();
	if(id3v2tag) {
		title=id3v2tag->title().toWString();
		artist=id3v2tag->artist().toWString();
		album=id3v2tag->album().toWString();
		genre=id3v2tag->genre().toWString();
		year=id3v2tag->year();
		lyricInner=id3v2tag->lyric().toWString();
		if (!lyricInner.empty())
			m_bLrcInner=TRUE;

		// we will use bytevector to retain to memory in frame
		pPicBuf=new ByteVector;
		id3v2tag->retainPicBuf(pPicBuf);
		//-----------------------------------------
		//idev3 album picture info
		img=new CImage;
		// load resource into memory
		DWORD len =pPicBuf->size();
		BYTE* lpRsrc=(BYTE*)pPicBuf->data();

		// Allocate global memory on which to create stream
		HGLOBAL m_hMem = GlobalAlloc(GMEM_FIXED, len);
		BYTE* pmem = (BYTE*)GlobalLock(m_hMem);
		memcpy(pmem,lpRsrc,len);
		IStream* pstm;
		CreateStreamOnHGlobal(m_hMem,FALSE,&pstm);
		if (S_OK != img->Load(pstm)){
			delete img;
			img=NULL;
		}
		
		//-----------------------------------------

		if ( title.empty() && artist.empty() && album.empty())
			bInvalidID3V2=TRUE;
	}
	else
		bInvalidID3V2=TRUE;
	

	if(bInvalidID3V2){
		ID3v1::Tag *id3v1tag = f.ID3v1Tag();
		if(id3v1tag) {
			title=id3v1tag->title().toWString();
			artist=id3v1tag->artist().toWString();
			album=id3v1tag->album().toWString();
			genre=id3v1tag->genre().toWString();

			year=id3v1tag->year();
		}
	}

	return TRUE;
}




BOOL PlayListItem::LrcFileMacth(std::tstring &filename)
{
	if( search(filename.begin(),filename.end(),title.begin(),title.end())!=filename.end() )
		return TRUE;

// 	if( search(filename.begin(),filename.end(),artist.begin(),artist.end()) !=filename.end())
// 		return TRUE;

	int index=url.find_last_of('\\');
	if(index!=url.length()){
		std::tstring filename=url.substr(index);
		if( search(filename.begin(),filename.end(),artist.begin(),artist.end()) !=filename.end())
			return TRUE;	
	}

	return FALSE;
}

BOOL PlayListItem::GetLrcFileFromLib()
{
	vector<std::tstring>::iterator i;
	for (i=MyLib::shared()->dataPaths.begin();i!=MyLib::shared()->dataPaths.end();i++)
	{
		int index=(*i).find_last_of('\\');
		if(index!=(*i).length()){
			std::tstring filename=(*i).substr(index);
			if (LrcFileMacth(filename)){
				lycPath=*i;
				return TRUE;
			}
		}
	}

	return FALSE;
}



BOOL LrcMng::OpenTrackPath(PlayListItem* track)
{
	Open((LPTSTR)track->lycPath.c_str());
	track->lyricFromLrcFile=lib;
	track->m_bLrcFromLrcFile=TRUE;
	return TRUE;
}