#include "PlayList.h"
#include <ShlObj.h>
#include "MyLib.h"
#include "BasicPlayer.h"
#include "customMsg.h"
#include "StringConvertions.h"
#include "globalStuffs.h"
#include "forwardMsg.h"
#include <time.h>

#include "Util1.h"
#include "MyConfigs.h"

#include <map>

#ifdef APP_PLAYER_UI
#include "CImg.h"
#endif

#include <io.h>
#include <xutility>
#include <algorithm>



#ifdef APP_PLAYER_UI
#include "LrcMng.h"
//for parse the ID3 tag
#include <direct.h>
#include <stdlib.h>
#include <mpegfile.h>
#include <id3v2tag.h>
#include <id3v2frame.h>
#include <id3v2header.h>
#include <id3v1tag.h>
#include <apetag.h>
#include <fileref.h>
#include <tbytevector.h>
#include <attachedpictureframe.h>
using namespace TagLib;
#endif

LPCPlayListItem MakeDuplicate(const LPCPlayListItem item)
{
	LPCPlayListItem dup(new CPlayListItem(* item.get()));
	
#ifdef APP_PLAYER_UI
	dup->img=NULL;
#endif
	return dup;
}



struct PLANDPATH
{
	CPlayList* pPlaylist;
	LPTSTR pszFolder;
};

static DWORD CALLBACK AddFolderThreadProc(LPVOID lpParameter)
{
	PLANDPATH* p=(PLANDPATH*)lpParameter;
	
	NotifyMsg(WM_FILE_FINDED,FALSE,(WPARAM)p->pPlaylist,(LPARAM)1);
	
	BOOL result=p->pPlaylist->AddFolder(p->pszFolder,TRUE);
	NotifyMsg(WM_FILE_FINDED,FALSE,NULL,(LPARAM)0);

#ifdef APP_PLAYER_UI
	SdMsg(WM_PL_TRACKNUM_CHANGED,TRUE,(WPARAM)p->pPlaylist,(LPARAM)result);
#endif

	//delete p->pszFolder;

	delete p;

	return result;
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



BOOL HaveHeywordsNoCase(const std::tstring &my,const TCHAR *keywordsLower)
{
	TCHAR buf[128];
	_tcscpy(buf,my.c_str());
	_tcslwr(buf);
	
	return _tcsstr(buf,keywordsLower)!=NULL;
}


bool PlayListItemComp(const LPCPlayListItem a,const LPCPlayListItem b)
{
	return 0;
	//return 0>_tcscmp(a->GetFileTrack()->url.c_str(),b->GetFileTrack()->url.c_str());	
}

bool IsPlayListItemDup(const LPCPlayListItem a,const LPCPlayListItem b)
{
	return 0;
	//return 0==_tcscmp(a->GetFileTrack()->url.c_str(),b->GetFileTrack()->url.c_str());	
}




CPlayListItem::~CPlayListItem()
{
	#ifdef APP_PLAYER_UI
	if (img)
	{
		delete img;
		img=NULL;
	}
	#endif
}

#ifdef APP_PLAYER_UI

void  CPlayListItem::ClearImgBuf()
{

	if (img)
	{
		delete img;
		img=NULL;
	}

}

void CPlayListItem::Buf2Img(BYTE* lpRsrc,DWORD len)
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
	if (S_OK != img->Load(pstm))
	{
		delete img;
		img=NULL;
	}

	GlobalUnlock(m_hMem);
	GlobalFree(m_hMem);

};
#endif


BOOL  CPlayListItem::IsFileExist() const
{
	//check if the file is exist
	return _taccess(url.c_str(), 0 )!=-1;
}




BOOL CPlayListItem::ScanId3Info(BOOL bRetainPic,BOOL forceRescan)
{
#ifdef APP_PLAYER_TRAY
	return TRUE;
#endif
	if( !forceRescan && m_bStatus!=UNKNOWN)
		return TRUE;
#ifdef APP_PLAYER_UI

	if ( !forceRescan)
	{
		FILETIME lastWriteTime;
		GetFileLastWriteTime(url.c_str(), lastWriteTime);
		if (CompareFileTime(&lastWriteTime,&fileTime)<=0)
		{
			return TRUE;
		}
	}

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

			if ( title.empty() || artist.empty() )
			{
				bInvalidID3V2=TRUE;
			}
			else
			{
				m_bStatus=ID3V2;
				album=id3v2tag->album().toWString();
				genre=id3v2tag->genre().toWString();

				TCHAR cYear[MAX_PATH]={_T("?")};
				uYear=id3v2tag->year();

				if(uYear!=0)
					_itow(id3v2tag->year(),cYear,10);
				year=cYear;


				lyricInner=id3v2tag->lyric().toWString();

				if (!lyricInner.empty())
					m_bLrcInner=TRUE;
			}

			if(  m_bStatus==ID3V2  && bRetainPic)
			{
				// we will use bytevector to retain to memory in frame
				pPicBuf=NULL;


				ID3v2::AttachedPictureFrame::Type type=static_cast<ID3v2::AttachedPictureFrame::Type>(id3v2tag->retainPicBuf(&pPicBuf));
				//AtlTrace("picture type :%d \n",(int)type);
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

			TCHAR cYear[MAX_PATH]={_T("?")};
			uYear=id3v1tag->year();
			if(uYear!=0)
				_itow(id3v1tag->year(),cYear,10);
			year=cYear;

			TrimRightByNull(title);
			TrimRightByNull(artist);
			TrimRightByNull(album);
			TrimRightByNull(genre);
		}
	}


	if(id3v2tag || id3v1tag)
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


	return id3v2tag || id3v1tag;
	#endif
}

void CPlayListItem::initFileLastWriteTime()
{
	fileTime.dwHighDateTime = 0;
	fileTime.dwLowDateTime = 0;
}

void CPlayListItem::updateFileLastWriteTime()
{
	GetFileLastWriteTime(GetUrl().c_str(), getFileTime());
}

#ifdef APP_PLAYER_UI

void CPlayListItem::TryLoadLrcFile(std::tstring &filename,BOOL forceLoad)
{
	if (forceLoad || LrcFileMacth(filename))//filename match
	{
		LrcMatchItem matchitem;
		matchitem.path=filename;

		LrcMng *m=LrcMng::Get();
		matchitem.match_type=(_match_type)m->MatchTrackAndLrcTags(this,filename);

		AddToLrcMatchList(matchitem);
		if (matchitem.match_type <GetHighestMatchLrc().match_type)
			SetHighestMatchLrc(matchitem);
	}

}



BOOL CPlayListItem::GetLrcFileFromLib(BOOL forceResearch)
{
	if (!forceResearch && m_bLrcFromLrcFile)
		return TRUE;

	ClearLrcMatchList();
	//clear highest match item , set not perfect
	LrcMatchItem item;
	item.match_type=invalide;
	SetHighestMatchLrc(item);

	for (auto i=MyLib::shared()->GetDataPaths().begin();i!=MyLib::shared()->GetDataPaths().end();i++)
	{
		TryLoadLrcFile(*i);
		if (!GetSearchLrcUntilEnd() && GetHighestMatchLrc().match_type==perfect)
			break;
	}


	//we finded
	auto matchlist=GetLrcMatchList();
	auto highest=GetHighestMatchLrc();
	if (matchlist.size()>0 && highest.match_type!=invalide)
	{
		lycPath=highest.path;
		m_bLrcFromLrcFile=TRUE;

		LrcMng::Get()->Open((LPTSTR)lycPath.c_str());

		//lyricFromLrcFile=LrcMng::Get()->lib;
		return TRUE;
	}

	return FALSE;
}



BOOL CPlayListItem::HaveKeywords(TCHAR *keywords) const
{	
	BOOL have=FALSE;
	TCHAR keywordsLower[128];
	_tcscpy(keywordsLower,keywords);
	_tcslwr(keywordsLower);

	if(
		HaveHeywordsNoCase(title,keywordsLower)||
		HaveHeywordsNoCase(artist,keywordsLower)||
		HaveHeywordsNoCase(album,keywordsLower)||
		HaveHeywordsNoCase(genre,keywordsLower)||
		HaveHeywordsNoCase(comment,keywordsLower)
		)
		have=TRUE;

	if(!m_bLrcFromLrcFile && m_bLrcInner)
		if(HaveHeywordsNoCase(title,keywordsLower))
			have=TRUE;
	return have;
}


//通过歌词文件名判断,是否与当前歌曲匹配
BOOL CPlayListItem::LrcFileMacth(std::tstring &lrcFile) const
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


UINT LrcMng::MatchTrackAndLrcTags(CPlayListItem * track,std::tstring &lrcpath)
{
	UINT matchFlag=perfect;

	if(! Open((LPTSTR)lrcpath.c_str()) )
	{
		matchFlag=invalide;
		return matchFlag;
	}


	if (ti.empty())
		matchFlag|=without_title;
	else
		if(StrCmpIgnoreCaseAndSpace(track->GetTitle(),ti) !=0)
			matchFlag|= title_mismatch;

	if (ar.empty() )
		matchFlag|=without_artist;
	else
		if(StrCmpIgnoreCaseAndSpace(track->GetArtist(),ar) !=0 )
			matchFlag|=artist_mismatch;

	return matchFlag;
}

#endif


//-----------------------------------------
//CPlayList
 CPlayList::CPlayList(void):
	topVisibleIndex(0),
	nItemSelected(0),m_bSearch(FALSE),m_bAuto(FALSE)
{
}


CPlayList::CPlayList(std::tstring &name):m_playlistName(name),topVisibleIndex(0),
		nItemSelected(0),m_bSearch(FALSE),m_bAuto(FALSE)
{
	m_playlistName=name;
}



CPlayList::~CPlayList(void)
{	
	DeleteAllItems();
}

LPCPlayListItem CPlayList::GetItem(int nItem) const 
{
	if(nItem<0 || nItem >= GetItemCount())
	{
		return NULL;
	}
	else
		return m_songList[nItem];
}

void CPlayList::ChangeTrackPath(TCHAR *from,TCHAR *to)
{
	for (auto i=m_songList.begin();i!=m_songList.end();++i)
	{
		LPCPlayListItem item=*i;
		if (_tcscmp(from,item->GetUrl().c_str())==0)
		{
			item->SetUrl(to);
			break;
		}
	}
}

void CPlayList::DeleteTrackByPath(TCHAR *path)
{
	if(GetItemCount()>0)
		for (auto i=m_songList.begin();i!=m_songList.end();++i)
		{
			LPCPlayListItem item=*i;
			if (_tcscmp(path,item->GetUrl().c_str())==0)
			{
				//先删除视图里的显视,再删除数据
				NotifyMsg(WM_PL_WILL_DELETED,FALSE,(WPARAM)this,0);
				m_songList.erase(i);
				break;
			}
		}
}

void CPlayList::DeleteItem(int nItem)
{
	m_songList.erase(m_songList.begin()+nItem);
}

// void CPlayList::DeleteTrack(int nItem,int nLastItem)
// {
// 	m_songList.erase(m_songList.begin()+nItem,m_songList.begin()+nLastItem);
// }

void CPlayList::DeleteAllItems()
{
	m_songList.clear();
}

// void CPlayList::DeleteTrack(CPlayListItem* track)
// {
// 	if(track)
// 		m_songList.erase(m_songList.begin()+track->indexInListView);
// }

HANDLE CPlayList::AddFolderByThread(LPCTSTR pszFolder)
{
	PLANDPATH* p=new PLANDPATH;
	p->pPlaylist=this;
	int len = _tcslen(pszFolder);
	p->pszFolder= new TCHAR[ len];

	//todo delete p->pszFolder
	_tcscpy(p->pszFolder, pszFolder);


	if (this->IsAuto())
	{
		MyConfigs* s = GetMyConfigs();
		FILETIME fileTime;

		if (GetFileLastWriteTime(pszFolder,fileTime))
		{
			s->setTimeForUpdateMediaLib(fileTime);
		}

	}



	return hAddDir=::CreateThread(NULL,NULL,AddFolderThreadProc,(LPVOID)p,
		NULL,NULL);
}


void CPlayList::Reserve(int count)
{
	m_songList.reserve(count);
}


void CPlayList::AddItem(LPCPlayListItem item)
{
	item->SetPlayList(this);
	item->SetIndex(m_songList.size());
	m_songList.push_back(item);
}

void CPlayList::resetFileTime()
{
	SYSTEMTIME sT;
	GetSystemTime(&sT);

	SystemTimeToFileTime(&sT, &fileTime);
}

BOOL CPlayList::AddFile(TCHAR *filepath)
{
// 	//if 'filepath' not include the dir name , added it. 
// 	if(!_tcschr(filepath,'\\'))
// 	{
// 		TCHAR szPath[MAX_PATH]={0};
// 		_tgetcwd(szPath,MAX_PATH);
// 		_tcscpy(szPath,filepath);
// 		filepath=szPath;
// 	}

	std::tstring str(filepath);
	LPCPlayListItem  item(new CPlayListItem(this,str));
	
	//OutputDebugString(filepath);
	
	if(item->ScanId3Info(FALSE,TRUE))
	{
		item->SetIndex(m_songList.size());
		m_songList.push_back(item);
		NotifyMsg(WM_FILE_FINDED,FALSE,(WPARAM)filepath,(LPARAM)2);
		return TRUE;
	}
	return FALSE;
}

int CPlayList::AddFolder(LPCTSTR pszFolder,BOOL bIncludeDir)
{
	//忽略了子目录下的mp3文件
	TCHAR* oldPath=new TCHAR[MAX_PATH];
	_tgetcwd(oldPath,MAX_PATH);

	//改变当前目录
	_tchdir(pszFolder);

	BOOL findResult=FALSE;
	WIN32_FIND_DATA findFileData;
	HANDLE hFind;
	int fileAdded=0;
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
					fileAdded+=AddFolder(findFileData.cFileName,bIncludeDir);
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
					
					fileAdded += AddFile(pathName);

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

	return fileAdded;
}


LPCPlayListItem CPlayList::GetNextTrackByOrder(int curr , PlayOrder playorder) const
{
	int count=GetItemCount();
	int next=0;

	if (playorder==Default)
	{
		next=++curr;
	}
	else if (playorder==Repeat_playlist)
	{
		next=++curr;
		if(next==count)
			next=0;
	}
	else if (playorder==Repeat_track)
	{
		next=curr;
	}
	else if (playorder==Random)
	{
		static int s=0;
		if(s++==0)
			srand(time(NULL));

		int n=rand() % (count) - 1;
		next= n;
	}
	else if (playorder==Shuffle_tracks)
	{
		next=++curr;
	}
	else if (playorder==Shuffle_albums)
	{
		next=++curr;
	}
	else if (playorder==Shuffle_folders)
	{
		next=++curr;
	}

	return GetItem(next);
}







void CPlayList::SortItems(CompareProc compFunc)
{
	//两项相等的情况下,STL的DEBUG检验会失败.
#ifdef _DEBUG
	reverse(m_songList.begin(),m_songList.end());
#else
	sort(m_songList.begin(),m_songList.end(),compFunc);
#endif

	
	//重新分配序号
	int i=0;
	for (auto iter=m_songList.begin();iter!=m_songList.end();++i,++iter)
		(*iter)->SetIndex(i);
}


void CPlayList::ReverseItems()
{
	reverse(m_songList.begin(),m_songList.end());
}



int  CPlayList::RemoveDeadItems()
{
	int removed=0;
	int count=0;
	auto end=m_songList.end();
	for ( auto i=m_songList.begin(); i!=end; ++count )
	{
		LPCPlayListItem item=*i;
		if(!item->IsFileExist())
		{
			i=m_songList.erase(i);
			item=*i;
			end=m_songList.end();
			++removed;
		}
		else
			++i;

		item->SetIndex(count);
	}


	return removed;
}



int  CPlayList::RemoveDuplicates()
{
	auto songlist2=m_songList;

//#ifdef RELEASE
	sort(songlist2.begin(),songlist2.end(),PlayListItemComp);
//#endif

	auto _First = songlist2.begin();
	auto _Last = songlist2.end();
	int removed = 0 ;
	if (_First != _Last)
		for (_songContainer::iterator _Firstb ; (_Firstb = _First), ++_First != _Last; )
			if (IsPlayListItemDup(*_Firstb, *_First))
			{	
				m_songList.erase( find( m_songList.begin(), m_songList.end(), *_Firstb ) );
				_Last=songlist2.end();
				++removed;
			}
			

	// reset the index
	int count=0;
	auto end=m_songList.end();
	for (auto i=m_songList.begin();i!=end;++i,++count)
		(*i)->SetIndex(count);

	return removed;
}



map<std::tstring, LPCPlayListItem> _map;
void isFileInMap(VOID *caller, LPCTSTR pszFile)
{
	LPCPlayList playlist =static_cast<LPCPlayList>(caller);
	std::tstring path(pszFile);
	if (_map.find(path) != _map.end())
	{
		//file is already in playlist . skip it.
	}
	else
	{
		addFileToPlaylist(playlist, pszFile);
	}
}

void rescanMediaLibrary(LPCPlayList playlist , LPCTSTR pszFolder)
{
	///update the item in playlist .
	int itemCount= playlist->GetItemCount();
	
	for (int i = 0; i < itemCount; )
	{
		LPCPlayListItem item = playlist->GetItem(i);
		if (!item->IsFileExist())
		{
			playlist->DeleteItem(i);
			itemCount = playlist->GetItemCount();
			if (i<itemCount)
				item = playlist->GetItem(i);
		}
		else
		{
			item->ScanId3Info(FALSE,FALSE);

			_map[item->GetUrl()] = item;
			++i;
		}

		if (i < itemCount)
			item->SetIndex(i);
	}



	///add the item not in playlist.
	enumMusicFileInFolder(pszFolder, TRUE, isFileInMap, (void*)playlist);
}


void enumMusicFileInFolder(LPCTSTR pszFolder, BOOL bIncludeDir, enumFileCallBack callback, VOID *caller)
{
		//忽略了子目录下的mp3文件
		TCHAR* oldPath = new TCHAR[MAX_PATH];
		_tgetcwd(oldPath, MAX_PATH);

		//改变当前目录
		_tchdir(pszFolder);

		BOOL findResult = FALSE;
		WIN32_FIND_DATA findFileData;
		HANDLE hFind;
		int fileAdded = 0;
		hFind = ::FindFirstFile(_T("*"), &findFileData);
		if (hFind != INVALID_HANDLE_VALUE)
		{
			findResult = TRUE;

			while (findResult)
			{
				if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM)
				{
					//system file, e.recycled, should ignore it
				}
				//目录
				else if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY &&
					!IsDots(findFileData.cFileName))
				{
					if (bIncludeDir)
						/*fileAdded += */ enumMusicFileInFolder(findFileData.cFileName, bIncludeDir , callback , caller);
				}
				else//文件
				{
					TCHAR *mp3File = _T(".mp3");
					TCHAR *wavFile = _T(".wav");
					if (StrIsEndedWith(findFileData.cFileName, mp3File) ||
						StrIsEndedWith(findFileData.cFileName, wavFile))
					{
						TCHAR *_findName = new TCHAR[MAX_PATH];
						memset(_findName, 0, (MAX_PATH)*sizeof(TCHAR));
						_tcscpy(_findName, findFileData.cFileName);

						TCHAR* pathName = new TCHAR[MAX_PATH * 2];
						memset(pathName, 0, (MAX_PATH * 2)*sizeof(TCHAR));
						_tgetcwd(pathName, MAX_PATH * 2);

						_tcscat(pathName, _T("\\"));
						_tcscat(pathName, _findName);

						callback(caller,pathName);

						//fileAdded += AddFile(pathName);

						delete[] _findName;
						delete[] pathName;
					}
				}

				findResult = FindNextFile(hFind, &findFileData);
			}//while(findResult)

			FindClose(hFind);
		}//if(hFind!=INVALID_HANDLE_VALUE)


		_tchdir(oldPath);
		delete[] oldPath;

		//return fileAdded;
}



void addFolderToPlaylist(LPCPlayList playlist, LPCTSTR pszFolder)
{
	enumMusicFileInFolder(pszFolder, TRUE, addFileToPlaylist, (void*)playlist);
}


void addFileToPlaylist(VOID* playlist, LPCTSTR pszMusicFile)
{
	LPCPlayList pl = static_cast<LPCPlayList>(playlist);
	pl->AddFile((TCHAR*)pszMusicFile);
}

