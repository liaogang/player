#pragma once
#include "MySerialize.h"
#include "LrcMng.h"
#include "fileMonitor.h"


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
#include <attachedpictureframe.h>

class PlayList;
class CPlayListView;
/*
PlayListItem is outside of playlistitemproxy
PlayListItemProxy is true data item ,datas from file
, count based
if count =0 , it will dealloced
*/


class FileTrack: public SerializeObj
{
public:
	virtual int SerializeB(FILE* pFile);
	virtual int ReSerialize(FILE* pFile);
private:
	//PlayListItem *parent;
public:
	FileTrack():playCount(0),starLvl(0)
		,pPicBuf(NULL),img(NULL)
		,m_bLrcInner(FALSE),m_bLrcFromLrcFile(FALSE)
		,bUnsynLyc(FALSE),m_bStatus(UNKNOWN),year(_T("???"))
	{

	}


	FileTrack(std::tstring &url):url(url),
		playCount(0),starLvl(0)
		,pPicBuf(NULL),img(NULL)
		,m_bLrcInner(FALSE),m_bLrcFromLrcFile(FALSE)
		,bUnsynLyc(FALSE),m_bStatus(UNKNOWN),year(_T("???"))
	{

	}

	~FileTrack();


public:
	std::tstring url;
	UINT          playCount;
	UINT		  starLvl;
	std::tstring  title,artist,album,genre,year,comment;

	TagLib::ByteVector *pPicBuf;
	CImage *img;
	//cimg_library_suffixed::CImg   *img;

	//lyrics stuffs
	BOOL m_bLrcInner;
	std::tstring  lyricInner;

	BOOL m_bLrcFromLrcFile;
	std::tstring  lycPath;
	LrcLines      lyricFromLrcFile;

	BOOL bUnsynLyc;
public:

	enum ID3Status
	{
		UNKNOWN,
		INVALIE,
		ID3V2,
		ID3V1
	};
	ID3Status m_bStatus;

	BOOL  ScanId3Info( BOOL bRetainPic=FALSE,BOOL forceRescan=TRUE);
	const TCHAR* GetTitle(){return title.c_str();}
	BOOL  GetLrcFileFromLib(BOOL forceResearch=FALSE);
	BOOL  HaveKeywords(TCHAR *keywords);
	void TryLoadLrcFile(std::tstring &filename,BOOL forceLoad=FALSE);
public:
	//void SetParent(PlayListItem *p){parent=p;}
	//PlayListItem *GetParent(){return parent;}
private:
	void Buf2Img(BYTE* lpRsrc,DWORD len);
	BOOL LrcFileMacth(std::tstring &filename);
};



class PlayListItem
 {
public:
	PlayListItem()
	{

	}

	PlayListItem(PlayList *playlist,std::tstring url):
	  pPL(playlist),filetrack(new FileTrack(url)),index(-1)
	  {
		  //pfiletrack=&filetrack;
		  //filetrack->SetParent(this);
	  }
	  PlayListItem(PlayList *playlist):
	  pPL(playlist),filetrack(new FileTrack()),index(-1)
	  {
		  // pfiletrack=&filetrack;
		  //filetrack->SetParent(this);
	  }

	  ~PlayListItem()
	  {

	  };

	  //Move¸³Öµ¿½±´
// 	  PlayListItem& operator=(PlayListItem&& b)
// 	  {
// 		  if (this!=&b)
// 		  {
// 			  this->SetPlayList(b.GetPlayList());
// 			  this->filetrack=b.filetrack;
// 		  }
// 
// 		  return *this;
// 	  }


	  bool operator==(const PlayListItem &other)
	  {
		  if(&other == this )
			  return true;
		  return false;
	  }

	  inline PlayList * GetPlayList(){return this->pPL;}
	  inline void  SetPlayList(PlayList *pPL){this->pPL=pPL;}

	  //proxy functions
	  int SerializeB(FILE* pFile){return filetrack->SerializeB(pFile);}
	  int ReSerialize(FILE* pFile){return filetrack->ReSerialize(pFile);}
	  BOOL  ScanId3Info( BOOL bRetainPic=FALSE,BOOL forceRescan=TRUE){return filetrack->ScanId3Info(bRetainPic,forceRescan);}
	  const TCHAR* GetTitle(){return filetrack->GetTitle();}
	  BOOL  GetLrcFileFromLib(BOOL forceResearch=FALSE){return filetrack->GetLrcFileFromLib(forceResearch);}
	  BOOL  HaveKeywords(TCHAR *keywords){return filetrack->HaveKeywords(keywords);}
	  void TryLoadLrcFile(std::tstring &filename,BOOL forceLoad=FALSE){return filetrack->TryLoadLrcFile(filename,forceLoad);}

FileTrack* GetFileTrack(){return filetrack.get();}
int GetIndex(){return index;}
void SetIndex(int index){this->index=index;}
private:
	PlayList *pPL;

	shared_ptr<FileTrack> filetrack;
	
	//debug
	//shared_ptr<FileTrack>* pfiletrack;
	//index in playlist
	int index;
};



typedef PlayListItem _songContainerItem;


class PlayList:
	public SerializeObj
{
public:
	virtual int SerializeB(FILE* pFile);
	virtual int ReSerialize(FILE* pFile);

public:
	typedef vector<_songContainerItem> _songContainer;
	_songContainer m_songList;
	int GetItemCount(){return m_songList.size();}

	_songContainerItem GetItem(int nItem){
		return m_songList[nItem];
	}
	// 	int  GetItem(_songContainerItem item)
	// 	{
	// 		int i;
	// 		for (i=0;i<m_songList.size() && m_songList[i]!=item;++i);
	// 		return i;
	// 	}

	std::tstring       m_playlistName;
	void Rename(TCHAR *newName){m_playlistName=newName;}
	std::tstring       m_saveLocation;

public:
	//void SetSelectedItem(int index){curSelectedItem=m_songList[index];}
	//void SetSelectedItem(_songContainerItem _item){curSelectedItem=_item;}
	//_songContainerItem SelectedItem(){return curSelectedItem;}

	void SetCurPlaying(_songContainerItem item,BOOL scanID3=FALSE);

	//_songContainerItem lastTrack(){return lastPlayingItem;}
	//PlayListItem* curTrack(){return curPlayingItem;};
	//void          setCurTrack(PlayListItem* _item){curPlayingItem=_item;}
	_songContainerItem nextTrack();
public:
	PlayList(void);
	PlayList(std::tstring &name);
	PlayList(std::tstring &name,bool bMonitor=false);
	~PlayList(void);

public:
	//operation
	_songContainerItem DeleteTrack(int nItem);
	void DeleteTrack(int nItem,int nLastItem);
	void DeleteTrackByPath(TCHAR *path);
	void ChangeTrackPath(TCHAR *from,TCHAR *to);
	//void DeleteTrack(PlayListItem* track);

public:
	HANDLE hAddDir;
	BOOL AddFolderByThread(LPCTSTR pszFolder);
	void scanAllId3Info();
	void TerminateAddDirThread();

	//return file added
	int AddFolder(LPCTSTR pszFolder,BOOL bIncludeDir=FALSE);
	
public:
	BOOL AddFile(TCHAR *filepath);
public:
	_songContainerItem GetNextTrackByOrder(BOOL bMoveCur=TRUE);


public:
	//this data will used in list view when display
	int topVisibleIndex;
	int selectedIndex;

	bool m_bMonitor;
	fileMonitors m_fileMonitor;

	

	CPlayListView *pPLV;
};