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
	::SendMessage(MyLib::GetMain(),WM_FILE_FINDED,NULL,(LPARAM)1);
	BOOL result=p->pPlaylist->AddFolder(p->pszFolder,TRUE);
	::SendMessage(MyLib::GetMain(),WM_FILE_FINDED,NULL,(LPARAM)0);

	::PostMessage(MyLib::GetMain(),WM_ADDFOLDERED,NULL,NULL);
	
	delete p;
	return result;
}




//-----------------------------------------
//PlayList
 PlayList::PlayList(void):curPlayingItem(NULL),curSelectedItem(NULL)
{
}

 PlayList::PlayList(std::tstring &name):curPlayingItem(NULL),curSelectedItem(NULL)
 {
	m_playlistName=name;
	m_saveLocation=m_playlistName+_T(".pl");
 }


PlayList::~PlayList(void)
{	
	curPlayingItem=NULL;
	curSelectedItem=NULL;
	m_songList.clear();
}


BOOL PlayList::AddFolderByThread(LPCTSTR pszFolder)
{
	PLANDPATH* p=new PLANDPATH;
	p->pPlaylist=this;
	p->pszFolder=pszFolder;

	hAddDir=::CreateThread(NULL,NULL,AddFolderThreadProc,(LPVOID)p,
		NULL,NULL);
	return 0;
}

void PlayList::TerminateAddDirThread()
{
	::TerminateThread(hAddDir,-1);
	::SendMessage(MyLib::GetMain(),WM_FILE_FINDED,NULL,(LPARAM)0);
	::PostMessage(MyLib::GetMain(),WM_ADDFOLDERED,NULL,NULL);
}

//ignore case
BOOL StrIsEndedWith(TCHAR *_str,TCHAR *_end)
{
	int strLen=_tcslen(_str);
	int endLen=_tcslen(_end);

	if (strLen<endLen)
		return FALSE;
	
	
	for (int i=1;i<=endLen;i++)
		if (tolower((int)_end[endLen-i] ) !=  tolower((int)_str[strLen-i] ))
			return FALSE;
	
	return TRUE;
}

// return TRUE if the file name is "." or ".." 
BOOL IsDots(TCHAR* fn)
{
	BOOL bResult=FALSE;
	if (fn[0]=='.')
		if (fn[1]=='\0' ||
		   (fn[1]=='.'&& fn[2]=='\0'))
				bResult=TRUE;
	return bResult;
}

BOOL PlayList::AddFolder(LPCTSTR pszFolder,BOOL bIncludeDir)
{
	//todo
	//CFileFind a;a.IsDots()
	//忽略了子目录下的mp3文件
	TCHAR* oldPath=new TCHAR[MAX_PATH];
	_tgetcwd(oldPath,MAX_PATH);

	//改变当前目录
	_tchdir(pszFolder);

	BOOL findResult=FALSE;
	WIN32_FIND_DATA findFileData;
	HANDLE hFind;

	hFind=::FindFirstFile(_T("*"),&findFileData);
	if(hFind!=INVALID_HANDLE_VALUE)
	{
		findResult=TRUE;
		while(findResult)
		{
			if( IsDots(findFileData.cFileName) )
			{
			}
			//目录
			else if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				if(bIncludeDir)
					AddFolder(findFileData.cFileName,bIncludeDir);
			}
			else//文件
			{
				TCHAR *mp3File=_T(".mp3");
				TCHAR *wavFile=_T(".wav");
				if (StrIsEndedWith(findFileData.cFileName,mp3File) ||
					StrIsEndedWith(findFileData.cFileName,wavFile))
				{
					TCHAR *_findName=new TCHAR[MAX_PATH];
					memset(_findName,0,(MAX_PATH)*sizeof(TCHAR));
					_tcscpy(_findName,findFileData.cFileName);

					TCHAR* pathName=new TCHAR[MAX_PATH*2];
					memset(pathName,0,(MAX_PATH*2)*sizeof(TCHAR));
					_tgetcwd(pathName,MAX_PATH*2);
					
					_tcscat(pathName,_T("\\"));
					_tcscat(pathName,_findName);
					std::tstring str(pathName);
					
					::SendMessage(MyLib::GetMain(),WM_FILE_FINDED,(WPARAM)pathName,(LPARAM)2);
					delete[] pathName;
					delete[] _findName;

					PlayListItem *pItem=new PlayListItem(this,&str);
					pItem->ScanId3Info();
					m_songList.push_back(*pItem);
					//msonglist的析构会删除*pItem;
				}
			}

			findResult=FindNextFile(hFind,&findFileData);
		}//while(findResult)

		FindClose(hFind);
	}//if(hFind!=INVALID_HANDLE_VALUE)
	

	_tchdir(oldPath);
	delete[] oldPath;

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
	if (scanID3) curPlayingItem->ScanId3Info(TRUE);
}

PlayListItem* PlayList::GetNextTrackByOrder(BOOL bMoveCur)
{
	list<PlayListItem>::iterator cur,next;

	for (cur=m_songList.begin();cur!=m_songList.end();++cur)
	{
		if (&(*cur)==curPlayingItem)break;
	}

	lastPlayingItem=curPlayingItem;

	if ( cur==m_songList.end()|| ++cur==m_songList.end())
		return NULL;

	next=MyLib::shared()->GetNextByOrder(--cur);

	if(bMoveCur)curPlayingItem=&(*next);
	return &(*next);
}


PlayListItem::~PlayListItem()
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


void TrimRightByNull(std::wstring &_str)
{
	char c='\0';
	int index=_str.find_first_of(c);
	if (index!=_str.npos)
		_str=_str.substr(0,index);	
}


BOOL PlayListItem::ScanId3Info(BOOL bRetainPic)
{
	if(m_bStatus!=UNKNOWN && bRetainPic==FALSE)return TRUE;

	MPEG::File f(url.c_str());
	ID3v2::Tag *id3v2tag = f.ID3v2Tag();
	BOOL bInvalidID3V2=FALSE;
	if(id3v2tag) 
	{
		if(m_bStatus==UNKNOWN)
		{
			title=id3v2tag->title().toWString();
			artist=id3v2tag->artist().toWString();
			album=id3v2tag->album().toWString();
			genre=id3v2tag->genre().toWString();
			year=id3v2tag->year();
			lyricInner=id3v2tag->lyric().toWString();
		
			if (!lyricInner.empty())
				m_bLrcInner=TRUE;

			if ( title.empty() || artist.empty() )
				bInvalidID3V2=TRUE;
			else
				m_bStatus=ID3V2;
		}

		if(bRetainPic)
		{
			// we will use bytevector to retain to memory in frame
			pPicBuf=NULL;
			id3v2tag->retainPicBuf(pPicBuf);
			if (pPicBuf)
			{
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
					img=NULL;}
			}
			
		}//if(bRetainPic)
	}
	else
	{
		bInvalidID3V2=TRUE;
	}
	

	if(bInvalidID3V2 && m_bStatus==UNKNOWN)
	{
		ID3v1::Tag *id3v1tag = f.ID3v1Tag();
		if(id3v1tag) 
		{
			//the id3v1tag string may be wrongly
			//like 我很好0000fsadfsdf...
			title=id3v1tag->title().toWString();
			artist=id3v1tag->artist().toWString();
			album=id3v1tag->album().toWString();
			genre=id3v1tag->genre().toWString();
			year=id3v1tag->year();

			TrimRightByNull(title);
			TrimRightByNull(artist);
			TrimRightByNull(album);
			TrimRightByNull(genre);
		}
	}

	//if the title is empty ,we will use the filename
	//without suffix
	if (title.empty())
	{
		int a=url.find_last_of('\\');
		int b=url.find_last_of('.');
		if (a<b && b!=url.npos)
			title=url.substr(a+1,b-(a+1));
	}


	return TRUE;
}






//通过歌词文件名判断,是否与当前歌曲匹配
BOOL PlayListItem::LrcFileMacth(std::tstring &lrcFile)
{
	if( search(lrcFile.begin(),lrcFile.end(),title.begin(),title.end())!=lrcFile.end() )
		return TRUE;

// 	if( search(filename.begin(),filename.end(),artist.begin(),artist.end()) !=filename.end())
// 		return TRUE;

	int indexA=url.find_last_of('\\');
	int indexB=url.find_last_of('.');
	if(indexA==url.npos || 
		indexB==url.npos ||
		indexA>indexB) return FALSE;
	else
	{
		if(indexB!=url.length())
		{
			std::tstring filename=url.substr(indexA+1,indexB-indexA);
			if( search(lrcFile.begin(),lrcFile.end(),filename.begin(),filename.end()) !=lrcFile.end())
				return TRUE;	
		}
	}

	return FALSE;
}

BOOL PlayListItem::GetLrcFileFromLib()
{
	vector<std::tstring>::iterator i;
	for (i=MyLib::shared()->dataPaths.begin();i!=MyLib::shared()->dataPaths.end();i++)
	{
		int index=(*i).find_last_of('\\');
		if(index!=(*i).npos){
			std::tstring filename=(*i).substr(index+1);
			if (LrcFileMacth(filename)){
				lycPath=*i;
				return TRUE;
			}
		}
	}

	return FALSE;
}

BOOL HaveHeywordsNoCase(std::tstring &my,TCHAR *keywords)
{
	std ::tstring tmp(my);	
	
	_tcsupr(const_cast<TCHAR*>(tmp.c_str()) );
	_tcsupr(keywords);

	return tmp.find(keywords)!=std::tstring::npos;
}


BOOL PlayListItem::HaveKeywords(TCHAR *keywords)
{	
	BOOL have=FALSE;
	if(
		HaveHeywordsNoCase(title,keywords)||
		HaveHeywordsNoCase(artist,keywords)||
		HaveHeywordsNoCase(album,keywords)||
		HaveHeywordsNoCase(genre,keywords)||
		HaveHeywordsNoCase(comment,keywords)
		)
		have=TRUE;

	if(!m_bLrcFromLrcFile && m_bLrcInner)
		if(HaveHeywordsNoCase(title,keywords))
			have=TRUE;
	return have;
}


BOOL LrcMng::OpenTrackPath(PlayListItem* track)
{
	Open((LPTSTR)track->lycPath.c_str());


	if (lib.size()>0)
	{
		track->lyricFromLrcFile=lib;
		track->m_bLrcFromLrcFile=TRUE;
		return TRUE;
	}
	
	return FALSE;
}