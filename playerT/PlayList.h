#pragma once
#include "stdafx.h"
#include "MySerialize.h"
#include <string>
#include <vector>

#ifdef APP_PLAYER_UI
//for parse the ID3 tag
#include <direct.h>
#include <stdlib.h>
#include <mpegfile.h>
#include <id3v2tag.h>
#include <id3v2frame.h>
#include <id3v2header.h>
#include <taglib.h>
#include <tbytevector.h>
#include <id3v1tag.h>
#include <apetag.h>
#include <fileref.h>
#include <tbytevector.h>
#include <attachedpictureframe.h>
#endif



class CPlayList;
class CPlayListItem;
typedef shared_ptr<CPlayListItem>  LPCPlayListItem;
typedef vector<LPCPlayListItem> _songContainer;
typedef  CPlayList *  LPCPlayList;
typedef  bool (*CompareProc)(LPCPlayListItem a, LPCPlayListItem b);

enum PlayOrder
{
	Default,
	Repeat_playlist,
	Repeat_track,
	Random,
	Shuffle_tracks,
	Shuffle_albums,
	Shuffle_folders,
};

class CPlayListItem:public SerializeObj<CPlayListItem>
 {
public:
	CPlayListItem():m_iIndex(-1),playCount(0),starLvl(0)
		,m_bLrcInner(FALSE),m_bLrcFromLrcFile(FALSE)
		,bUnsynLyc(FALSE),m_bStatus(UNKNOWN),year(_T("???")),uYear(0),m_pPL(NULL)
#ifdef APP_PLAYER_UI
		,img(NULL),pPicBuf(NULL)
#endif
	{

	}

	CPlayListItem(std::tstring &url):url(url),
		playCount(0),starLvl(0)
		,m_bLrcInner(FALSE),m_bLrcFromLrcFile(FALSE)
		,bUnsynLyc(FALSE),m_bStatus(UNKNOWN),year(_T("???")),uYear(0),m_pPL(NULL)
#ifdef APP_PLAYER_UI
		,img(NULL),pPicBuf(NULL)
#endif
	{

	}

	CPlayListItem(LPCPlayList pPl,std::tstring &url):url(url),
		playCount(0),starLvl(0)
		,m_bLrcInner(FALSE),m_bLrcFromLrcFile(FALSE)
		,bUnsynLyc(FALSE),m_bStatus(UNKNOWN),year(_T("???")),uYear(0),m_pPL(pPl)
#ifdef APP_PLAYER_UI
		,img(NULL),pPicBuf(NULL)
#endif
	{

	}

	~CPlayListItem();
 private:
	 std::tstring url;
	 UINT          playCount;
	 UINT		  starLvl;

	 std::tstring  title,artist,album,genre,year,comment;
	 UINT                                   uYear;

#ifdef APP_PLAYER_UI
	 TagLib::ByteVector *pPicBuf;
#endif	 
	 //cimg_library_suffixed::CImg   *img;

	 //lyrics stuffs
	 BOOL m_bLrcInner;
	 std::tstring  lyricInner;

	 BOOL m_bLrcFromLrcFile;
	 std::tstring  lycPath;

	 BOOL bUnsynLyc;

	 enum ID3Status
	 {
		 UNKNOWN,
		 INVALIE,
		 ID3V2,
		 ID3V1
	 };
	 ID3Status m_bStatus;
 
	 CPlayList *m_pPL;
	 int m_iIndex;


 public:
	 FILE& operator<<(FILE& f);
	 FILE& operator>>(FILE& f) const ;

	 BOOL  IsFileExist() const;


	bool isValide() const {return GetIndex()!=-1;}

	inline CPlayList * GetPlayList() const {return m_pPL;}
	inline void  SetPlayList(CPlayList *pPL){m_pPL=pPL;}
	int GetIndex() const     {return m_iIndex;}
	void SetIndex(int _index){m_iIndex=_index;}
	void SetUrl(TCHAR *s){url=s;}
	void SetUrl(tstring &s){url=s;}
	const tstring& GetUrl() const {return url;}
	const tstring& GetTitle() const{return title;}
	const tstring& GetArtist()const{return artist;}
	const tstring& GetAlbum()const{return album;}
	const tstring& GetGenre()const{return genre;}
	const tstring& GetYear()const{return year;}
	const tstring& GetComment()const{return comment;}
	const tstring& GetLycPath()const{return lycPath;}
	BOOL    IsLyricFromFile() const {return m_bLrcFromLrcFile;}
	BOOL  ScanId3Info ( BOOL bRetainPic=FALSE,BOOL forceRescan=TRUE);

#ifdef APP_PLAYER_UI
	 void Buf2Img(BYTE* lpRsrc,DWORD len);
	 CImage* GetImg(){return img;}
	 CImage *img;

	 
	 void  ClearImgBuf();
	 BOOL  GetLrcFileFromLib (BOOL forceResearch=FALSE);
	 BOOL  HaveKeywords (TCHAR *keywords) const;
	 void TryLoadLrcFile(std::tstring &filename,BOOL forceLoad=FALSE) ;

 private:
	 BOOL LrcFileMacth(std::tstring &filename) const;
	 #endif
};





class CPlayList:
	public SerializeObj<CPlayList>
{
public:
	CPlayList(void);
	CPlayList(std::tstring &name);
	~CPlayList(void);
private:
	_songContainer m_songList;
	std::tstring       m_playlistName;
	HANDLE hAddDir;
	BOOL m_bAuto;//自动播放列表？
	BOOL m_bSearch;//是否为搜索列表
	int topVisibleIndex;//this data will used in list view when display
	int nItemSelected;
public:
	FILE& operator<<(FILE& f);
	FILE& operator>>(FILE& f) const ;


	void SetSelectedIndex(int i){nItemSelected=i;}
	void SetTopVisibleIndex(int i){topVisibleIndex=i;}
	void SetAuto(BOOL a){m_bAuto=a;}
	void SetSearch(BOOL s)  {m_bSearch=s;}
	void SetPlaylistName(std::tstring name){m_playlistName=name;}


	BOOL IsAuto()const {return m_bAuto;}
	BOOL IsSearch() const {return m_bSearch;}
	int  GetSelectedIndex() const {return nItemSelected;}
	LPCPlayListItem GetSelectedItem() const{return GetItem(nItemSelected);}
	int  GetTopVisibleIndex() const{return topVisibleIndex;}
	std::tstring GetPlaylistName(){return m_playlistName;}



	int GetItemCount() const {return m_songList.size();}
	LPCPlayListItem GetItem(int nItem) const ;

	void Rename(TCHAR *newName){m_playlistName=newName;}
	//operation
	void SortItems(CompareProc compFunc);
	void ReverseItems();
	void DeleteItem(int nItem);
	void DeleteAllItems();
	void DeleteTrackByPath(TCHAR *path);
	void ChangeTrackPath(TCHAR *from,TCHAR *to);
	int  RemoveDeadItems();
	int  RemoveDuplicates();
	LPCPlayListItem GetNextTrackByOrder(int curr ,PlayOrder playorder) const;

	HANDLE AddFolderByThread(LPCTSTR pszFolder);
	void TerminateAddDirThread();

	//return file added
	int AddFolder(LPCTSTR pszFolder,BOOL bIncludeDir=FALSE);
	BOOL AddFile(TCHAR *filepath);
	void Reserve(int count);
	void AddItem(LPCPlayListItem item);
};


LPCPlayListItem MakeDuplicate(const LPCPlayListItem item);