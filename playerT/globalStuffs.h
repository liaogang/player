#include "stdafx.h"
#include <string>
#include <vector>
#pragma once
class CMainFrame;
class CMultiSpliltWnd;
class CPlayList;
class CPlayListItem;

CMainFrame* GetMainFrame();
CMultiSpliltWnd* GetSplitter();

void SdMsg(UINT msg,BOOL bPost=TRUE,WPARAM wparam=NULL,LPARAM lparam=NULL);
const TCHAR *GetAppName();



// class MYTREE;
// void SetRootTree(MYTREE *root);
// MYTREE *UISplitterTreeRoot();



/************************************************************************/
/*                                                                      */
/************************************************************************/
#define PLAYLISTINDEXFILE _T("index.dat")
bool SaveAllPlayList();
bool LoadAllPlayList();

#define CFGFILENAME _T("core.cfg")

//Lrc and global configs
bool SaveCoreCfg();
bool LoadCoreCfg();

void CollectInfo();
void ValidateCfg();


#ifdef APP_PLAYER_UI
//Splitter window info
#define UIFILENAME  _T("ui.cfg")
bool LoadUICfg();
bool SaveUICfg();
#endif

void SaveAll();
bool LoadAll();



/************************************************************************/
/*                                                                      */
/************************************************************************/

//保存当前播放项目的序号
// int GetPlayingIndex();
// void SetPlayingIndex();

//是否搜索整个目录完为止,否的话,搜索到完全匹配的即可.
bool GetSearchLrcUntilEnd();
void SetSearchLrcUntilEnd(bool b);


enum _match_type
{
	perfect=0,
	without_artist=0x0001,
	without_title=0x0002,
	without_artist_title=without_artist| without_title,
	artist_mismatch=0x0004,
	title_mismatch=0x0008,
	artist_mismatch_no_title=artist_mismatch|without_title,
	title_artist_mismatch_no_artist=title_mismatch|without_artist,
	title_artist_mismatch=title_mismatch| artist_mismatch ,
	invalide=0xFFFF
};

struct LrcMatchItem
{
public:
	LrcMatchItem():match_type(invalide)
	{}
	std::wstring path;
	_match_type match_type;

	


	const TCHAR* LrcMatchItem::GetReason()
	{
		static const TCHAR* reason[]=
		{
			_T("完全匹配"),
			_T("~艺术家"),
			_T("~标题"),
			_T("~标题 ~艺术家"),
			_T("!艺术家"),
			_T("!标题"),
			_T("~标题!艺术家"),
			_T("!标题 ~艺术家"),
			_T("!标题 !艺术家"),
			_T("程序或目标歌词文件格式出错!")
		};

		int iMatch;
		switch (match_type)
		{
		case perfect:
			iMatch=0;
			break;
		case without_artist:
			iMatch=1;
			break;
		case without_title:
			iMatch=2;
			break;
		case without_artist_title:
			iMatch=3;
			break;
		case artist_mismatch:
			iMatch=4;
			break;
		case title_mismatch:
			iMatch=5;
			break;
		case artist_mismatch_no_title:
			iMatch=6;
			break;
		case title_artist_mismatch_no_artist:
			iMatch=7;
			break;
		case title_artist_mismatch:
			iMatch=8;
			break;
		case invalide:
			iMatch=9;
			break;
		}

		return reason[iMatch];
	}
};


std::vector<LrcMatchItem> GetLrcMatchList();
void ClearLrcMatchList();
void AddToLrcMatchList(LrcMatchItem &item);

LrcMatchItem GetHighestMatchLrc();
void SetHighestMatchLrc(LrcMatchItem &item);





#ifdef APP_PLAYER_UI
struct TrackFormatInfo
{
	int type;
	int rate;//kpbs
	DWORD  nSamplesPerSec;//Hz
	int mode; 
	const TCHAR* getModeString();
};
void SetTrackFormatInfo(int type,int rate,int samplesps,int mode);
TrackFormatInfo* GetTrackFormatInfo();



void RegisterStuffs();
#endif


struct blockData
{
private:
	BYTE *data;
	UINT   len;

	//abandoned assgin
	blockData operator=(const blockData &);
public:
	void Take(blockData &other)
	{
		len=other.len;   other.len=0;
		data=other.data; other.data=NULL;
	}

	void Copy(blockData &other)
	{
		len=other.len;
		data=new BYTE[len];
		memcpy(data,other.data,len);
	}

	blockData():data(NULL),len(0)
	{

	}

	blockData(int _len)
	{
		len=_len;
		data=new BYTE[len];
	}

	blockData(BYTE *_data,int _len):data(_data),len(_len)
	{
	}

	~blockData()
	{
		if(data)
			delete[] data;
	}

	INT GetLength() const 
	{
		return len;
	}

	void CopyDataIn(void *buf,int _len)
	{
		ATLASSERT(data==NULL);

		len=_len;
		data=new BYTE[len];
		memcpy(data,buf,len);
	}

	void CopyDataOut(void *buf,int _len) const
	{
		ATLASSERT(_len <= len);

		memcpy(buf,data,_len);
	}

	void Clear()
	{
		len=0;

		if(data)
		{
			delete[] data;
			data=NULL;
		}
	}
};

#define  RECT_INIT(rc) 	rc.left=0;rc.right=0;rc.top=0;rc.bottom=0;




struct trackPosInfo
{
	double used;
	double left;
public:
	trackPosInfo():used(-1),left(-1)
	{}
};


enum PlayingStatus
{
	status_invalide,
	status_stopped,
	status_playing,
	status_paused
};

