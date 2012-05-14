#include "StdAfx.h"
#include "PlayList.h"
#include "BasicPlayer.h"
#include "customMsg.h"



template <class T>
int Serialize (FILE *pFile,T t)
{
	
	return 0;
};

template <>
int Serialize (FILE *pFile,UINT t)
{
	fwrite(&t,1,4,pFile);
	return 4;
};


template <>
int Serialize (FILE *pFile,std::tstring t)
{
	const TCHAR* str=t.c_str();
	int size=t.length()*sizeof(TCHAR);
	int totalSize=size+4;

	fwrite(&totalSize,1,4,pFile);
	fwrite(str,1,size,pFile);
	return totalSize;
};

template <>
int Serialize (FILE *pFile,int t)
{
	fwrite(&t,1,4,pFile);
	return 4;
};

//-----------------------------------------

template <class T>
int ReSerialize (FILE *pFile,T *t)
{

	return 0;
};

template <>
int ReSerialize (FILE *pFile,std::tstring *str)
{
	int size;
	fread(&size,1,4,pFile);

	//string len is 0
	if(size-4<0)
	{
		MessageBox(MyLib::GetMain(),_T("Parse File Failed,Invalide Data"),_T(""),MB_OK);
		return 0;
	}
	else if (size-4==0)
	{
		*str=_T("");
	}
	else
	{
		int len=(size-4)/sizeof(TCHAR);
		TCHAR* pStr=new TCHAR[len];
		fread(pStr,sizeof(TCHAR),len,pFile);

		std::tstring tmp(pStr,pStr+len);
		delete[] pStr;
		(*str)=tmp;
	}

	return size;
};


template <>
int ReSerialize (FILE *pFile,int *pVal)
{
	fread(pVal,1,4,pFile);
	return 4;
};

template <>
int ReSerialize (FILE *pFile,uint *pVal)
{
	fread(pVal,1,4,pFile);
	return 4;
};




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


//-----------------------------------------
//

PlayList*  MyLib::NewPlaylist()
{
	PlayList *l=new PlayList;
	m_playLists.push_back(*l);
	return l;
}

void MyLib::AddFolderToCurrentPlayList(LPCTSTR pszFolder)
{
 	MyLib::curPlaylist()->AddFolderByThread(pszFolder);
}

MyLib* MyLib::shared()
{
	static MyLib* p=NULL;
	return p==NULL?p=new MyLib():p;
}

PlayList* MyLib::curPlaylist()
{
	return shared()->m_pActivePlaylist;
}

 void MyLib::play()
{
	CBasicPlayer::shared()->open(curPlaylist()->curTrack());
	CBasicPlayer::shared()->play();
}


 void MyLib::pause()
{
	CBasicPlayer::shared()->pause();
}

 void MyLib::stop()
{
	CBasicPlayer::shared()->stop();
}

//-----------------------------------------

 void MyLib::playNext()
{	
	PlayListItem *track=curPlaylist()->GetNextTrackByOrder();
	if (!track) return;

	CBasicPlayer *sbp=CBasicPlayer::shared();
	if ( track==curPlaylist()->lastTrack())
	{
		sbp->ResetFile();
	}
	else
	{
		sbp->stop();
		sbp->open(track);
		sbp->play();
	}

}

//-----------------------------------------
//PlayList
 PlayList::PlayList(void):curPlayingItem(NULL)
{
	index=Default;
}

 PlayList::PlayList(std::tstring name)
 {
	 PlayList();
	m_playlistName=name;
 }


PlayList::~PlayList(void)
{	
}



void PlayList::scanAllId3Info()
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
	
	// for each mp3 file in this folder
	// data . add

	//忽略了子目录下的mp3文件
	TCHAR* curPath=new TCHAR[256];
	_tgetcwd(curPath,256);

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

			PlayListItem *pItem=new PlayListItem(&str);
			pItem->ScanId3Info();
			m_songList.push_back(*pItem);
			//msonglist的析构会删除*pItem;
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

				PlayListItem *pItem=new PlayListItem(&str);
				pItem->ScanId3Info();
				m_songList.push_back(*pItem);
				//msonglist的析构会删除*pItem;
			}
		}
	}
	FindClose(hFind);


	_tchdir(curPath);

	return TRUE;
}



BOOL PlayListItem::ScanId3Info()
{
	BOOL bInvalidID3V2=FALSE;

	MPEG::File f(url.c_str());

	ID3v2::Tag *id3v2tag = f.ID3v2Tag();
	if(id3v2tag) 
	{
		title=id3v2tag->title().toWString();
		artist=id3v2tag->artist().toWString();
		album=id3v2tag->album().toWString();
		genre=id3v2tag->genre().toWString();

		year=id3v2tag->year();


		// we will use bytevector to retain to memory in frame
		pPicBuf=new ByteVector;
		id3v2tag->retainPicBuf(pPicBuf);

		//-----------------------------------------
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
		if (S_OK != img->Load(pstm))
		{
			delete img;
			img=NULL;
		}
		
		//-----------------------------------------

		if ( title.empty() && artist.empty() && album.empty())
			bInvalidID3V2=TRUE;
	}
	else
		bInvalidID3V2=TRUE;
	

	if(bInvalidID3V2)
	{
		ID3v1::Tag *id3v1tag = f.ID3v1Tag();
		if(id3v1tag) 
		{
			title=id3v1tag->title().toWString();
			artist=id3v1tag->artist().toWString();
			album=id3v1tag->album().toWString();
			genre=id3v1tag->genre().toWString();

			year=id3v1tag->year();
		}
	}

	return TRUE;
}



int MyLib::SerializeB(FILE *pFile)
{
	int size=0;
// 	list<PlayList>::iterator i;
// 	for (i=m_playLists.begin();i!=m_playLists.end();++i)
// 	{
// 		size+=(*i).Serialize(out);
// 	}
	size+=m_pActivePlaylist->Serialize(pFile);
	return size;
}

int MyLib::ReSerialize(FILE *pFile)
{
	int totalSize=0;
	fread(&totalSize,1,4,pFile);

	int size=m_pActivePlaylist->ReSerialize(pFile);

	return size;
}

 int PlayList::SerializeB(FILE *pFile)
{
	int size=0;

	//m_playlistName Serialize
	size+=::Serialize(pFile,m_playlistName);

	//m_songList Serialize
	list<PlayListItem>::iterator i;
	for (i=m_songList.begin();i!=m_songList.end();++i)
	{
		size+=(*i).Serialize(pFile);
	}

	return size;
}

 int PlayList::ReSerialize(FILE *pFile)
{
	int size=0;
	int totalSize=0;
	int playlistnameSize=0;
	fread(&totalSize,1,4,pFile);
	
	playlistnameSize=::ReSerialize(pFile,&m_playlistName);
	size+=playlistnameSize;

	while(size<totalSize-playlistnameSize)
	{
		PlayListItem *track=new PlayListItem;
		size+=track->ReSerialize(pFile);
		m_songList.push_back(*track);
	}
	

	

	return size;
}


 int PlayListItem::SerializeB(FILE *pFile)
 {
	 int size=0;
	 size+=::Serialize(pFile,url);
	 size+=::Serialize(pFile,playCount);
	 size+=::Serialize(pFile,starLvl);
  	 size+=::Serialize(pFile,title);
 	 size+=::Serialize(pFile,artist);
	 size+=::Serialize(pFile,album);
	 size+=::Serialize(pFile,genre);
	 size+=::Serialize(pFile,comment);
	 size+=::Serialize(pFile,year);
	 size+=::Serialize(pFile,indexInListView);
	return size;
 }

 int PlayListItem::ReSerialize(FILE *pFile)
 {
	int size=0;

	int totalSize=0;
	fread(&totalSize,1,4,pFile);
	size+=4;

	size+=::ReSerialize(pFile,&url);
	size+=::ReSerialize(pFile,&playCount);
	size+=::ReSerialize(pFile,&starLvl);
	size+=::ReSerialize(pFile,&title);
	size+=::ReSerialize(pFile,&artist);
	size+=::ReSerialize(pFile,&album);
	size+=::ReSerialize(pFile,&genre);
	size+=::ReSerialize(pFile,&comment);
	size+=::ReSerialize(pFile,&year);
	size+=::ReSerialize(pFile,&indexInListView);
	return size;
 }