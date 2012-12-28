#include "StdAfx.h"
#include "PlayList.h"
#include "MyLib.h"
#include "BasicPlayer.h"
#include "customMsg.h"
#include "StringConvertions.h"
#include "DialogLyric.h"
#include "CImg.h"
#include "StringConvertions.h"

using namespace TagLib;


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
 PlayList::PlayList(void):
	curSelectedItem(NULL),
	lastPlayingItem(NULL),
	curPlayingItem(NULL),
	nextPlayingItem(NULL),
	topVisibleIndex(0),
	selectedIndex(-1)
{

}

 PlayList::PlayList(std::tstring &name):
	curSelectedItem(NULL),
	lastPlayingItem(NULL),
	curPlayingItem(NULL),
	nextPlayingItem(NULL),
	topVisibleIndex(0),
	selectedIndex(-1)
 {
	m_playlistName=name;
	m_saveLocation=m_playlistName+_T(".pl");
 }


PlayList::~PlayList(void)
{	
	curPlayingItem=NULL;
	curSelectedItem=NULL;

	_songContainer::iterator i;
	for (i=m_songList.begin();i!=m_songList.end();i++)
	{
		_songContainerItem item=*i;
		delete item;
	}

	m_songList.clear();
}

void PlayList::DeleteTrack(PlayListItem* track)
{
	if(track)
		m_songList.erase(m_songList.begin()+track->indexInListView);
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


HRESULT GetFolderDescriptionId(LPCWSTR pszPath, SHDESCRIPTIONID *pdid)
{
	HRESULT hr;
	LPITEMIDLIST pidl;
	if (SUCCEEDED(hr = SHParseDisplayName(pszPath, NULL,
		&pidl, 0, NULL))) 
	{
			IShellFolder *psf;
			LPCITEMIDLIST pidlChild;
			if (SUCCEEDED(hr = SHBindToParent(pidl, IID_IShellFolder,
				(void**)&psf, &pidlChild))) 
			{
					hr = SHGetDataFromIDList(psf, pidlChild,
						SHGDFIL_DESCRIPTIONID, pdid, sizeof(*pdid));
					psf->Release();
			}
			CoTaskMemFree(pidl);
	}
	return hr;
}

BOOL IsRecycle(TCHAR *fn)
{
	BOOL bResult=FALSE;

	SHDESCRIPTIONID did;
	if (SUCCEEDED(GetFolderDescriptionId(fn, &did)) &&
		did.clsid == CLSID_RecycleBin) 
		bResult=TRUE;
	

	return bResult;
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

BOOL PlayList::AddFile(TCHAR *filepath)
{
	std::tstring str(filepath);
	PlayListItem *pItem=new PlayListItem(this,&str);
	if(pItem->ScanId3Info())
	{	
		pItem->itemIndex=m_songList.size();
		m_songList.push_back(pItem);
		//msonglist的析构会删除*pItem;
		::SendMessage(MyLib::GetMain(),WM_FILE_FINDED,(WPARAM)filepath,(LPARAM)2);
		return TRUE;
	}

	return FALSE;
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
			if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM)
			{
				//system file, e.recycled, should ignore it
			}
			//目录
			else if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY &&
				!IsDots(findFileData.cFileName))
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
					
					AddFile(pathName);

					delete[] _findName;
					delete[] pathName;
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
	_songContainer::iterator cur,next;

	for (cur=m_songList.begin();cur!=m_songList.end();++cur)
	{
		if (*cur==curPlayingItem)break;
	}

	lastPlayingItem=curPlayingItem;

	if ( cur==m_songList.end()|| ++cur==m_songList.end())
		return NULL;

	next=MyLib::shared()->GetNextByOrder(--cur);

	if(bMoveCur)curPlayingItem=*next;
	return *next;
}


PlayListItem::~PlayListItem()
{
	if (img)
	{
		delete img;
		img=NULL;
	}

// 	if (pPicBuf)
// 	{	
// 		delete pPicBuf;
// 		pPicBuf=NULL;
// 	}
}




void PlayListItem::Buf2Img(BYTE* lpRsrc,DWORD len)
{
// 	TCHAR *picName=_T("runningArtWork.jpeg");
// 	FILE *file=_tfopen(picName,L"wb");
// 	if(file)
// 	{
// 	 	::fwrite(pPicBuf->data(),1,pPicBuf->size(),file);
// 	 	fclose(file);
// 	}
	
// 	CImg img1;
// 	img1.load_jpeg(picName);
// 	img1.draw_image()
	
	//idev3 album picture info
	img=new CImage;  
// 	if (S_OK != img->Load(picName) ){
// 	 	delete img;
// 	 	img=NULL;}

	// Allocate global memory on which to create stream
	HGLOBAL m_hMem = GlobalAlloc(GMEM_FIXED, len);
	BYTE* pmem = (BYTE*)GlobalLock(m_hMem);
	memcpy(pmem,lpRsrc,len);
	IStream* pstm;
	CreateStreamOnHGlobal(m_hMem,FALSE,&pstm);
	if (S_OK != img->Load(pstm)){
		delete img;
		img=NULL;}
};

BOOL PlayListItem::ScanId3Info(BOOL bRetainPic,BOOL forceRescan)
{
	if( !forceRescan && m_bStatus!=UNKNOWN)
		return TRUE;

	MPEG::File f(url.c_str());

	APE::Tag *apeTag=f.APETag();
	if (apeTag)
		if(m_bStatus==UNKNOWN)
			title=apeTag->title().toWString();
		


	ID3v2::Tag *id3v2tag = f.ID3v2Tag();
	BOOL bInvalidID3V2=FALSE;
	if(id3v2tag) 
	{
		if(forceRescan ||
			(!forceRescan && m_bStatus==UNKNOWN) )
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

			if(  m_bStatus==ID3V2  && bRetainPic)
			{
				// we will use bytevector to retain to memory in frame
				pPicBuf=NULL;
				

				ID3v2::AttachedPictureFrame::Type type=static_cast<ID3v2::AttachedPictureFrame::Type>(id3v2tag->retainPicBuf(&pPicBuf));
				AtlTrace("picture type :%d \n",(int)type);
				if (pPicBuf)
					Buf2Img((BYTE*)pPicBuf->data(),pPicBuf->size());
			}
		}
	}
	else
	{
		bInvalidID3V2=TRUE;
	}
	
	ID3v1::Tag *id3v1tag=NULL;
	if(bInvalidID3V2 &&( (!forceRescan && m_bStatus==UNKNOWN ) || forceRescan))
	{
		id3v1tag = f.ID3v1Tag();
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


	if(id3v2tag && id3v1tag)
	{
		//if the title is empty ,we will use the filename
		//without suffix
		if (title.empty())
		{
			int a=url.find_last_of('\\');
			int b=url.find_last_of('.');
			if (a<b && b!=url.npos)
				title=url.substr(a+1,b-(a+1));
		}
	}


	return id3v2tag&&id3v1tag;
}




BOOL PlayListItem::TryLoadLrcFile(std::tstring &filename,BOOL forceLoad)
{
	if (forceLoad || LrcFileMacth(filename))//filename match
	{
		LrcMng *m=LrcMng::Get();
		if(m->OpenTrackPath(this,filename,forceLoad ? FALSE :TRUE))//lrc tag match
		{
			lycPath=filename;
			m_bLrcFromLrcFile=TRUE;
			return TRUE;
		}
	}

	return FALSE;
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



BOOL PlayListItem::GetLrcFileFromLib(BOOL forceResearch)
{
	if (!forceResearch && m_bLrcFromLrcFile)
		return TRUE;

	vector<std::tstring>::iterator i;
	for (i=MyLib::shared()->dataPaths.begin();i!=MyLib::shared()->dataPaths.end();i++)
	{
		if(TryLoadLrcFile(*i))
			return TRUE;
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


BOOL LrcMng::OpenTrackPath(PlayListItem* track,std::tstring &path,BOOL bMatchIdentityTag)
{
	Open((LPTSTR)path.c_str());

	if (lib.size()>0)
	{
		if(!bMatchIdentityTag)
		{
			track->lyricFromLrcFile=lib;
			return TRUE;
		}
		else
		{
			
			if (!ti.empty() && StrCmpIgnoreCaseAndSpace(track->title,ti)!=0)
			{
				 return FALSE;
			}

			if (!ar.empty() && StrCmpIgnoreCaseAndSpace(track->artist,ar) !=0 )
			{
				return FALSE;
			}

			{
				track->lyricFromLrcFile=lib;
				return TRUE;
			}
		}
	}

	return FALSE;
}